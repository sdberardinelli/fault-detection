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
#include "Distributions.hpp"
#include "Message.hpp"
#include "typedefs.hpp"
#include <string>
#include <thread>
#include <boost/asio.hpp>

/*******************************************************************************
*                                    DATATYPES
********************************************************************************/

/*******************************************************************************
*                                    EXTERNS
********************************************************************************/

/*******************************************************************************
*                                    DEFINES
********************************************************************************/
#define CONFIG_FILENAME  "config"

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
        void write ( const Message& );
        void close ( void );
        bool is_connected ( void );
        void run ( void );
        
        /* variables */
        

    private:
        /* functions */        

        void do_connect ( boost::asio::ip::tcp::resolver::iterator );
        void do_read_header ( void );
        void do_read_body ( void );
        void do_write ( void );
        
        // ALGORITHM
        Message process_message ( TestFunctions::TEST_FUNCTIONS, std::vector<std::string>& );
        
        /* variables */    
        boost::asio::io_service&     _io_service;
        boost::asio::ip::tcp::socket _socket;
        std::thread                  _thread;
        Message                      _read_msg;
        message_queue                _write_msgs;

        // ALGORITHM
        bool                          _fault;
        Distributions::Distribution   _dist;
        Distributions::DISTRIBUTIONS  _dist_type;  
        TestFunctions::TestFunction   _tf;
        std::string                   _reply_message;
};
#endif
