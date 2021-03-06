/******************************************************************************
 * Filename      : Logger.hpp
 * Source File(s): Logger.cpp
 * Description   :
 * Authors(s)    : 
 * Date Created  : 
 * Date Modified :
 * Modifier(s)   :
 *******************************************************************************/
#ifndef LOGGER_H
#define LOGGER_H

/*******************************************************************************
*  INCLUDES
********************************************************************************/
#include <string>

/*******************************************************************************
*  DEFINES
********************************************************************************/

/*******************************************************************************
*  MACROS
********************************************************************************/

/*******************************************************************************
*  DATA TYPES
********************************************************************************/

/*******************************************************************************
*  EXTERNALS
********************************************************************************/

/*******************************************************************************
*  CLASS DEFINITIONS
********************************************************************************/
class Logger
{
    public: 
        /* constructors */
        Logger ( bool, bool );
        Logger ( void ); /* default */
        Logger ( Logger& ); /* copy */
        Logger& operator= ( const Logger& ); /* assign */
        ~Logger ( void );
        /* functions */
        void log (std::string,std::string,std::string);
        void enable_logging ( void );
        void disable_logging ( void );
        void enable_timestamp ( void );
        void disable_timestamp ( void );
    private:
        void _init (void);
        bool logging;
        bool timestamp;
};
#endif