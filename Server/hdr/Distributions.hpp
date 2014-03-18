/*******************************************************************************
 * Filename      : Distributions.hpp
 * Source File(s): Distributions.cpp
 * Description   :
 * Authors(s)    : 
 * Date Created  : 
 * Date Modified : 
 * Modifier(s)   : 
 *******************************************************************************/
#ifndef DISTRIBUTIONS_H
#define	DISTRIBUTIONS_H

/*******************************************************************************
*                                   INCLUDES
********************************************************************************/
#include <valarray>
#include <boost/random.hpp>
#include <boost/math/distributions.hpp>

namespace Distributions
{
/*******************************************************************************
*                                    DATATYPES
********************************************************************************/
typedef enum E_DISTRIBUTIONS
{
    NONE    = 0,
    BETA    = 1,
    UNIFORM = 2,
    NORMAL  = 3,
    GAMMA   = 4 
}DISTRIBUTIONS;
/*******************************************************************************
*                                    EXTERNS
********************************************************************************/
inline const char* DistToString(DISTRIBUTIONS e)
{
    switch (e)
    {
        case BETA:     return "beta";
        case UNIFORM:  return "uniform";
        case NORMAL:   return "normal";
        case GAMMA:    return "gamma";
        default:       return "none";
    }
}  
inline DISTRIBUTIONS DistToEnum(const char* in)
{
    if ( strcmp(in, "beta")      == 0 ) return BETA;
    if ( strcmp(in, "uniform")   == 0 ) return UNIFORM;
    if ( strcmp(in, "normal")    == 0 ) return NORMAL;
    if ( strcmp(in, "gamma")     == 0 ) return GAMMA;

    return NONE;
}  
/*******************************************************************************
*                                    DEFINES
********************************************************************************/
    
/*******************************************************************************
*  CLASS DEFINITIONS
********************************************************************************/
class Distribution
{
    public: 
        /* constructors */
        Distribution ( void );
        ~Distribution ( void );

        /* functions */
        double beta     ( void );
        double uniform  ( void );
        double normal   ( void );
        double gamma    ( void );
        
        bool user_pick ( DISTRIBUTIONS );
        
        void construct_distribution ( DISTRIBUTIONS );
        void set_parameters ( std::valarray<double>& );
        std::valarray<double>& get_parameters ( void );
        void clear_parameters ( void );
        
         
        
        /* variables */

    private:
        /* functions */

        /* variables */        
        std::valarray<double>                     parameters;
        boost::mt19937                            rng;
        boost::math::beta_distribution<>          _beta;
        boost::math::normal_distribution<>        _normal;
        boost::math::uniform_distribution<>       _uniform;
        boost::math::inverse_gamma_distribution<> _inv_gamma;
};
}
#endif
