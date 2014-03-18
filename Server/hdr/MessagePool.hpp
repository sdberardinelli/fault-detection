/*******************************************************************************
 * Filename      : MessagePool.hpp
 * Source File(s): MessagePool.cpp
 * Description   :
 * Authors(s)    : 
 * Date Created  : 
 * Date Modified : 
 * Modifier(s)   : 
 *******************************************************************************/
#ifndef MESSAGEPOOL_H
#define	MESSAGEPOOL_H

/*******************************************************************************
*                                   INCLUDES
********************************************************************************/
#include "Message.hpp"
#include "typedefs.hpp"
#include <set>
#include <string>

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
class MessagePool
{
    public: 
        /* constructors */
        MessagePool ( void );
        ~MessagePool ( void );
        
        /* variables */
        
        /* functions */
        void join ( participant_ptr );
        void leave ( participant_ptr );
        void deliver ( std::string );
        void deliver ( const Message& );
        void receive ( const Message& );
        int  message_count ( void );
        int  participant_count ( void );
    private:
        /* functions */
        
        /* variables */
        enum { max_recent_msgs = 100 };
        std::set<participant_ptr> _participants;
        message_queue             _rcvd_msgs;        
};
#endif
