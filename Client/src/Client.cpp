/*******************************************************************************
 * Filename      : Client.cpp
 * Header File(s): Client.hpp
 * Description   :
 * Authors(s)    : 
 * Date Created  : 
 * Date Modified :
 * Modifier(s)   :
 *******************************************************************************/
/************************************
 * Included Headers 
 ************************************/
#include "Client.hpp"
#include "TestFunctions.hpp"
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <boost/bind.hpp>
#include<boost/algorithm/string/split.hpp>                                      
#include<boost/algorithm/string.hpp>    
#include <iostream>
#include <vector>

/************************************
 * Namespaces 
 ************************************/
using namespace std;
using namespace TestFunctions;
using boost::asio::deadline_timer;
using boost::asio::ip::tcp;

/************************************
 * Local Variables 
 ************************************/

/*******************************************************************************
* Constructor  : 
* Description  : 
* Arguments    : 
* Remarks      : 
********************************************************************************/
Client::Client ( boost::asio::io_service& io_service ) : _stopped(false),
                                                           _socket(io_service),
                                                           _deadline(io_service)
#if HEARTBEAT
        
                                                           ,_heartbeat_timer(io_service)
#endif
{
    ;
}
/*******************************************************************************
* Deconstructor: 
* Description  : 
* Arguments    : 
* Remarks      : 
********************************************************************************/
Client::~Client ( void ) { ; }
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Client::start ( tcp::resolver::iterator endpoint_iter )
{
    // Called by the user of the client class to initiate the connection process.
    // The endpoint iterator will have been obtained using a tcp::resolver.
    
    // Start the connect actor.
    start_connect(endpoint_iter);

    // Start the deadline actor. You will note that we're not setting any
    // particular deadline here. Instead, the connect and input actors will
    // update the deadline prior to each asynchronous operation.
    _deadline.async_wait(boost::bind(&Client::check_deadline, this));
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Client::stop ( void )
{
    // This function terminates all the actors to shut down the connection. It
    // may be called by the user of the client class, or by the class itself in
    // response to graceful termination or an unrecoverable error.    
    _stopped = true;
    _socket.close();
    _deadline.cancel();
#if HEARTBEAT
    _heartbeat_timer.cancel();
#endif
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Client::start_connect ( tcp::resolver::iterator endpoint_iter )
{
    if (endpoint_iter != tcp::resolver::iterator())
    {
        cout << "Trying " << endpoint_iter->endpoint() << "..." << endl;

        // Set a deadline for the connect operation.
        _deadline.expires_from_now(boost::posix_time::seconds(60));

        // Start the asynchronous connect operation.
        _socket.async_connect(endpoint_iter->endpoint(),
                              boost::bind(&Client::handle_connect,
                                          this, 
                                          _1, 
                                          endpoint_iter));
    }
    else
    {
        // There are no more endpoints to try. Shut down the client.
        stop();
    }
}

/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Client::handle_connect ( const boost::system::error_code& ec, 
                                tcp::resolver::iterator endpoint_iter )
{
    if (_stopped)
    {
        return;
    }

    // The async_connect() function automatically opens the socket at the start
    // of the asynchronous operation. If the socket is closed at this time then
    // the timeout handler must have run first.
    if ( !_socket.is_open() )
    {
      cout << "Connect timed out" << endl;

      // Try the next available endpoint.
      start_connect(++endpoint_iter);
    }
    else if ( ec ) // Check if the connect operation failed before the deadline expired.
    {
        cout << "Connect error: " << ec.message() << endl;

        // We need to close the socket used in the previous connection attempt
        // before starting a new one.
        _socket.close();

        // Try the next available endpoint.
        start_connect(++endpoint_iter);
    }
    else // Otherwise we have successfully established a connection.
    {
        cout << "Connected to " << endpoint_iter->endpoint() << endl;

        // Start the input actor.
        start_read();
#if HEARTBEAT
        // Start the heartbeat actor.
        start_write();
#endif
    }
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Client::start_read ( void )
{
    // Set a deadline for the read operation.
    _deadline.expires_from_now(boost::posix_time::seconds(30));

    // Start an asynchronous operation to read a newline-delimited message.
    boost::asio::async_read_until(_socket,
                                  _input_buffer, 
                                  '\n',
                                 boost::bind(&Client::handle_read, 
                                            this, 
                                            _1));
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Client::handle_read ( const boost::system::error_code& ec )
{
    if (_stopped)
    {
        return;
    }

    if (!ec)
    {
        // Extract the newline-delimited message from the buffer.
        string line;
        istream is(&_input_buffer);
        getline(is, line);
        stringstream ss;
        vector<string> strs;

        // Empty messages are heartbeats and so ignored.
        if (!line.empty())
        {
            switch ( line[0] )
            {
                case TestFunctions::DO_ADD:
                {
                    boost::algorithm::split(strs,line,boost::is_any_of(","));
                    int value = _tf.do_add(atoi(strs[1].c_str()),
                                           atoi(strs[2].c_str()));
                    ss << value;
                    _reply_message = (ss.str()+"\n");
                }
                    break;
                case TestFunctions::DO_STR:
                {
                    boost::algorithm::split(strs,line,boost::is_any_of(","));
                    string value = _tf.do_string_cat(strs[1], strs[2]);
                    _reply_message = (value+"\n");
                    
                }
                    break;
                case TestFunctions::DO_MUL:
                {
                    boost::algorithm::split(strs,line,boost::is_any_of(","));
                    int value = _tf.do_multipy(atoi(strs[1].c_str()),
                                               atoi(strs[2].c_str()));
                    ss << value;
                    _reply_message = (ss.str()+"\n");
                }
                    break;
                default:
                    cout << "NULL" << endl;
                    break;
            }            
        }
        
        ss.str(string());
        strs.clear();

        start_write();
        start_read();
    }
    else
    {
        cout << "Error on receive: " << ec.message() << endl;

        stop();
    }
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Client::start_write ( void )
{
    if (_stopped)
    {
        return;
    }
    
    // Start an asynchronous operation to send a heartbeat message.
    boost::asio::async_write(_socket,
#if HEARTBEAT
                             boost::asio::buffer("\n"),
#else
                             boost::asio::buffer(_reply_message),
#endif
                             boost::bind(&Client::handle_write, 
                                         this, 
                                         _1));
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Client::handle_write (const boost::system::error_code& ec )
{
    if (_stopped)
    {
        return;
    }

    if (!ec)
    {
#if HEARTBEAT
        // Wait 10 seconds before sending the next heartbeat.
        _heartbeat_timer.expires_from_now(boost::posix_time::seconds(10));
        _heartbeat_timer.async_wait(boost::bind(&Client::start_write, this));
#endif
    }
    else
    {
        cout << "Error on heartbeat: " << ec.message() << endl;

        stop();
    }
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Client::check_deadline ( void )
{
    if (_stopped)
    {
        return;
    }

    // Check whether the deadline has passed. We compare the deadline against
    // the current time since a new asynchronous operation may have moved the
    // deadline before this actor had a chance to run.
    if (_deadline.expires_at() <= deadline_timer::traits_type::now())
    {
        // The deadline has passed. The socket is closed so that any outstanding
        // asynchronous operations are cancelled.
        _socket.close();

        // There is no longer an active deadline. The expiry is set to positive
        // infinity so that the actor takes no action until a new deadline is set.
        _deadline.expires_at(boost::posix_time::pos_infin);
    }

    // Put the actor back to sleep.
    _deadline.async_wait(boost::bind(&Client::check_deadline, this));    
}