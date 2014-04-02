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
#include <thread>
#include <vector>
#include "Connection.hpp"
#include "TestFunctions.hpp"
#include "Distributions.hpp"
#include "Logger.hpp"
#include "Participant.hpp"
#include "MessagePool.hpp"

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
        Server ( boost::asio::io_service&, const boost::asio::ip::tcp::endpoint& );
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
        
        // ALGORITHM
        std::string construct_job ( char );
        void        send_jobs      ( std::string );
        bool        check_fault    ( void );
        std::string self_test      ( TestFunctions::TEST_FUNCTIONS, 
                                     std::vector<std::string>& );
        
        /* variables */
        bool                              _running;
        std::thread *                     _thread;
        boost::asio::ip::tcp::acceptor    _acceptor;
        boost::asio::ip::tcp::socket      _socket;
        MessagePool                       _mp;
        
        std::vector< std::vector<int> * >         _indecies;
        std::vector< std::vector<std::string> * > _quorum;
        int                                       _quorum_index;
        // ALGORITHM
        TestFunctions::TestFunction       _tf;
        Distributions::Distribution       _dist;
        Distributions::DISTRIBUTIONS      _dist_type;
        Distributions::Distribution       _sending_dist;
        Distributions::DISTRIBUTIONS      _sending_dist_type;        
        Logger*                           _log;
        double                            _quorum_ratio;  
        double                            _client_ratio;
        double                            _receive_ratio;
        int                               _timeout;
        int                               _trials;
        std::string                       _experiement;
        
        int                               _total_job_count;
        int                               _quorum_incorrect_count;
        int                               _server_check_count;  
        
        int                               _quorum_agree_count;
        int                               _quorum_disagree_count;
        int                               _quorum_correct_count;      
};
#endif
