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
    
    vector<string> message_pool;
    do
    {
Begin:
        if ( _mp.participant_count() < 1 )
            continue;

        for ( vector<int>::size_type i = 0; i < _quorum.size(); i++ )
        {
            _quorum[i]->clear();
        }
        _quorum_index = 0;
        _quorum.clear();
        _mp.reset(); 
        
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
            cout << "quorum did not agree" << endl;
            accept_count = 0;
        }
        
        if (  _dist.user_pick(_dist_type) )
        {
            cout << "checking";
            vector<string> strs;
            boost::algorithm::split(strs,_message,boost::is_any_of(","));
            string _self_test_message = self_test(ToEnum(strs[0].c_str()),strs);
            if ( _self_test_message.compare(_mp.get_msg_at(_quorum_index)) == 0 )
            {
                self_accept_count++;
                cout << endl;
            }
            else
            {
                cout << ", server check not agree" << endl;
                self_accept_count = 0;
            }
        }   
        
        cout << accept_count << "," << self_accept_count << endl;
        
        
        vector<string> strs1;
        boost::algorithm::split(strs1,_message,boost::is_any_of(","));
        string _self_test_message1 = self_test(ToEnum(strs1[0].c_str()),strs1);
        
        if ( _self_test_message1.compare(_mp.get_msg_at(_quorum_index)) == 0 )
        {
            cout << "quorum was correct" << endl;
        }
        else
        {
            
            cout << "actual: " << _self_test_message1 << endl;
            cout << "quorum: " << _mp.get_msg_at(_quorum_index) << endl;
            cout << "quorum was incorrect" << endl;
            
//            for ( vector<int>::size_type i = 0; i < _quorum.size(); i++ )
//            {
//                cout << i << ": ";
//                for ( vector<int>::size_type j = 0; i < _quorum[i]->size(); j++ )
//                {
//                    cout << _quorum[i]->at(j) << ",";
//                }
//                cout << endl;
//            }
//            
//            getchar();
        }
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
    vector<int> indecies;
    
Retry:   
    for ( vector<int>::size_type i = 0; i < _mp.participant_count(); i++ )
    {
        if ( _sending_dist.user_pick(_sending_dist_type) )
        {
            indecies.push_back(i);
        }
        
        if ( double(indecies.size())/double(_mp.participant_count()) >= _client_ratio )
        {
            break;
        }
    }
    
    if ( double(indecies.size())/double(_mp.participant_count()) < _client_ratio )
    {
        indecies.clear();
        goto Retry;
    }
    for ( vector<int>::size_type i = 0; i < indecies.size(); i++ )
    {
        _mp.deliver(_message, indecies[i]);
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
    _quorum_index = 0;
    bool agree = false;
    
    if ( double(_mp.message_count())/double(_mp.participant_count())  >= _client_ratio )
    {
        string test_string = _mp.get_msg_at(0);
        _quorum.push_back(new vector<string>());
        _quorum[0]->push_back(test_string);
        for ( vector<int>::size_type i = 1; i < _mp.message_count(); i++ )
        {
            test_string = _mp.get_msg_at(i);
            for ( vector<int>::size_type j = 0; j < _quorum.size(); j++ )
            {
                if ( test_string.compare(_quorum[j]->at(0)) == 0 )
                {
                    _quorum[j]->push_back(test_string);
                }
                else
                {
                    _quorum.push_back(new vector<string>());
                    _quorum[j+1]->push_back(test_string);
                    break;
                }
                
            }
        }
        
        int largest = -1;
        for ( vector<int>::size_type i = 0; i < _quorum.size(); i++ )
        {
            if ( largest > _quorum[i]->size() && 
                double(_quorum[i]->size())/_mp.message_count() > _quorum_ratio )
            {
                _quorum_index = i;
                largest = _quorum[i]->size();
                agree = true;
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
    