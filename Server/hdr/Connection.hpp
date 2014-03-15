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
        
    private:
        /* functions */
        void handle_write ( const boost::system::error_code&, size_t);
        void handle_read ( const boost::system::error_code&, size_t);
        
        /* variables */
        boost::asio::ip::tcp::socket _socket;
        boost::asio::streambuf _input_buffet;
};
#endif
