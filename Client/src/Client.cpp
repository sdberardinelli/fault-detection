/*******************************************************************************
 * Filename      : Client.cpp
 * Header File(s): Client.hpp
 * Description   :
 * Authors(s)    : 
 * Date Created  : 
 * Date Modified :
 * Modifier(s)   :
 *******************************************************************************/
/************************************
 * Included Headers 
 ************************************/
#include "Client.hpp"
#include "TestFunctions.hpp"
#include "Message.hpp"
#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/write.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string/split.hpp>                                      
#include <boost/algorithm/string.hpp> 
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <iostream>
#include <vector>
#include <valarray>
#include <thread>
#include <chrono>

/************************************
 * Namespaces 
 ************************************/
using namespace std;
using namespace TestFunctions;
using namespace Distributions;
using namespace boost::filesystem;
using boost::asio::deadline_timer;
using boost::asio::ip::tcp;

/************************************
 * Local Variables 
 ************************************/

/*******************************************************************************
* Constructor  : 
* Description  : 
* Arguments    : 
* Remarks      : 
********************************************************************************/
Client::Client ( boost::asio::io_service& io_service ) : _io_service(io_service),
                                                          _socket(io_service)
{
    tcp::resolver resolver(io_service);
    vector<string> strs;
    valarray<double> parameters;
    std::string str,ip,port; 
    boost::filesystem::ifstream file_in;
    file_in.open(CONFIG_FILENAME,ios::in);

    if ( !file_in )
    {
        _fault = false;
        _dist_type = NONE;
    }
    else
    {
        try
        {
            getline(file_in, str);
            boost::algorithm::split(strs,str,boost::is_any_of("="));
            ip = strs[1];
            getline(file_in, str);
            boost::algorithm::split(strs,str,boost::is_any_of("="));   
            port = strs[1];
            getline(file_in, str);
            boost::algorithm::split(strs,str,boost::is_any_of("="));
            _fault = (strs[1].compare(("true"))?false:true);
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
        }
        catch(const boost::filesystem::ifstream::failure & ex)
        {
            _fault = false;
            _dist_type = NONE;
        }
    }
    do_connect(resolver.resolve({ ip, port }));
}
/*******************************************************************************
* Deconstructor: 
* Description  : 
* Arguments    : 
* Remarks      : 
********************************************************************************/
Client::~Client ( void ) { ; }
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Client::write ( const Message& msg )
{
    _io_service.post([this, msg]()
                     {
                         bool write_in_progress = !_write_msgs.empty();
                         _write_msgs.push_back(msg);
                         if (!write_in_progress)
                         {
                             do_write();
                         }
                     });
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Client::close ( void )
{
    _io_service.post([this]() { _socket.close(); });
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
bool Client::is_connected ( void )
{
    return _socket.is_open();
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Client::run ( void )
{
    thread t([this](){ _io_service.run(); });

    char line[Message::max_body_length + 1];
    while ( is_connected() )
    {
        this_thread::sleep_for (chrono::seconds(10));
    }

    close();
    t.join();
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Client::do_connect ( tcp::resolver::iterator endpoint_iterator )
{
    boost::asio::async_connect(_socket, 
                               endpoint_iterator,
                                [this](boost::system::error_code ec, 
                                       tcp::resolver::iterator)
                                {
                                    if (!ec)
                                    {
                                        do_read_header();
                                    }
                                });
}

/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Client::do_read_header ( void )
{
    boost::asio::async_read(_socket,
                            boost::asio::buffer(_read_msg.data(), 
                                                Message::header_length),
                            [this](boost::system::error_code ec, size_t )
                            {
                                if ( !ec && _read_msg.decode_header() )
                                {
                                    do_read_body();
                                }
                                else
                                {
                                    _socket.close();
                                }
                            });
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Client::do_read_body ( void )
{
    boost::asio::async_read(_socket,
                           boost::asio::buffer(_read_msg.body(), 
                                               _read_msg.body_length()),
                           [this](boost::system::error_code ec, size_t )
                           {
                               if (!ec)
                               {
                                   vector<string> strs;
                                   string str(_read_msg.body());
                                   if (!str.empty())
                                   {   
                                       Message msg;
                                       //boost::algorithm::split(strs,str,boost::is_any_of(","));
                                       //msg = process_message(ToEnum(strs[0].c_str()),strs);
                                       //write(msg);
                                       
                                       
                                        string _message = "hey, i got it";

                                        msg.body_length(strlen(_message.c_str()));
                                        memcpy(msg.body(), _message.c_str(), msg.body_length());
                                        msg.encode_header();   
                                        
                                        write(msg);
                                   }                
                                   
                                   do_read_header();
                               }
                               else
                               {
                                   _socket.close();
                               }
                           });
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Client::do_write ( void )
{
    boost::asio::async_write(_socket,
                             boost::asio::buffer(_write_msgs.front().data(),
                                                 _write_msgs.front().length()),
                             [this]( boost::system::error_code ec, size_t )
                             {
                                 if (!ec)
                                 {
                                     cout << "writting" << endl;
                                     _write_msgs.pop_front();
                                     if (!_write_msgs.empty())
                                     {
                                        do_write();
                                     }
                                 }
                                 else
                                 {
                                     _socket.close();
                                 }
                            });
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
Message Client::process_message ( TEST_FUNCTIONS CMD, vector<string>& strs )
{    
    stringstream ss;
    Message msg;  
    double probability;
    string param1 = strs[1], param2 = strs[2], _reply_message;
    static const char modification[] = "0123456789";
    
    if ( _fault )
    {
        probability = _dist.user_pick(_dist_type);
        
        if ( double(rand()/double(RAND_MAX)) < probability )
        {
            if ( rand()/double(RAND_MAX) < 0.51 )
                param1[rand()%(param1.length()-1)] = modification[rand()%(sizeof(modification)-1)];
            if ( rand()/double(RAND_MAX) < 0.51 )
                param2[rand()%(param2.length()-1)] = modification[rand()%(sizeof(modification)-1)];
        }
        else
        {
            valarray<double> parameters = _dist.get_parameters();
            
            try
            {
                param1[int(parameters[0])] = modification[int(parameters[2])];
                param2[int(parameters[1])] = modification[int(parameters[2])];
            }
            catch ( const out_of_range& oor )
            {
                param1[0] = modification[1];
                param2[0] = modification[1];                
            }
        }
    }
        
    switch ( CMD )
    {
        case TestFunctions::DO_ADD:
        {        
            int value = _tf.do_add(atoi(param1.c_str()), atoi(param2.c_str()));
            ss << value;
            _reply_message = (ss.str()+"\n");
        }
            break;
        case TestFunctions::DO_STR:
        {
            string value = _tf.do_string_cat(param1, param2);
            _reply_message = (value+"\n");

        }
            break;
        case TestFunctions::DO_MUL:
        {
            int value = _tf.do_multipy(atoi(param1.c_str()), atoi(param2.c_str()));
            ss << value;
            _reply_message = (ss.str()+"\n");
        }
            break;
        default:;
            break;
    }  
    
    msg.body_length(strlen(_reply_message.c_str()));
    memcpy(msg.body(), _reply_message.c_str(), msg.body_length());
    msg.encode_header();      
    ss.str(string()); 
    
    return msg;
}