/*******************************************************************************
 * Filename      : Server.hpp
 * Source File(s): Server.cpp
 * Description   :
 * Authors(s)    : 
 * Date Created  : 
 * Date Modified : 
 * Modifier(s)   : 
 *******************************************************************************/
#ifndef SERVER_H
#define	SERVER_H

/*******************************************************************************
*                                   INCLUDES
********************************************************************************/
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <vector>
#include "Connection.hpp"
#include "TestFunctions.hpp"

/*******************************************************************************
*                                    DATATYPES
********************************************************************************/

/*******************************************************************************
*                                    EXTERNS
********************************************************************************/

/*******************************************************************************
*                                    DEFINES
********************************************************************************/

/*******************************************************************************
*  CLASS DEFINITIONS
********************************************************************************/
class Server
{
    public: 
        /* constructors */
        Server ( boost::asio::io_service&, short );
        ~Server ( void );

        /* functions */
        void run ( void );
        void start ( void );
        void stop ( void );
        bool is_running ( void );
                
        /* variables */

    private:
        /* functions */
        void start_accept ( void );
        void handle_accept (Connection::pointer, const boost::system::error_code& );
        
        /* variables */
        bool                              _running;
        boost::thread *                   _thread;
        std::vector<Connection::pointer>  _connections;
        boost::asio::ip::tcp::acceptor    _acceptor;
        TestFunctions::TestFunction       _tf;


        
};
#endif
