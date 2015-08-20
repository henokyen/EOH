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




#ifndef SIMU_CARTPOLE_HPP_
#define SIMU_CARTPOLE_HPP_

#include <sferes/simu/simu.hpp>
#include "cartpole.hpp"
#include "display.hpp"

namespace sferes
{
  namespace simu
  {
    // params :
    // Params::simu::x_min
    // Params::simu::x_max
    // Params::simu::pole_mass
    // Params::simu::pole_length
    // Params::simu::dt
    // Params::simu::display
    SFERES_SIMU(SimuCartpole, Simu)
    {
    public:
      SimuCartpole() : 
	_cartpole(cartpole::cst::mass_cart,
		  Params::simu::x_min,
		  Params::simu::x_max)
	{
	  init();
	}
      // required
      void init()
      {
	if (!_cartpole.get_nb_poles())
	  _cartpole
	    .add_pole(cartpole::Pole(Params::simu::pole_mass, 
				     Params::simu::pole_length));
	_cartpole.reset();
      }
      void refresh()
      {
	_cartpole.step(_force, Params::simu::dt);
      }
      // simu-specific

      double get_theta() const 
      {
	assert(_cartpole.get_nb_poles()); 
	return _cartpole.get_pole(0).get_theta(); 
      }
      double get_x() const { return _cartpole.get_x(); }

      void set_force(double f) { _force = f;}
      void set_theta(float angle) 
      { 
	assert(_cartpole.get_nb_poles()); 
	_cartpole.get_pole(0).set_theta(angle); 
      }
      void set_x(float x) { _cartpole.set_x(x); }
      void refresh_view() { _refresh_display(); }
      void init_view() { _init_display(); }
    protected:
      cartpole::Cartpole _cartpole;
      float _force;
      sdl::Display* _display;
      void _init_display()
      {
	_display = new sdl::Display(500, 500);
      }
      void _refresh_display()
      {
	float l = 50;
	float x_base = _display->get_w() / 2;
	float y_base = _display->get_h() - 20;
	float scale = 250;
	
	
	_display->fill_black();
	float x = _cartpole.get_x();
	float xs = x * scale + x_base;
	_display->line(xs - l, y_base, xs + l, y_base, 200, 0, 0);
	_display->circle(xs - 40, y_base, 10.0f, 100, 0, 0);
	_display->circle(xs + 40, y_base, 10.0f, 100, 0, 0);
	_display->circle(xs, y_base, 5.0f, 150, 0, 0);
	for (unsigned i = 0; i < _cartpole.get_nb_poles(); ++i)
	  {
	    float xp1 = cosf(_cartpole.get_pole(i).get_theta() 
			     + M_PI / 2.0f) * _cartpole.get_pole(i).get_length();
	    float yp1 = sinf(_cartpole.get_pole(i).get_theta() 
			     + M_PI/2.0f) * _cartpole.get_pole(i).get_length();
	    float xp1s = xs + xp1 * scale;
	    float yp1s = y_base - yp1 * scale;
	    _display->line(xs, y_base, xp1s, yp1s, 200, 0, 0);
	  }
	
	_display->update();
	usleep((int)1e4);
      }      
    };
  }
}

#endif
