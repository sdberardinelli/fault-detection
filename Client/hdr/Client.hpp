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
#include <string>
#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/streambuf.hpp>

/*******************************************************************************
*                                    DATATYPES
********************************************************************************/
typedef enum E_CLIENT_DISTRIBUTIONS
{
    NONE    = 0,
    BETA    = 1,
    UNIFORM = 2,
    NORMAL  = 3,
    GAMMA   = 4 
}CLIENT_DISTRIBUTIONS;
/*******************************************************************************
*                                    EXTERNS
********************************************************************************/
inline const char* DistToString(CLIENT_DISTRIBUTIONS e)
{
    switch (e)
    {
        case BETA:     return "beta";
        case UNIFORM:  return "uniform";
        case NORMAL:   return "normal";
        case GAMMA:    return "gamma";
        default:       return "none";
    }
}  
inline CLIENT_DISTRIBUTIONS DistToEnum(const char* in)
{
    if ( strcmp(in, "beta")      == 0 ) return BETA;
    if ( strcmp(in, "uniform")   == 0 ) return UNIFORM;
    if ( strcmp(in, "normal")    == 0 ) return NORMAL;
    if ( strcmp(in, "gamma")     == 0 ) return GAMMA;

    return NONE;
}  

/*******************************************************************************
*                                    DEFINES
********************************************************************************/
#define HEARTBEAT        0
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
        
        void process_message ( TestFunctions::TEST_FUNCTIONS, std::vector<std::string>& );
        
        /* variables */    
        bool                          _fault;
        Distributions::Distribution   _dist;
        CLIENT_DISTRIBUTIONS          _dist_type;
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
