/*******************************************************************************
 * Filename      : Client.hpp
 * Source File(s): Client.cpp
 * Description   :
 * Authors(s)    : 
 * Date Created  : 
 * Date Modified : 
 * Modifier(s)   : 
 *******************************************************************************/
#ifndef CLIENT_H
#define	CLIENT_H

/*******************************************************************************
*                                   INCLUDES
********************************************************************************/
#include "TestFunctions.hpp"
#include <string>
#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/streambuf.hpp>

/*******************************************************************************
*                                    DATATYPES
********************************************************************************/

/*******************************************************************************
*                                    EXTERNS
********************************************************************************/

/*******************************************************************************
*                                    DEFINES
********************************************************************************/
#define HEARTBEAT 0

/*******************************************************************************
*  CLASS DEFINITIONS
********************************************************************************/
class Client
{
    public: 
        /* constructors */
        Client ( boost::asio::io_service& );
        ~Client ( void );

        /* functions */
        void start ( boost::asio::ip::tcp::resolver::iterator );
        void stop ( void );
              
        /* variables */
        

    private:
        /* functions */
        void start_connect ( boost::asio::ip::tcp::resolver::iterator );
        void handle_connect ( const boost::system::error_code&,
                                boost::asio::ip::tcp::resolver::iterator );
        void start_read ( void );
        void handle_read (const boost::system::error_code& );
        void start_write ( void );
        void handle_write (const boost::system::error_code& );
        void check_deadline ( void );
        
        /* variables */        
        bool                          _stopped;
        boost::asio::ip::tcp::socket  _socket;
        boost::asio::streambuf        _input_buffer;
        boost::asio::deadline_timer   _deadline;
#if HEARTBEAT
        boost::asio::deadline_timer   _heartbeat_timer;
#else
        std::string                    _reply_message;
#endif
        TestFunctions::TestFunction   _tf;
};
#endif
