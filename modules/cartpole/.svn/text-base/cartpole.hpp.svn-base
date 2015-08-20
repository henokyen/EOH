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




/*
** cartpole.hh
** Login : <mouret@asuncion.lip6.fr>
** Started on  Sat Feb  9 21:34:11 2008 Jean-Baptiste MOURET
** $Id$
** 
** Copyright (C) 2008 Jean-Baptiste MOURET
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef   	CARTPOLE_HH_
# define   	CARTPOLE_HH_

#include <valarray>
#include <vector>
#include <cassert>

namespace cartpole
{
  namespace cst
  {
    static const float g = -9.81;
    //static const float mu_c = 0.0005;
    static const double mu_c = 0.0; // cf NEAT code & doncieux code
    //    static const double mu_p = 0.000002;
    static const double mu_p = 0.0f;
    static const double mass_cart = 1.0;
    static const double mass_pole = 0.1;
    static const double length_pole = 0.5;
    static const double force_mag = 10.0;  
  }
  
  class Pole
  {
  public:
    Pole(double mass, double length) :
      _mass(mass),
      _length(length),
      _theta_d(0.0f),
      _theta(0.0f)
    {}
    void reset() { _theta_d = 0; _theta = 0; }
    double get_length() const { return _length; }
    double get_mass() const { return _mass; }
    double get_theta_d() const { return _theta_d; }
    double get_theta() const { return _theta; }
    void set_theta_d(double d) { _theta_d = d; }
    void set_theta(double t) { _theta = t; }
  protected:
    double _mass;
    double _length;
  
    double _theta_d;
    double _theta;
  };
  
  class Cartpole
  {
  public:
    Cartpole() : _mass(-1),  _x_d(0.0f), _x(0.0f), _state(2) {}
    Cartpole(double mass, double xmin, double xmax) : 
      _mass(mass),
      _xmin(xmin),
      _xmax(xmax),
      _x_d(0.0f),
      _x(0.0f),
      _state(2)
    {}
    void init(double mass, double xmin, double xmax)
    {
      _mass = mass;
      _xmin = xmin;
      _xmax = xmax;
      _x_d = 0.0f;
      _x = 0.0f;
      reset();
    }
    void reset()
    {
      _x = 0.0f;
      _x_d = 0.0f;
      for (unsigned i = 0; i < _poles.size(); ++i)
	_poles[i].reset();
    }
    void add_pole(const Pole& p) { _poles.push_back(p); _state.resize(_poles.size() * 2 + 2); }
    void step(double force, double dt) { _integrate(force, dt); }
    void set_x(double x) { _x = x; }
    double get_x() const { return _x;}
    double get_x_d() const { return _x_d; }
    const Pole& get_pole(unsigned i) const { return _poles[i]; }
    Pole& get_pole(unsigned i) { return _poles[i];}
    unsigned get_nb_poles() const { return _poles.size(); }
    double get_mass() const { return _mass; }

  protected:
    int sign(double f) { return (f > 0 ? 1 : -1) ; }
    void _integrate(double force, double dt);
    template<typename F>
    void _rk4(double force, double h, std::valarray<double>& y, F step);
    template<typename F>
    void _euler(double force, double h, std::valarray<double>& y, F step);

    // neat implementation
    void _neat_simu(double force);
    void _neat_rk4(double f, double y[], double dydx[], double yout[]);
    void _neat_step(double action, double *st, double *derivs);




    std::vector<Pole> _poles;
    double _mass;
    double _xmin;
    double _xmax;
    
    double _x_d;
    double _x;
    
    std::valarray<double> _state;
    std::valarray<double> _k1;
    std::valarray<double> _k2;
    std::valarray<double> _k3;
    std::valarray<double> _k4;

    // functors
    struct _step 
    { 
      Cartpole& _c;
      _step(Cartpole& c) : _c(c) {}
      void operator()(double force, const std::valarray<double> &state,
		      std::valarray<double> &deriv);
    };
    struct _step_simple
    {
      Cartpole& _c;
      _step_simple(Cartpole& c) : _c(c) {}
      void operator()(double force,
		      const std::valarray<double> &state,
		      std::valarray<double> &derivs);
    };


  };

  template<typename F>
  inline void Cartpole :: _rk4(double f, double h, 
			       std::valarray<double>& y,
			       F step)
  {
    assert(y.size());
    if (_k1.size() != y.size())
      {
	_k1.resize(_state.size());
	_k2.resize(_state.size());
	_k3.resize(_state.size());
	_k4.resize(_state.size());
      }
    double h2 = h / 2.0;
    
    step(f, y, _k1);
    step(f, y + h2 * _k1, _k2);
    step(f, y + h2 * _k2, _k3);
    step(f, y + h  * _k3, _k4);
    
    y += h / 6.0 * (_k1 + 2.0 * _k2 + 2.0 * _k3 + _k4);
  }

}


#endif	    /* !CARTPOLE_HH_ */
