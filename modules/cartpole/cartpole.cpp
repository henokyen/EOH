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
** cartpole.cc
** Login : <mouret@asuncion.lip6.fr>
** Started on  Sat Feb  9 21:36:03 2008 Jean-Baptiste MOURET
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

#include <iostream>
#include <assert.h>
#include "cartpole.hpp"

namespace cartpole
{  

  void Cartpole :: _integrate(double force, double dt)
  {
    _state[0] = _x;
    _state[1] = _x_d;
    unsigned p = 0;
    for (unsigned i = 2; i < _state.size(); i += 2)
      {
	_state[i] = _poles[p].get_theta();
	_state[i + 1] = _poles[p].get_theta_d();
	++p;
      }
    
    ///
    //force = 0;
    // for (unsigned i = 0; i < _state.size(); ++i)
//       std::cout<<"state["<<i<<"]"<<_state[i]<<std::endl;
//    std::cout<<"force :"<<force<<std::endl;
    _rk4(force, dt, _state, _step(*this));
    //_euler(force, dt, _state, _step_simple(*this));
    //
    _x = _state[0];
    _x_d = _state[1];
    p = 0;
    for (unsigned i = 2; i < _state.size(); i += 2)
      {
	_poles[p].set_theta(_state[i]);
	_poles[p].set_theta_d(_state[i + 1]);
	++p;
      }
  }
  
  // cf Sutton & Barto 
  void Cartpole::_step_simple :: operator()(double force,
					    const std::valarray<double> &state,
					    std::valarray<double> &derivs)
  {
    assert(derivs.size() == 4);
    assert(state.size() == 4);
    double theta = state[2];
    double theta_d = state[3];
    double m = _c._poles[0].get_mass();
    double l = _c._poles[0].get_length();
    double m_c = _c._mass;

    double signxp = 0;
    //double mup = 0;
	double muc = 0;
    double mc = m_c;
    double g = -cst::g;
    double tmp((force * (-1.f) - m * (l * (theta_d * (theta_d * (double) sin(theta)))) + muc * signxp) / (mc + m));
    double tmp2(4./3. - (m * (double)(::pow(cos(theta),2.)))/(mc + m));


    double theta_a = (g * sin(theta) + cos(theta) * tmp) / (l * tmp2);

    double x_a = (force + m * l * (theta_d * theta_d  * sin(theta) - theta_a  * cos(theta)))  / (m_c + m);

    derivs[0] = state[1];
    derivs[1] = x_a;
    derivs[2] = theta_d;
    derivs[3] = theta_a;
  }  

  // state : [x, x_d, theta_1, theta_d_1, ..., theta_n, theta_d_n]
  // derivs : [x_d, x_a, theta_d_1, theta_a_1, ...]
  // cf Wieland
  void Cartpole::_step :: operator()(double force, 
				     const std::valarray<double> &state,
				     std::valarray<double> &derivs)
  {
    unsigned k = state.size();
    double sum_fi = 0;
    double sum_mi = 0;

    unsigned p = 0;
    for (unsigned i = 2; i < k; i += 2)
      {
	assert(p < _c._poles.size());
	assert(i + 1 < state.size());
	double theta = state[i];
	double theta_d = state[i + 1];
	double cos_theta_i = cosf(theta);
	double sin_theta_i = sinf(theta);
	double m_i = _c._poles[p].get_mass();
	double ml_i = _c._poles[p].get_length() * m_i;

	double m_eff_i = m_i * (1.0 - 3.0 / 4.0 * cos_theta_i * cos_theta_i);
	double f_eff_i = 
	  ml_i * theta_d * theta_d * sin_theta_i 
	  + 3.0 / 4.0 * m_i * cos_theta_i * (cst::mu_p * theta_d / ml_i + cst::g * sin_theta_i);
      
	sum_fi += f_eff_i;
	sum_mi += m_eff_i;
	++p;
      }
    // there is a problem with the undefined sign function (cf wieland) : if x_d is
    // null or quasi-null, it applies a small force ... destabilizing everything
    double x_a = (force + sum_fi) / (_c._mass + sum_mi);
    derivs[1] = x_a;
    for (unsigned i = 0; i < k; i += 2)
      derivs[i] = state[i + 1];
    
    p = 0;
    for (unsigned i = 2; i < k; i += 2)
      {
	double l_i = _c._poles[p].get_length();
	double theta_i = state[i];
	double theta_d_i = state[i + 1];
	double m_i = _c._poles[p].get_mass();
	double theta_i_a = 
	  - 3.0 / (4.0 * l_i) 
	  * (x_a * cos(theta_i)
	     + cst::g * sin(theta_i)
	     + cst::mu_p * theta_d_i / (m_i * l_i));
	assert(i + 1 < derivs.size());
	derivs[i + 1] = theta_i_a;
	++p;
      }

  }


  template<typename F>
  void Cartpole :: _euler(double f, double h, 
			  std::valarray<double>& y,
			  F step)
  {  
    assert(y.size());
    if (_k1.size() != y.size())
      _k1.resize(_state.size());
    step(f, y, _k1);

    y = y + _k1 * h;
  }


}
