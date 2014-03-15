/*******************************************************************************
 * Filename      : TestFunctions.hpp
 * Source File(s): TestFunctions.cpp
 * Description   :
 * Authors(s)    : 
 * Date Created  : 
 * Date Modified : 
 * Modifier(s)   : 
 *******************************************************************************/
#ifndef TESTFUNCTIONS_H
#define	TESTFUNCTIONS_H

/*******************************************************************************
*                                   INCLUDES
********************************************************************************/
#include <string>

namespace TestFunctions
{
/*******************************************************************************
*                                    DATATYPES
********************************************************************************/
typedef enum E_TEST_FUNCTIONS
{
    DO_ADD = 49,
    DO_STR = 50,
    DO_MUL = 51
}TEST_FUNCTIONS;
/*******************************************************************************
*                                    EXTERNS
********************************************************************************/
inline const char* ToString(TEST_FUNCTIONS v)
{
    switch (v)
    {
        case DO_ADD:  return "1";
        case DO_STR:  return "2";
        case DO_MUL:  return "3";
        default:      return "0";
    }
}
/*******************************************************************************
*                                    DEFINES
********************************************************************************/
    
/*******************************************************************************
*  CLASS DEFINITIONS
********************************************************************************/
class TestFunction
{
    public: 
        /* constructors */
        TestFunction ( void );
        ~TestFunction ( void );

        /* functions */
        int do_add ( int, int );
        std::string do_string_cat ( std::string, std::string );
        int do_multipy ( int, int );

        /* variables */

    private:
        /* functions */

        /* variables */        

};
}
#endif
