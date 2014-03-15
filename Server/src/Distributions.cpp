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
void Distribution::construct_distributions ( void )
{

    if ( parameters[0] == 0 || parameters[1] == 0)
    {
        beta_distribution<> tmp_beta(1, 1);
        _beta = tmp_beta;
    }
    else
    {
        beta_distribution<> tmp_beta(parameters[0], parameters[1]);
        _beta = tmp_beta;
    }
    
    if ( parameters[0] < 0 || parameters[1] > 0)
    {
        uniform_distribution<> tmp_uniform(0, 1);
        _uniform = tmp_uniform;
    }
    else
    {
        uniform_distribution<> tmp_uniform(parameters[0], parameters[1]);
        _uniform = tmp_uniform;
    }

    if ( parameters[0] == 0 || parameters[1] == 0)
    {
        normal_distribution<> tmp_normal(1, 1);
        _normal = tmp_normal;
    }
    else
    {
        normal_distribution<> tmp_normal(parameters[0], parameters[1]);
        _normal = tmp_normal;
    }

    if ( parameters[0] == 0 || parameters[1] == 0)
    {
        boost::gamma_distribution<> tmp_gamma(  1 );
        _gamma = tmp_gamma;
    }
    else
    {
        boost::gamma_distribution<> tmp_gamma( ( parameters[0]*parameters[0] )/parameters[1] );
        _gamma = tmp_gamma;
    }      

}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
double Distribution::beta ( void )
{
    return (quantile(_beta,parameters[2]?parameters[2]:(rand()/double(RAND_MAX))));
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
double Distribution::uniform ( void )
{   
    return (quantile(_uniform,parameters[2]?parameters[2]:(rand()/double(RAND_MAX))));
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
double Distribution::normal ( void )
{
    normal_distribution<> _normal(parameters[0], parameters[1]);
    
    return (quantile(_normal,parameters[2]?parameters[2]:(rand()/double(RAND_MAX))));
}
/*******************************************************************************
* Function     : 
* Description  : 
* Arguments    : 
* Returns      : 
* Remarks      : 
********************************************************************************/
double Distribution::gamma ( void )
{
    boost::variate_generator<boost::mt19937&,boost::gamma_distribution<> > gd( rng, _gamma );

    return (parameters[1]/parameters[0])*gd();
}
}