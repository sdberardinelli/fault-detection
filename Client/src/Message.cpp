/*******************************************************************************
 * Filename      : Message.cpp
 * Header File(s): Message.hpp
 * Description   :
 * Authors(s)    : 
 * Date Created  : 
 * Date Modified :
 * Modifier(s)   :
 *******************************************************************************/
/************************************
 * Included Headers 
 ************************************/
#include "Message.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>

/************************************
 * Namespaces 
 ************************************/
using namespace std;

/************************************
 * Local Variables 
 ************************************/

/*******************************************************************************
* Constructor  : 
* Description  : 
* Arguments    : 
* Remarks      : 
********************************************************************************/
Message::Message ( void )
{
    body_length_ = 0;
}
/*******************************************************************************
* Deconstructor: 
* Description  : 
* Arguments    : 
* Remarks      : 
********************************************************************************/
Message::~Message ( void ) { ; }
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
const char* Message::data ( void ) const
{
    return data_;
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
char* Message::data ( void )
{
    return data_;
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
size_t Message::length ( void ) const
{
    return header_length + body_length_;
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
const char* Message::body ( void ) const
{
     return data_ + header_length;
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
char* Message::body ( void )
{
    return data_ + header_length;
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
size_t Message::body_length ( void ) const
{
    return body_length_;
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Message::body_length ( size_t new_length )
{
    body_length_ = new_length;
    if (body_length_ > max_body_length)
      body_length_ = max_body_length;
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
bool Message::decode_header ( void )
{
    char header[header_length + 1] = "";
    strncat(header, data_, header_length);
    body_length_ = atoi(header);
    if (body_length_ > max_body_length)
    {
      body_length_ = 0;
      return false;
    }
    return true;    
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Message::encode_header ( void )
{
    char header[header_length + 1] = "";
    sprintf(header, "%4d", body_length_);
    memcpy(data_, header, header_length);    
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
