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

/*******************************************************************************
*                                    EXTERNS
********************************************************************************/

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
        
        void construct_distributions ( void );
        void set_parameters ( std::valarray<double>& );
        std::valarray<double>& get_parameters ( void );
        void clear_parameters ( void );
        
         
        
        /* variables */

    private:
        /* functions */

        /* variables */        
        std::valarray<double> parameters;
        boost::mt19937                                rng;
        boost::math::beta_distribution<>              _beta;
        boost::math::uniform_distribution<>           _uniform;
        boost::math::normal_distribution<>            _normal;
        boost::gamma_distribution<>                   _gamma;
};
}
#endif
