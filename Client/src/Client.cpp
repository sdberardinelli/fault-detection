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
#include <boost/algorithm/string/split.hpp>                                      
#include <boost/algorithm/string.hpp> 
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <iostream>
#include <vector>
#include <valarray>

/************************************
 * Namespaces 
 ************************************/
using namespace std;
using namespace TestFunctions;
using namespace Distributions;
using namespace boost::filesystem;
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
    tcp::resolver r(io_service);
    vector<string> strs;
    valarray<double> parameters;
    std::string str,ip,port; 
    boost::filesystem::ifstream file_in;
    file_in.open(CONFIG_FILENAME,ios::in);

    if ( !file_in )
    {
        _fault = false;
        _dist_type = NONE;
    }
    else
    {
        try
        {
            getline(file_in, str);
            boost::algorithm::split(strs,str,boost::is_any_of("="));
            ip = strs[1];
            getline(file_in, str);
            boost::algorithm::split(strs,str,boost::is_any_of("="));   
            port = strs[1];
            getline(file_in, str);
            boost::algorithm::split(strs,str,boost::is_any_of("="));
            _fault = (strs[1].compare(("true"))?false:true);
            getline(file_in, str);
            boost::algorithm::split(strs,str,boost::is_any_of("="));
            _dist_type = DistToEnum(strs[1].c_str());
            getline(file_in, str);
            boost::algorithm::split(strs,str,boost::is_any_of("="));
            boost::algorithm::split(strs,strs[1],boost::is_any_of(","));
            
            parameters.resize(strs.size());
            for ( vector<int>::size_type i = 0; i != strs.size(); i++ )
            {
                parameters[i] = atof(strs[i].c_str());
            }
            
            _dist.set_parameters(parameters);
            _dist.construct_distributions();
        }
        catch(const boost::filesystem::ifstream::failure & ex)
        {
            _fault = false;
            _dist_type = NONE;
        }
    }

    this->start(r.resolve(tcp::resolver::query(ip, port)));
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
#if DEBUG
        cout << "Trying " << endpoint_iter->endpoint() << "..." << endl;
#endif
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
#if DEBUG
        cout << "Connected to " << endpoint_iter->endpoint() << endl;
#endif
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
        vector<string> strs;
        istream is(&_input_buffer);
        getline(is, line);

        // Empty messages are heartbeats and so ignored.
        if (!line.empty())
        {          
            boost::algorithm::split(strs,line,boost::is_any_of(","));
            process_message(ToEnum(strs[0].c_str()),strs);
        }
        
        strs.clear();
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
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Client::process_message ( TEST_FUNCTIONS CMD,
                               std::vector<std::string>& strs )
{
    stringstream ss;
    double probability;
    string param1 = strs[1], param2 = strs[2];
    static const char modification[] = "0123456789";
    
    if ( _fault )
    {
        switch ( _dist_type )
        {
            case BETA:
                probability = _dist.beta();
                break;
            case UNIFORM:
                probability = _dist.uniform();
                break;
            case NORMAL:
                probability = _dist.normal();
                break;
            case GAMMA:
                probability = _dist.gamma();
                break;
            default:
                probability = -1.0;
                break;
        }
        
        if ( double(rand()/double(RAND_MAX)) < probability )
        {
            if ( rand()/double(RAND_MAX) < 0.51 )
                param1[rand()%(param1.length()-1)] = modification[rand()%(sizeof(modification)-1)];
            if ( rand()/double(RAND_MAX) < 0.51 )
                param2[rand()%(param2.length()-1)] = modification[rand()%(sizeof(modification)-1)];
        }
        else
        {
            valarray<double> parameters = _dist.get_parameters();
            
            try
            {
                param1[int(parameters[0])] = modification[int(parameters[2])];
                param2[int(parameters[1])] = modification[int(parameters[2])];
            }
            catch ( const out_of_range& oor )
            {
                param1[0] = modification[1];
                param2[0] = modification[1];                
            }
        }
    }
        
    switch ( CMD )
    {
        case TestFunctions::DO_ADD:
        {        
            int value = _tf.do_add(atoi(param1.c_str()), atoi(param2.c_str()));
            ss << value;
            _reply_message = (ss.str()+"\n");
        }
            break;
        case TestFunctions::DO_STR:
        {
            string value = _tf.do_string_cat(param1, param2);
            _reply_message = (value+"\n");

        }
            break;
        case TestFunctions::DO_MUL:
        {
            int value = _tf.do_multipy(atoi(param1.c_str()), atoi(param2.c_str()));
            ss << value;
            _reply_message = (ss.str()+"\n");
        }
            break;
        default:;
            break;
    }  
    
    
    ss.str(string());
    
    start_write();
    start_read();    
}