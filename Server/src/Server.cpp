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
#include <string>

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
* Constructor  : 
* Description  : 
* Arguments    : 
* Remarks      : 
********************************************************************************/
Server::Server ( boost::asio::io_service & io, short port ) : _acceptor(io, 
                                                                        boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 
                                                                                                       port))
{
    srand (time(NULL));

    vector<string> strs;
    valarray<double> parameters;
    std::string str; 
    boost::filesystem::ifstream file_in;
    file_in.open(CONFIG_FILENAME,ios::in);

    if ( !file_in )
    {
        ;
    }
    else
    {
        try
        {
            getline(file_in, str);
            boost::algorithm::split(strs,str,boost::is_any_of("="));
            _dist_type = DistToEnum(strs[1].c_str());
            getline(file_in, str);
            boost::algorithm::split(strs,str,boost::is_any_of("="));
            boost::algorithm::split(strs,strs[1],boost::is_any_of(","));
            
            parameters.resize(strs.size());
            for ( vector<int>::size_type i = 0; i != strs.size(); i++ )
            {
                parameters[i] = atof(strs[i].c_str());
            }
            
            _dist.set_parameters(parameters);
            _dist.construct_distribution(_dist_type);
           
            getline(file_in, str);
            boost::algorithm::split(strs,str,boost::is_any_of("="));
            _quorum_ratio = atof(strs[1].c_str());
            
            getline(file_in, str);
            boost::algorithm::split(strs,str,boost::is_any_of("="));
            _client_ratio = atof(strs[1].c_str());            
            
            getline(file_in, str);
            boost::algorithm::split(strs,str,boost::is_any_of("="));
            _timeout = atoi(strs[1].c_str());            
        }
        catch(const boost::filesystem::ifstream::failure & ex)
        {
            ;
        }
    }
    
    _running = false;
    start_accept();
}
/*******************************************************************************
* Deconstructor: 
* Description  : 
* Arguments    : 
* Remarks      : 
********************************************************************************/
Server::~Server ( void ) { ; }
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Server::start_accept ( void )
{   
    Connection::pointer new_connection = Connection::create( _acceptor.get_io_service() );

    _acceptor.async_accept( new_connection->socket(),
                           boost::bind( &Server::handle_accept, 
                                        this, 
                                        new_connection,
                                        boost::asio::placeholders::error));
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Server::handle_accept ( Connection::pointer _new_connection, 
                               const boost::system::error_code& error )
{
    if (!error)
    {
        _connections.push_back(_new_connection);
    }

    start_accept();
}
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
    _thread = new boost::thread(boost::bind(&Server::run, this));
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
    _thread->interrupt();
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
    int timeout;
    char in;
    string _message,_self_test_message;
    
    do 
    {  
        if ( _connections.size() == 0 )
            continue;
        
        in = (rand()%3) + TestFunctions::DO_ADD; /* randomly pick a command   */
        _message = construct_job(in);            /* construct message for job */
        send_jobs(_message);                     /* send job to n clients     */
        try
        {
            timeout = 0;                         /* wait until quorum ratio   */
            do                                   /* or a timeout of 1 second  */
            {
                if ( _connections.size() == 0 )
                    break;
                
                _receive_ratio = total_received()/_connections.size();
                boost::this_thread::sleep(boost::posix_time::milliseconds(100));
            }while ( _receive_ratio <= _client_ratio && timeout++ < _timeout*10);
            
            if ( timeout == 11 )
            {
                cout << "query timeout" << endl;
                continue;
            }
        }
        catch(boost::thread_interrupted&)
        {
            cout << "Thread is stopped" << endl;
            break;
        }  
        
        if ( check_fault() )
        {
            /* quorum agree */
            cout << "answer is accepted" << endl;
        }
        else
        {
            /* quorum disagree: fault */
             cout << "answer is NOT accepted" << endl;
        }
        
        if ( rand()/double(RAND_MAX) < _dist.uniform() )
        {
            vector<string> strs;
            boost::algorithm::split(strs,_message,boost::is_any_of(","));
            _self_test_message = self_test(ToEnum(strs[0].c_str()),strs);
            
            
            if ( _self_test_message.compare(_connections[0]->get_message()) == 0 )
            {
                cout << "answer is accepted" << endl;
            }
            else
            {
                 cout << "answer is NOT accepted" << endl;
            }
        }        
        
    }while ( _running );
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
    for ( vector<int>::size_type i = 0; i < _connections.size(); i++ )
    {
        if ( _connections[i]->is_connected() )
        {   
            _connections[i]->set_received(false);
                    
            if ( _dist.user_pick(_dist_type) ) 
            {
                _connections[i]->start_write(_message);
                boost::this_thread::sleep(boost::posix_time::milliseconds(100));
                
                _message.erase(remove(_message.begin(), 
                                      _message.end(), 
                                      '\n'), 
                                      _message.end());            
                cout << "Sending \"" << _message
                                      << "\" to " 
                                      << _connections[i]->get_connection_info() 
                                      << endl << endl;
            }
            else
            {
                ;
            }
        }
        else
        {
            if ( _connections.size() > 1 )
                swap(_connections[i], _connections.back());
            _connections.pop_back();
            
            if ( _connections.size() == 0 )
                break;
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
    for ( vector<int>::size_type i = 0; i < _connections.size()-1; i++ )
    {
        if ( _connections[i]->get_message().compare(_connections[i+1]->get_message()) )
        {
            if ((++count/_connections.size() > _quorum_ratio))
            {
                agree = true;
                break;
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
int Server::total_received ( void )
{
    int count = 0;
    for ( vector<int>::size_type i = 0; i < _connections.size(); i++ )
    {
        if ( _connections[i]->is_received() )
        {
            count++;
        }
    }
    return count;
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
            _tmp = (ss.str()+"\n");
        }
            break;
        case TestFunctions::DO_STR:
        {
            string value = _tf.do_string_cat(param1, param2);
            _tmp = (value+"\n");

        }
            break;
        case TestFunctions::DO_MUL:
        {
            int value = _tf.do_multipy(atoi(param1.c_str()), atoi(param2.c_str()));
            ss << value;
            _tmp = (ss.str()+"\n");
        }
            break;
        default:;
            break;
    }  
    
    return _tmp;
}
    