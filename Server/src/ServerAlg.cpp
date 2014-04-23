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
    string message;
    int timeout;
    bool quorum_agree;
    bool server_check;
    int init_client_count = 0;
    ostringstream ostream_string;
    
    do
    {
        
        if ( init_client_count < _mp.participant_count() )
        {
             cout << "connected clients: " << _mp.participant_count() << endl;
            init_client_count = _mp.participant_count();
        }
    }while ( _mp.participant_count() < _clients );
    
    
    for ( int trial = 0; trial < _trials; trial++ )
    {
        cout << "starting trial " << trial << endl;
        _total_msg_count        = 0;
        _total_job_count        = 0;
        _quorum_incorrect_count = 0;
        _server_quorum_incorrect= 0;
        _server_quorum_correct  = 0;
        _quorum_agree_count     = 0;
        _quorum_disagree_count  = 0;
        _quorum_correct_count   = 0;        
        server_check = true;
        
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

            for ( vector<int>::size_type i = 0; i < _indecies.size(); i++ )
            {
                _indecies[i]->clear();
            }        
            _indecies.clear();
            _mp.reset(); 

            in = (rand()%3) + TestFunctions::DO_ADD; /* randomly pick a command   */
            message = construct_job(in);             /* construct message for job */   
            send_jobs(message);                      /* send job to n clients     */
            
            _total_job_count++;

            timeout = 0;

            do                                       /* wait until msgs are rcvd  */
            {
                this_thread::sleep_for (chrono::milliseconds(500));

                if ( _mp.participant_count() == 0 )
                    continue;

                _receive_ratio = _mp.message_count()*1.0/_mp.participant_count()*1.0;     
                if (timeout++ >= _timeout)
                {
                    timeout = 0;
                    cout << "timeout"<< endl;
                    goto Begin;
                }

            } while ( _receive_ratio < _client_ratio );

            if ( check_fault() )                   /* check the majority         */
            {
                quorum_agree = true;
            }
            else
            {
                quorum_agree = false;
            }

            if ( quorum_agree )
            {       
                cout << "quorum agree" << endl;
                _quorum_agree_count++;
                
                if (  _dist.user_pick(_dist_type) ) /* server check */
                {
                    cout << "server checking: ";
                    
                    vector<string> strs;
                    boost::algorithm::split(strs,message,boost::is_any_of(","));
                    string _self_test_message = self_test(ToEnum(strs[0].c_str()),strs);
                    if ( _self_test_message.compare(_mp.get_msg_at(_quorum_index)) != 0 )
                    {
                       server_check = false;
                       cout << " quorum was incorrect" << endl;
                       _server_quorum_incorrect++;
                    }
                    else
                    {
                        cout << " quorum was correct" << endl;
                        _server_quorum_correct++;
                    }
                }

                vector<string> strs1;
                boost::algorithm::split(strs1,message,boost::is_any_of(","));
                string _self_test_message1 = self_test(ToEnum(strs1[0].c_str()),strs1);

                if ( _self_test_message1.compare(_mp.get_msg_at(_quorum_index)) == 0 )
                {
                    _quorum_correct_count++;
                    cout << "quorum was correct" << endl;
                }
                else
                {
                    _quorum_incorrect_count++;
                    cout << "quorum was incorrect" << endl;
                    ostream_string << trial << ","
                                   << _mp.participant_count() << ","
                                   << _total_job_count << ","
                                   << _server_quorum_correct << ","
                                   << _quorum_incorrect_count << ","
                                   << _server_quorum_incorrect << ","
                                   << _quorum_disagree_count << ","                    
                                   << _total_msg_count << ","
                                   << _quorum_agree_count << ","
                                   << _quorum_correct_count; 
                    _log->log("output", _experiement, ostream_string.str());   
                    ostream_string.str("");                    
                }
            }
            else
            {
                _quorum_disagree_count++;
                cout << "quorum did not agree" << endl;
            }

            if (!_running)
            {
                return;
            }
            
            cout << trial << ","
                 << _mp.participant_count() << ","
                 << _total_job_count << ","
                 << _server_quorum_correct << ","
                 << _quorum_incorrect_count << ","
                 << _server_quorum_incorrect << ","
                 << _quorum_disagree_count << ","                    
                 << _total_msg_count << ","
                 << _quorum_agree_count << ","
                 << _quorum_correct_count << endl;
        
        }while(server_check);
        
        cout << "end of trial " << trial << endl;
        
        
	ostream_string << trial << ","
                       << _mp.participant_count() << ","
                       << _total_job_count << ","
                       << _server_quorum_correct << ","
                       << _quorum_incorrect_count << ","
                       << _server_quorum_incorrect << ","
                       << _quorum_disagree_count << ","                    
                       << _total_msg_count << ","
                       << _quorum_agree_count << ","
                       << _quorum_correct_count;
        
        cout << "result: " << ostream_string.str() << endl;
        cout << endl << endl;
        _log->log("output", _experiement, ostream_string.str());
        
        ostream_string.str("");
    }
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
    bool got_jobs = false;
      
    do
    {
        for ( vector<int>::size_type i = 0; i < _mp.participant_count(); i++ )
        {
            if ( _sending_dist.user_pick(_sending_dist_type) )
            {
                indecies.push_back(i);
            }

            if ( double(indecies.size())/double(_mp.participant_count()) >= _client_ratio )
            {
                got_jobs = true;
                break;
            }
        }
    
        if ( double(indecies.size())/double(_mp.participant_count()) < _client_ratio )
        {
            indecies.clear();
        }
    }while (!got_jobs);
    
    for ( vector<int>::size_type i = 0; i < indecies.size(); i++ )
    {
        _total_msg_count++;
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
    bool added = false;
    
    if ( double(_mp.message_count())/double(_mp.participant_count())  >= _client_ratio )
    {
        string test_string = _mp.get_msg_at(0);
        _quorum.push_back(new vector<string>());
        _quorum[0]->push_back(test_string);
        
        _indecies.push_back(new vector<int>());
        _indecies[0]->push_back(0);
        
        for ( vector<int>::size_type i = 1; i < _mp.message_count(); i++ )
        {
            test_string = _mp.get_msg_at(i);
            added = false;
            for ( vector<int>::size_type j = 0; j < _quorum.size(); j++ )
            {
                
                if ( test_string.compare(_quorum[j]->at(0)) == 0 )
                {
                    _quorum[j]->push_back(test_string);
                    _indecies[j]->push_back(i);
                    added = true;
                    break;
                }
            }
            
            if (!added)
            {
                _quorum.push_back(new vector<string>());
                _quorum.back()->push_back(test_string);
                _indecies.push_back(new vector<int>());
                _indecies.back()->push_back(i);                
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
    