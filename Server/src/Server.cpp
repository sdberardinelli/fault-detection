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
#include <boost/algorithm/string/split.hpp>                                      
#include <boost/algorithm/string.hpp> 
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <thread>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <valarray>
#include <utility>
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
Server::Server ( boost::asio::io_service& io, 
                 const boost::asio::ip::tcp::endpoint& endpoint ) : _acceptor(io, endpoint), 
                                                                    _socket(io)
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
            _sending_dist_type = DistToEnum(strs[1].c_str());
            getline(file_in, str);
            boost::algorithm::split(strs,str,boost::is_any_of("="));
            boost::algorithm::split(strs,strs[1],boost::is_any_of(","));
            
            parameters.resize(strs.size());
            for ( vector<int>::size_type i = 0; i != strs.size(); i++ )
            {
                parameters[i] = atof(strs[i].c_str());
            }
            
            _sending_dist.set_parameters(parameters);
            _sending_dist.construct_distribution(_sending_dist_type);            
           
            getline(file_in, str);
            boost::algorithm::split(strs,str,boost::is_any_of("="));
            _quorum_ratio = atof(strs[1].c_str());
            
            getline(file_in, str);
            boost::algorithm::split(strs,str,boost::is_any_of("="));
            _client_ratio = atof(strs[1].c_str());            
            
            getline(file_in, str);
            boost::algorithm::split(strs,str,boost::is_any_of("="));
            _timeout = atoi(strs[1].c_str());  
            
            _log = new Logger(true,false);
        }
        catch(const boost::filesystem::ifstream::failure & ex)
        {
            ;
        }
    }
    
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
    _acceptor.async_accept(_socket,
                           [this](boost::system::error_code ec)
                           {
                               if (!ec)
                               {
                                   make_shared<Connection>(move(_socket), _mp)->start();
                               }
                               start_accept();
                           });
}
