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
#include "MessagePool.hpp"
#include "Participant.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <utility>

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
Connection::Connection ( boost::asio::ip::tcp::socket _in_socket,
                          MessagePool& _in_mp )  : _socket(move(_in_socket)), 
                                                   _mp(_in_mp)
{
    cout << get_connection_info() << " connected" << endl;
}
/*******************************************************************************
* Deconstructor: 
* Description  : 
* Arguments    : 
* Remarks      : 
********************************************************************************/
Connection::~Connection ( void )
{ 
    cout << get_connection_info() << " disonnected" << endl;
    _socket.close();
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Connection::start ( void )
{
    _mp.join(shared_from_this());
    do_read_header();
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Connection::deliver ( const Message& msg )
{
    bool write_in_progress = !_write_messages.empty();
    _write_messages.push_back(msg);
    if (!write_in_progress)
    {
        do_write();
    }
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Connection::do_read_header ( void )
{
    auto self(shared_from_this());
    boost::asio::async_read(_socket,
                            boost::asio::buffer(_message.data(), 
                                                Message::header_length),
                                                [this, self](boost::system::error_code ec, size_t)
                                                {
                                                    if (!ec && _message.decode_header())
                                                    {
                                                        do_read_body();
                                                    }
                                                    else
                                                    {
                                                        _mp.leave(shared_from_this());
                                                    }
                                                });
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Connection::do_read_body ( void )
{
    auto self(shared_from_this());
    boost::asio::async_read(_socket,
                            boost::asio::buffer(_message.body(), _message.body_length()),
                            [this, self](boost::system::error_code ec, size_t)
                            {
                                if (!ec)
                                {      
                                    _message.connection_info = get_connection_info();
                                    _mp.receive(_message);
                                    do_read_header();
                                }
                                else
                                {
                                    _mp.leave(shared_from_this());
                                }
                            });
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Connection::do_write ( void )
{
    auto self(shared_from_this());
    boost::asio::async_write(_socket,
                            boost::asio::buffer(_write_messages.front().data(),
                                                _write_messages.front().length()),
                            [this, self](boost::system::error_code ec, size_t)
                            {
                                if (!ec)
                                {
                                    _write_messages.pop_front();
                                    if (!_write_messages.empty())
                                    {
                                        do_write();
                                    }
                                }
                                else
                                {
                                    _mp.leave(shared_from_this());
                                }
                            });
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
string Connection::get_connection_info ( void )
{
    stringstream ss;
    ss << _socket.remote_endpoint().port();
    return _socket.remote_endpoint().address().to_string() + ":" + ss.str();
}