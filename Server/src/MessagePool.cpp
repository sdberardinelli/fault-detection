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
    for ( auto msg: _recent_msgs )
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
void MessagePool::deliver ( const Message& msg )
{
    _recent_msgs.push_back(msg);
    while ( _recent_msgs.size() > max_recent_msgs )
    {
        _recent_msgs.pop_front();
    }

    for (auto participant: _participants)
    {
        participant->deliver(msg);
        
        cout << msg.data() << endl;
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
    return _recent_msgs.size();
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
