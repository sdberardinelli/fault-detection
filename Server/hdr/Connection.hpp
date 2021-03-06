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
#include "Participant.hpp"
#include "MessagePool.hpp"
#include "typedefs.hpp"

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
class Connection : public Participant,
                   public std::enable_shared_from_this<Connection>
{
    public: 
        /* constructors */
        Connection (  boost::asio::ip::tcp::socket,  MessagePool& );
        ~Connection ( void );

        /* variables */
        
        /* functions */
        
        void start ( void );
        void deliver (const Message& );
        std::string get_connection_info ( void );

    private:
        /* functions */
        void do_write ( void );
        void do_read_header ( void );
        void do_read_body ( void );
        
        /* variables */
        boost::asio::ip::tcp::socket _socket;
        MessagePool&                 _mp;
        Message                      _message;
        message_queue                _write_messages;
};
#endif
