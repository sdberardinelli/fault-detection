/*******************************************************************************
 * Filename      : Message.hpp
 * Source File(s): Message.cpp
 * Description   :
 * Authors(s)    : 
 * Date Created  : 
 * Date Modified : 
 * Modifier(s)   : 
 *******************************************************************************/
#ifndef MESSAGE_H
#define	MESSAGE_H

/*******************************************************************************
*                                   INCLUDES
********************************************************************************/
#include <cstddef>

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
class Message
{
    public: 
        /* constructors */
        Message ( void );
        ~Message ( void );

        /* variables */
        enum { header_length = 4 };
        enum { max_body_length = 512 };
        
        /* functions */
        const char* data ( void ) const;
        char* data ( void );
        std::size_t length ( void ) const;
        const char* body ( void ) const;
        char* body ( void );
        std::size_t body_length ( void ) const;
        void body_length ( std::size_t );
        bool decode_header ( void );
        void encode_header ( void );
    private:
        /* functions */
        
        /* variables */
        char data_[header_length + max_body_length];
        std::size_t body_length_;       
};
#endif
