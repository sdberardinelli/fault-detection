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
#include "Client.hpp"
#include <boost/asio.hpp>
#include <iostream>

/************************************
 * Namespaces 
 ************************************/
using namespace std;
using boost::asio::ip::tcp;

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
    try
    {
        boost::asio::io_service io_service;
        Client c(io_service);
        io_service.run();
    }
    catch (std::exception& e)
    {
      std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
