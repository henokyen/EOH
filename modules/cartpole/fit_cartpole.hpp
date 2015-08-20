//| This file is a part of the sferes2 framework.
//| Copyright 2009, ISIR / Universite Pierre et Marie Curie (UPMC)
//| Main contributor(s): Jean-Baptiste Mouret, mouret@isir.fr
//|
//| This software is a computer program whose purpose is to facilitate
//| experiments in evolutionary computation and evolutionary robotics.
//| 
//| This software is governed by the CeCILL license under French law
//| and abiding by the rules of distribution of free software.  You
//| can use, modify and/ or redistribute the software under the terms
//| of the CeCILL license as circulated by CEA, CNRS and INRIA at the
//| following URL "http://www.cecill.info".
//| 
//| As a counterpart to the access to the source code and rights to
//| copy, modify and redistribute granted by the license, users are
//| provided only with a limited warranty and the software's author,
//| the holder of the economic rights, and the successive licensors
//| have only limited liability.
//|
//| In this respect, the user's attention is drawn to the risks
//| associated with loading, using, modifying and/or developing or
//| reproducing the software by the user in light of its specific
//| status of free software, that may mean that it is complicated to
//| manipulate, and that also therefore means that it is reserved for
//| developers and experienced professionals having in-depth computer
//| knowledge. Users are therefore encouraged to load and test the
//| software's suitability as regards their requirements in conditions
//| enabling the security of their systems and/or data to be ensured
//| and, more generally, to use and operate it in the same conditions
//| as regards security.
//|
//| The fact that you are presently reading this means that you have
//| had knowledge of the CeCILL license and that you accept its terms.




#ifndef FIT_CARTPOLE_HPP_
#define FIT_CARTPOLE_HPP_

#include <sferes/fit/fitness_simu.hpp>

namespace sferes
{
  namespace fit
  {
    SFERES_FITNESS_SIMU(FitCartpole, FitnessSimu)
    {
    public:
      static const float theta_max = Params::fit::theta_max;
      static const float x_max = Params::fit::x_max;

      FitCartpole() : 
	_error_theta(0), 
	_error_x(0),
	_steps_ok(0)
	  { this->_objs.resize(2); }
      template<typename Phen>
	int refresh(Phen& p) 
      {
	float e_x = fabs(this->_simu.get_x());
	float e_t = fabs(this->_simu.get_theta());
	if (isnan(e_x) || isnan(e_t))
	  return -1;
	_error_x += e_x;
	_error_theta += e_t;
	if (e_x > x_max || e_t > theta_max)
	    return -1;
	++_steps_ok;
	return 0;
      }

      template<typename Phen>
	void refresh_end_exp(Phen& p)
      {}

      template<typename Phen>
	void refresh_end_eval(Phen& p) 
      {
	float e_theta = (this->_step - _steps_ok) * theta_max;
	float e_x     = (this->_step - _steps_ok) * x_max;	
	this->_objs[0] = 1 - (_error_theta + e_theta) / this->_step;
	this->_objs[1] = 1 - (_error_x + e_x) / this->_step;
      }

      template<typename Phen>
	void scheduler(Phen& p) // define yours !
      { assert(0);}
    protected:
      float _error_theta;
      float _error_x;
      int _steps_ok;
    };
  }
}

#endif
