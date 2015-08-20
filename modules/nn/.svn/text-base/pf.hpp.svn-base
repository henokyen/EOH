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




#ifndef NN_PF_HPP
#define NN_PF_HPP

#include <cstdlib>

// potential functions
namespace nn
{ 
  template<int NbParams = 0>
  class PfT
  {
  public:
    const std::vector<float>& get_params() const { return _params; }
    template<typename T>
    T operator()(const std::valarray<T>& weights, 
		 const std::valarray<T>& inputs) 
    { assert(0); return T(); }
    void set_params(const std::vector<float>& v) {}
    static const size_t nb_params = NbParams;
  protected:
    std::vector<float> _params;
  };

  // oscillators :
  // \frac{\tau_1 (1 - w_{22}) + \tau_2 (1 - w_{11})}{\tau_1 \tau_2}  >= 0
  // \frac{w_{11} w_{22} - w_{12} w_{21} - w_{11} - w_{22} + 1}{\tau_1 \tau_2} > 0
  // cf Graben et al. 	
  // Lectures in Supercomputational Neurosciences Dynamics in Complex Brain Networks
  class PfLeaky : public PfT<>
  {
  public:
    static const float default_tau  = 0.075f;// tau \in [0.003,0.1]
    static const float dt = 0.001f;
    void set_params(const std::vector<float>& v) 
    { 
      if (v.size() >= 1)
	_tau = v[0]; 
      _params = v;
    }
    PfLeaky() : _m(0), _tau(default_tau) {}
    float operator()(const std::valarray<float>& weights, 
  		     const std::valarray<float>& inputs)
    {      
      float dmi_dt = (-_m + (weights * inputs).sum()) / _tau;
      _m += dmi_dt * dt;
      return _m;
    }
    static const size_t nb_params = 1;
  protected:
    float _m;
    float _tau;
  protected:
  };
  
  struct PfWSum : public PfT<>
  {
    void set_params(const std::vector<float>& v) {}
    float operator()(const std::valarray<float>& weights, 
  		     const std::valarray<float>& inputs) const
    {
      std::valarray<float> r = weights * inputs;
      return r.sum();
    }
    static const size_t nb_params = 0;
  };

  // -1 : false
  // 1 : true
  struct PfNand : public PfT<>
  {
    void set_params(const std::vector<float>& v) {}
    float operator()(const std::valarray<float>& weights,
  		     const std::valarray<float>& inputs) const
    {
      assert(inputs.size() <= 2);
      if (inputs.size() == 0)
	return -1;
      if (inputs.size() == 1)
	return inputs[0];
      bool a = (inputs[0] > 0);
      bool b = (inputs[1] > 0);
      bool r = !(a && b);
      return r ? 1 : -1;
    }
    static const size_t nb_params = 0;
  }; 


  // Ijsspert's coupled non-linear oscillators
  // See Science, 2007
  struct PfIjspeert : public PfT<>
  {
    typedef std::pair<float, float> io_t;
    static const float dt = 0.005;
    static const size_t nb_params = 3;
    static const float max_weight_ampl = 10.;
    static const float max_weight_phase = 2. * M_PI;
    PfIjspeert() :
      _theta(0),
      _r(0),
      _x(0),
      _r_dot(0),
      _a(20),
      _R(1),
      _v(1)
    {}
		   
    io_t operator()(const std::valarray<io_t>& weights,
  		     const std::valarray<io_t>& inputs)
    {
      // questions: initial values of _theta, _r_dot, _r ?
      assert(weights.size() == inputs.size());
      float theta_dot = 2 * M_PI * _v;
      for (size_t i = 0; i < weights.size(); ++i)
	theta_dot += 
	  inputs[i].first * (weights[i].first * max_weight_ampl) 
	  * sin(inputs[i].second - _theta - weights[i].second * max_weight_phase);
      _theta += theta_dot * dt;
      float r_dot_dot = _a * (_a / 4.0f * (_R - _r) - _r_dot);
      _r_dot += r_dot_dot * dt;
      _r += _r_dot * dt;
      _x = _r * (1 + cos(_theta));
      return std::make_pair(_r, _theta);
    }
    void init_params() 
    {
      _r = _R;
      _x = 0;
      _r_dot = 0;
    }

    void update_params() 
    {
      _params.resize(3);
      _params[0] = _R;
      _params[1] = _v;
      _params[2] =_theta;
    } 
    
    void set_params(const std::vector<float>& v) 
    { 
      _params = v;
      if (!v.empty())
	{
	  _R = _params[0];
	  _v = _params[1];
	  _theta = _params[2];
	}
      init_params();
    }
    float x() const { return _x; }
    float r() const { return _r; }

    void set_a(float a) { _a=a;}
    void set_R(float R) { _R=R;}
    void set_v(float v) { _v=v;}
    float a(void) const { return _a;}
    float R(void) const { return _R;}
    float v(void) const { return _v;}
  protected:
    // states
    float _theta;
    float _r;
    float _x;
    float _r_dot;
    
    // params
    float _a;
    float _R;
    float _v;
  };

}

#endif
