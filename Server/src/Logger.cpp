/******************************************************************************
 * Filename      : Logger.cpp
 * Header File(s): Logger.hpp
 * Description   :
 * Authors(s)    : 
 * Date Created  : 
 * Date Modified :
 * Modifier(s)   :
*******************************************************************************/
/************************************
 * Included Headers 
 ************************************/
#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <fstream>
#include <ctime>
#include <boost/filesystem.hpp>
#ifdef WIN32
    #include <direct.h>
#else
#endif
#include "Logger.hpp"

/************************************
 * Namespaces 
 ************************************/
using namespace std;

/************************************
 * Local Variables 
 ************************************/

/*******************************************************************************
* Constructor  : (Default)
* Description  : 
* Arguments    : 
* Remarks      : 
********************************************************************************/
Logger::Logger ( bool logging, bool timestamp )
{ 
    this->logging = logging;
    this->timestamp = timestamp;
    _init();
}
/*******************************************************************************
* Constructor  : (Default)
* Description  : 
* Arguments    : 
* Remarks      : 
********************************************************************************/
Logger::Logger ( void )
{ 
    this->logging = true;
    this->timestamp = false;
    _init();
}
/*******************************************************************************
* Constructor  : (Copy)
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
Logger::Logger ( Logger& obj ) { ; }
/*******************************************************************************
* Deconstructor: 
* Description  : 
* Arguments    : 
* Remarks      : 
********************************************************************************/
Logger::~Logger ( void ) { ; }
/*******************************************************************************
* Constructor  : (Assignment)
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
Logger& Logger::operator=( const Logger& obj ) 
{
    if (this != &obj) // prevent self-assignment
    {
        ;
    }
    return *this;
}
/*******************************************************************************
* Function     : void _init (void)
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Logger::_init (void)
{
    /* create base directory for logging */
#ifdef WIN32
    _mkdir("log/");
#else
#endif
}
/*******************************************************************************
* Function     : void enable_logging ( void )
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Logger::enable_logging ( void )
{
    this->logging = true;
}
/*******************************************************************************
* Function     : void disable_logging ( void )
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Logger::disable_logging ( void )
{
    this->logging = false;
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Logger::log ( string directory, string filename, string msg )
{

    ofstream myfile;
    myfile.open (filename.c_str(), ios::out | ios::app);
    myfile << msg << endl;
    myfile.close();
}