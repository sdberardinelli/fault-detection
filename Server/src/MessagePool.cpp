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
void MessagePool::deliver ( string _msg, int i)
{
    Message msg;
    msg.body_length(strlen(_msg.c_str()));
    memcpy(msg.body(), _msg.c_str(), msg.body_length());
    msg.encode_header();  
    deliver(msg, i);
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
void MessagePool::deliver ( const Message& msg, int i )
{
    int itr = 0;
    for (auto participant: _participants)
    {
        if ( itr++ == i)
        {
            participant->deliver(msg);
        }
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
void MessagePool::reset ( void )
{
    _rcvd_msgs.clear();
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
string MessagePool::get_next_msg ( void )
{
    string str = "";
    Message msg;
    if ( !_rcvd_msgs.empty() )
    {
        msg = _rcvd_msgs.back();
        _rcvd_msgs.pop_back();
        
        char * data = msg.body();
        for ( int i = 0; i < msg.body_length(); i++ )
        {
            str += data[i];
        }
    }
    
    return str;
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
string MessagePool::get_msg_at ( int idx )
{
    string str = "";
    try
    {
        Message msg = _rcvd_msgs.at(idx);        
        char * data = msg.body();
        for ( int i = 0; i < msg.body_length(); i++ )
        {
            str += data[i];
            if (data[i] == '\n')
                break;
        }
    }
    catch ( const out_of_range& oor )
    {
        str = "1";
#if DEBUG
         cerr << "Out of Range error: " << oor.what() << endl;
#endif
    }
    
    return str;
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
string MessagePool::get_participant_info ( int idx )
{
    string str = "";
    try
    {
        Message msg = _rcvd_msgs.at(idx);
        
        str = msg.connection_info;
    }
    catch ( const out_of_range& oor )
    {
        str = "1";
#if DEBUG
         cerr << "Out of Range error: " << oor.what() << endl;
#endif         
    }
    
    return str;
}
