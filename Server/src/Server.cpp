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
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <valarray>

/************************************
 * Namespaces 
 ************************************/
using namespace std;
using namespace TestFunctions;

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
    valarray<double> paramaters(3);
    
    paramaters[0] = 0;
    paramaters[1] = 1;
    paramaters[2] = 0;
    
    _dist.set_parameters(paramaters);
    _dist.construct_distributions();
    
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
    char in;
    string _message, param1, param2, _server_computation;
    stringstream ss;
    do 
    {  
        if ( _connections.size() == 0 )
            continue;
        
        in = (rand()%3) + TestFunctions::DO_ADD;
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
        

        /* send command */
        for ( vector<int>::size_type i = 0; i != _connections.size(); i++ )
        {
            if ( _connections[i]->is_connected() )
            {
                _connections[i]->start_write(_message);
                
                if ( rand()/double(RAND_MAX) < _dist.uniform() )
                {
                    _connections[i]->set_checked(true);
                    cout << "Checking";
                    switch ( in )
                    {
                        case TestFunctions::DO_ADD:
                        {
                            int value = _tf.do_add(atoi(param1.c_str()),
                                                   atoi(param2.c_str()));
                            ss << value;
                            _server_computation = ss.str();
                        }
                            break;
                        case TestFunctions::DO_STR:
                        {
                            _server_computation = _tf.do_string_cat(param1, param2);
                        }
                            break;
                        case TestFunctions::DO_MUL:
                        {
                            int value = _tf.do_multipy(atoi(param1.c_str()),
                                                       atoi(param2.c_str()));
                            ss << value;
                            _server_computation = ss.str();
                        }
                            break;
                        default:
                            _message = in+"\n";
                            break;
                    }
                    _connections[i]->set_server_computation(_server_computation);
                }
                else
                {
                    cout << "Not Checking" << endl;
                    _connections[i]->set_checked(false);
                }
                
            }
            else
            {
                //swap(_connections[i], _connections.back());
                //_connections.pop_back();
                cout << "removing connection" << endl;
            }
        }
        
        try
        {
            // Sleep and check for interrupt.
            // To check for interrupt without sleep,
            // use boost::this_thread::interruption_point()
            // which also throws boost::thread_interrupted
            boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
        }
        catch(boost::thread_interrupted&)
        {
            cout << "Thread is stopped" << endl;
            break;
        }  
        
        for ( vector<int>::size_type i = 0; i != _connections.size(); i++ )
        {
            if ( _connections[i]->is_checked() )
            {
                if ( _connections[i]->get_server_computation().compare(_connections[i]->get_message()) == 0 )
                {
                    cout << " ... agree" << endl;
                }
                else
                {
                    cout << " ... disagree" << endl;
                }
            }
        }        
        
        param1 = "";
        param2 = "";
        ss.str(string());
        
    }while ( in != 'q' || !_running );
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
