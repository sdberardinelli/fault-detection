/*******************************************************************************
 * Filename      : Connection.hpp
 * Source File(s): Connection.cpp
 * Description   :
 * Authors(s)    : 
 * Date Created  : 
 * Date Modified : 
 * Modifier(s)   : 
 *******************************************************************************/
#ifndef CONNECTION_H
#define	CONNECTION_H

/*******************************************************************************
*                                   INCLUDES
********************************************************************************/
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
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

/*******************************************************************************
*  CLASS DEFINITIONS
********************************************************************************/
class Connection : public boost::enable_shared_from_this<Connection>
{
    public: 
        /* constructors */
        Connection ( boost::asio::io_service& );
        ~Connection ( void );

        /* variables */
        typedef boost::shared_ptr<Connection> pointer;
        
        /* functions */
        static pointer create(boost::asio::io_service& );
        boost::asio::ip::tcp::socket& socket ( void );
        void start_write ( std::string );
        void start_read ( void );
        bool is_connected ( void );
        std::string get_message ( void );
        std::string get_server_computation ( void );
        void set_server_computation ( std::string );
        
        bool is_checked ( void );
        void set_checked ( bool );
        
    private:
        /* functions */
        void handle_write ( const boost::system::error_code&, size_t);
        void handle_read ( const boost::system::error_code&, size_t);
        
        /* variables */
        boost::asio::ip::tcp::socket _socket;
        boost::asio::streambuf       _input_buffet;
        std::string                  _message;
        std::string                  _server_computation;
        bool                         _checked;
};
#endif
