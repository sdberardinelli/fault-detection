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
#include "Distributions.hpp"
#include "Logger.hpp"

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
        
        std::string construct_job ( char );
        void        send_jobs      ( std::string );
        bool        check_fault    ( void );
        std::string self_test      ( TestFunctions::TEST_FUNCTIONS, 
                                     std::vector<std::string>& );
        int         total_received  ( void );
        
        /* variables */
        bool                              _running;
        boost::thread *                   _thread;
        std::vector<Connection::pointer>  _connections;
        boost::asio::ip::tcp::acceptor    _acceptor;
        TestFunctions::TestFunction       _tf;
        Distributions::Distribution       _dist;
        Distributions::DISTRIBUTIONS      _dist_type;
        Logger                            _log;
        double                            _quorum_ratio;  
        double                            _client_ratio;
        double                            _receive_ratio;
        int                               _timeout;
};
#endif
