/*******************************************************************************
 * Filename      : Distributions.cpp
 * Header File(s): Distributions.hpp
 * Description   :
 * Authors(s)    : 
 * Date Created  : 
 * Date Modified :
 * Modifier(s)   :
 *******************************************************************************/
/************************************
 * Included Headers 
 ************************************/
#include "Distributions.hpp"
#include <valarray>
#include <cstdlib>
#include <ctime>
#include <boost/random.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/gamma_distribution.hpp>
#include <boost/math/distributions.hpp>
  
/************************************
 * Namespaces 
 ************************************/
using namespace std;
using namespace boost::math;

/************************************
 * Local Variables 
 ************************************/

namespace Distributions
{
/*******************************************************************************
* Constructor  : 
* Description  : 
* Arguments    : 
* Remarks      : 
********************************************************************************/
Distribution::Distribution ( void ) { srand(time(NULL)); }
/*******************************************************************************
* Deconstructor: 
* Description  : 
* Arguments    : 
* Remarks      : 
********************************************************************************/
Distribution::~Distribution ( void ) { ; }
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Distribution::set_parameters ( valarray<double>& _parameters )
{
    parameters = _parameters;
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
valarray<double>& Distribution::get_parameters ( void )
{
    return parameters;
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Distribution::clear_parameters ( void )
{
    parameters.resize(0);
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
void Distribution::construct_distribution ( DISTRIBUTIONS d )
{
    if ( parameters.size() < 2 )
    {
        parameters.resize(3);
    }
    
    switch ( d )
    {
        case BETA:
        {
            if ( parameters[0] == 0 || parameters[1] == 0)
            {
                parameters[0] = 0.5;
                parameters[1] = 0.5;
            }
            boost::math::beta_distribution<> _b(parameters[0],parameters[1]);
            _beta = _b;

        }
            break;
        case UNIFORM:
        {
            if ( parameters[0] > parameters[1] )
            {
                parameters[0] = 0;
                parameters[1] = 1;
            }
            boost::math::uniform_distribution<> _u(parameters[0], parameters[1]);
            _uniform = _u;            
        }
            break;
        case NORMAL:
        {
            if ( parameters[1] < 0 )
            {
                parameters[0] = 0;
                parameters[1] = 1; 
            }
            
            boost::math::normal_distribution<> _n(parameters[0], parameters[1]);
            _normal = _n;
        }
            break;
        case GAMMA:
        {
            if ( parameters[0] == 0 || parameters[1] == 0 )
            {
                parameters[0] = 1;
                parameters[1] = 1; 
            }
            
            boost::math::inverse_gamma_distribution<> _i_g( ( parameters[0]*parameters[0] )/parameters[1] );
            _inv_gamma = _i_g;
        }
            break;
    }
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : parameters=a,b,P(x less than X); e.g, parameters=0.5,0.5,0.5
********************************************************************************/
double Distribution::beta ( void )
{
    return cdf(_beta,(rand()/double(RAND_MAX)));
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : parameters=a,b,P(x less than X); e.g., parameters=0,2,0.5
********************************************************************************/
double Distribution::uniform ( void )
{
    return cdf(_uniform,(rand()/double(RAND_MAX)));
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : parameters=u,s,P(x less than X); e.g., parameters=0,1,0.5
********************************************************************************/
double Distribution::normal ( void )
{
    return cdf(_normal,(rand()/double(RAND_MAX)));
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : parameters=a,B,P(x less than X);parameters=1,1,0.5
********************************************************************************/
double Distribution::gamma ( void )
{
    return cdf(_inv_gamma,(rand()/double(RAND_MAX)));
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
bool Distribution::user_pick ( DISTRIBUTIONS d )
{
    switch (d)
    {
        case BETA:     
            return ((beta() < parameters[2])?true:false);
        case UNIFORM:  
            return (uniform() < parameters[2])?true:false;
        case NORMAL:   
            return ((normal() < parameters[2])?true:false);
        case GAMMA:  
            return ((gamma() < parameters[2])?true:false);
        default:       
            return false;
    }
}
}