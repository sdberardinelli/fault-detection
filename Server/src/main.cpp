/*******************************************************************************
 * Filename      : main.cpp
 * Header File(s): 
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
#include <iostream>
#include <boost/asio.hpp>

/************************************
 * Namespaces 
 ************************************/
using namespace std;

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
int main ( int argc, char* argv[] ) 
{
    
    if ( argc < 2 || argc > 2)
    {
        cout << "Provide Port or Too many arguments" << endl;
        exit(0);
    }
    
    try
    {
        boost::asio::io_service io_service;
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), atoi(argv[1]));
        Server server(io_service, endpoint);
        server.start();
        io_service.run();
    }
    catch (std::exception& e)
    {
        cerr << e.what() << endl;
    }

    cout << "Done." << endl;
        
    return 0;
}

