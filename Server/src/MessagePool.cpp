/*******************************************************************************
 * Filename      : MessagePool.cpp
 * Header File(s): MessagePool.hpp
 * Description   :
 * Authors(s)    : 
 * Date Created  : 
 * Date Modified :
 * Modifier(s)   :
 *******************************************************************************/
/************************************
 * Included Headers 
 ************************************/
#include "MessagePool.hpp"
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <utility>
#include <cstring>
#include <string>

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
MessagePool::MessagePool ( void ) { ; }
/*******************************************************************************
* Deconstructor: 
* Description  : 
* Arguments    : 
* Remarks      : 
********************************************************************************/
MessagePool::~MessagePool ( void ) { ; }
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void MessagePool::join ( participant_ptr participant )
{
    _participants.insert(participant);
    for ( auto msg: _rcvd_msgs )
    {
        /* send all the recent messages to the receinly joined client */
        participant->deliver(msg);
    }
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void MessagePool::leave ( participant_ptr  participant  )
{
    _participants.erase(participant);
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void MessagePool::deliver ( string _msg )
{
    Message msg;
    msg.body_length(strlen(_msg.c_str()));
    memcpy(msg.body(), _msg.c_str(), msg.body_length());
    msg.encode_header();  
    deliver(msg);
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void MessagePool::deliver ( const Message& msg )
{
    for (auto participant: _participants)
    {
        participant->deliver(msg);
    }
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void MessagePool::receive ( const Message& msg )
{
    _rcvd_msgs.push_back(msg);
    while ( _rcvd_msgs.size() > max_recent_msgs )
    {
        _rcvd_msgs.pop_front();
    }
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
int MessagePool::message_count ( void )
{
    return _rcvd_msgs.size();
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
int MessagePool::participant_count ( void )
{
    return _participants.size();
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
