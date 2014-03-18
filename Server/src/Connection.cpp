/*******************************************************************************
 * Filename      : Connection.cpp
 * Header File(s): Connection.hpp
 * Description   :
 * Authors(s)    : 
 * Date Created  : 
 * Date Modified :
 * Modifier(s)   :
 *******************************************************************************/
/************************************
 * Included Headers 
 ************************************/
#include "Connection.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

/************************************
 * Namespaces 
 ************************************/
using namespace std;

/************************************
 * Local Variables 
 ************************************/

/*******************************************************************************
* Constructor  : 
* Description  : 
* Arguments    : 
* Remarks      : 
********************************************************************************/
Connection::Connection ( boost::asio::io_service & io )  : _socket(io) { ; }
/*******************************************************************************
* Deconstructor: 
* Description  : 
* Arguments    : 
* Remarks      : 
********************************************************************************/
Connection::~Connection ( void ) { ; }
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Connection::handle_write ( const boost::system::error_code& error,
                                  size_t size )
{
    if ( !error )
    {
        /* no response on write ack */
    }
    else
    {
        _socket.close();
    }
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Connection::handle_read ( const boost::system::error_code& error, size_t size )
{
    if ( !error )
    {
        istream is(&_input_buffet);
        getline(is, _message);
        start_read();
    }
    else
    {
        cout << "Error on receive: " << error.message() << endl;
        _socket.close();
    }
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
boost::asio::ip::tcp::socket& Connection::socket ( void )
{
    return _socket;
}
 /*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
Connection::pointer Connection::create ( boost::asio::io_service & _io_service )
{
    return pointer(new Connection(_io_service));
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Connection::start_write ( string _message )
{    
    start_read();

    boost::asio::async_write(_socket,
                             boost::asio::buffer(_message),
                             boost::bind( &Connection::handle_write,
                                          shared_from_this(),
                                          boost::asio::placeholders::error,
                                          boost::asio::placeholders::bytes_transferred));
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Connection::start_read ( void )
{
    boost::asio::async_read_until(_socket,
                                  _input_buffet, 
                                  '\n',
                                  boost::bind(&Connection::handle_read, 
                                              shared_from_this(),
                                              boost::asio::placeholders::error,
                                              boost::asio::placeholders::bytes_transferred));
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
bool Connection::is_connected ( void )
{
    return _socket.is_open();
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
string Connection::get_message ( void )
{
    return _message;
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Connection::set_server_computation ( string _in )
{
    _server_computation = _in;
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
string Connection::get_server_computation ( void )
{
    return _server_computation;
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Connection::set_checked ( bool _in )
{
    _checked = _in;
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
bool Connection::is_checked ( void )
{
    return _checked;
}