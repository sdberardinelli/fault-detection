/*******************************************************************************
 * Filename      : Server.cpp
 * Header File(s): Server.hpp
 * Description   :
 * Authors(s)    : 
 * Date Created  : 
 * Date Modified :
 * Modifier(s)   :
 *******************************************************************************/
/************************************
 * Included Headers 
 ************************************/
#include "Server.hpp"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/algorithm/string/split.hpp>                                      
#include <boost/algorithm/string.hpp> 
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <valarray>
#include <utility>
#include <string>
#include <thread>
#include <chrono>

/************************************
 * Namespaces 
 ************************************/
using namespace std;
using namespace TestFunctions;
using namespace Distributions;

/************************************
 * Local Variables 
 ************************************/

/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Server::start ( void )
{
     _running = true;
    _thread = new thread(&Server::run,this);
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Server::stop ( void )
{
    _running = false;
    _thread->join();
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
bool Server::is_running ( void )
{
    return _running;
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Server::run ( void )
{
    char in;
    string _message;
    int timeout;
    int accept_count = 0;
    int self_accept_count = 0;
    do
    {
Begin:
        if ( _mp.participant_count() < 1 )
            continue;
        
        in = (rand()%3) + TestFunctions::DO_ADD; /* randomly pick a command   */
        _message = construct_job(in);            /* construct message for job */     
        send_jobs(_message);                     /* send job to n clients     */
        
        timeout = 0;
        
        do                                       /* wait until msgs are rcvd  */
        {
            this_thread::sleep_for (chrono::seconds(1));
           
            if ( _mp.participant_count() == 0 )
                continue;

            _receive_ratio = _mp.message_count()*1.0/_mp.participant_count()*1.0;            
            if (timeout++ >= _timeout)
            {
                timeout = 0;
                cout << "timeout"<< endl;
                _mp.reset();
                goto Begin;
            }
            
        } while ( _receive_ratio < _client_ratio );
        
        if ( check_fault() )                   /* check the majority         */
        {
            /* quorum agree */
            accept_count++;
        }
        else
        {
            /* quorum disagree: fault */ 
            cout << accept_count << "," << self_accept_count << ",1" << endl;
            accept_count = 0;
        }
        
        if ( rand()/double(RAND_MAX) < _dist.uniform() )
        {
            vector<string> strs;
            boost::algorithm::split(strs,_message,boost::is_any_of(","));
            string _self_test_message = self_test(ToEnum(strs[0].c_str()),strs);
            if ( _self_test_message.compare(_mp.get_msg_at(rand()%(_mp.message_count()-1))) == 0 )
            {
                self_accept_count++;
            }
            else
            {
                cout << accept_count << "," << self_accept_count << ",2" << endl;
                self_accept_count = 0;
            }
        }   
        
        cout << accept_count << "," << self_accept_count << ",2" << endl;
        
        _mp.reset();        
    }while(_running);
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
string Server::construct_job ( char in )
{
    string _message, param1, param2;
    stringstream ss;
    
    switch ( in )
    {
        case TestFunctions::DO_ADD:
        {
            ss << (rand() % 100000);
            param1 = ss.str();
            ss.str(string());
            ss << (rand() % 100000);
            param2 = ss.str();
            ss.str(string());
            _message = string(ToString(TestFunctions::DO_ADD))+","+
                       param1+","+
                       param2+"\n";
        }
            break;
        case TestFunctions::DO_STR:
        {
            static const char alphanum[] = "0123456789!@#$%^&*ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
            int length = rand() % 100;
            for ( int i = 0; i < length; i++ )
            {
                param1 += alphanum[rand() % (sizeof(alphanum) - 1)];
            }
            length = rand() % 100;
            for ( int i = 0; i < length; i++ )
            {
                param2 += alphanum[rand() % (sizeof(alphanum) - 1)];
            }
            _message = string(ToString(TestFunctions::DO_STR))+","+
                                       param1+","+
                                       param2+"\n";
        }
            break;
        case TestFunctions::DO_MUL:
        {
            ss << (rand() % 100000);
            param1 = ss.str();
            ss.str(string());
            ss << (rand() % 100000);
            param2 = ss.str();
            ss.str(string());
            _message = string(ToString(TestFunctions::DO_MUL))+","+
                       param1+","+
                       param2+"\n";
        }
            break;
        default:
            _message = in+"\n";
            break;
    }
    
    return _message;
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Server::send_jobs ( string _message )
{
    for ( vector<int>::size_type i = 0; i < _mp.participant_count(); i++ )
    {
        if ( _dist.user_pick(_dist_type) )
        {
            _mp.deliver(_message, i);
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
bool Server::check_fault ( void )
{
    int count = 0;
    bool agree = false;
  
    if ( double(_mp.message_count())/double(_mp.participant_count())  > _client_ratio )
    {
        for ( vector<int>::size_type i = 0; i < _mp.message_count()-1; i++ )
        {
            if ( _mp.get_msg_at(i).compare(_mp.get_msg_at(i+1)) == 0 )
            {
                if ( (double(++count)/double(_mp.message_count())) > _quorum_ratio)
                {
                    agree = true;
                    break;
                }
            }
        }
    }
    return agree;
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
std::string Server::self_test ( TestFunctions::TEST_FUNCTIONS CMD, 
                                 std::vector<std::string>& strs )
{
    stringstream ss;
    string param1 = strs[1], param2 = strs[2], _tmp;
        
    switch ( CMD )
    {
        case TestFunctions::DO_ADD:
        {        
            int value = _tf.do_add(atoi(param1.c_str()), atoi(param2.c_str()));
            ss << value;
            _tmp = ss.str();
        }
            break;
        case TestFunctions::DO_STR:
        {
            string value = _tf.do_string_cat(param1, param2);
            _tmp = value;

        }
            break;
        case TestFunctions::DO_MUL:
        {
            int value = _tf.do_multipy(atoi(param1.c_str()), atoi(param2.c_str()));
            ss << value;
            _tmp = ss.str();
        }
            break;
        default:;
            break;
    }  
    
    return _tmp;
}
    