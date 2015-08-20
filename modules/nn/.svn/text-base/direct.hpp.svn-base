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




#ifndef DIRECT_HPP
#define DIRECT_HPP

#include <vector>
#include <boost/foreach.hpp>
#include <boost/serialization/utility.hpp>
#include <sferes/stc.hpp> 
#include <sferes/misc.hpp>

namespace sferes
{
  namespace gen
  {
    namespace direct
    {
      struct Connection
      {
	Connection() {}
	Connection(size_t s, size_t t, float w) : 
	  src(s), target(t), weight(w) {}
	size_t src;
	size_t target;
	float weight;
	template<typename Archive>
	void serialize(Archive& a, unsigned v)
	{
	  a & BOOST_SERIALIZATION_NVP(src);
	  a & BOOST_SERIALIZATION_NVP(target);
	  a & BOOST_SERIALIZATION_NVP(weight);
	}
      };
      inline bool operator==(const Connection& c1, const Connection& c2)
      {
	return c1.src == c2.src && c1.target == c2.target;
      }

      enum weight_t { continuous = 0, sampled };

      template<int T, typename Params>
      struct MutationWeight_f
      {
	float operator()(float w) { assert(0); return 0.0f; }
      };
      template<int T, typename Params>
      struct RandomWeight_f
      {
	float operator()() { assert(0); return 0.0f; }
      };
      template<int T, typename Params>
      struct RandomBias_f
      {
	float operator()() { assert(0); return 0.0f; }
      };


    }

    // direct::weight_t Params::direct::weight_type (continous/sampled/?)
    // -- random --
    // size_t Params::direct::nb_inputs
    // size_t Params::direct::nb_outputs
    // size_t Params::direct::min_nb_neurons
    // size_t Params::direct::max_nb_neurons
    // size_t Params::direct::min_nb_conns
    // size_t Params::direct::max_nb_conns
    // float  Params::direct::max_weight
    // float  Params::direct::max_bias
    // --- mutation --
    // float  Params::direct::m_rate_add_conn
    // float  Params::direct::m_rate_del_conn
    // float  Params::direct::m_rate_change_conn
    // float  Params::direct::m_rate_add_neuron
    // float  Params::direct::m_rate_del_neuron
    // float  Params::direct::weight_sigma
    // float  Params::direct::m_rate_weight

    SFERES_CLASS(Direct)
    {
    public:
      void random()
      {
	// i/o neurons
	for (size_t i = 0; i < Params::direct::nb_inputs + Params::direct::nb_outputs; ++i)
	  _neurons.push_back(0.0f);
	
	// neurons
	size_t nb_neurons = misc::rand(Params::direct::min_nb_neurons,
				       Params::direct::max_nb_neurons);
	for (size_t i = 0; i < nb_neurons; ++i)
	  _neurons.push_back(_random_bias());
	// conns
	size_t nb_conns = misc::rand(Params::direct::min_nb_conns,
				     Params::direct::max_nb_conns);
	for (size_t i = 0; i < nb_conns; ++i)
	  _add_conn();
      }

      void mutate()
      {
	BOOST_FOREACH(direct::Connection& c, _conns)
	  _change_conn(c);

	_change_neurons();
	
	if (misc::rand<float>() < Params::direct::m_rate_add_conn)
	  _add_conn();
	
	if (misc::rand<float>() < Params::direct::m_rate_del_conn)
	  _del_conn();

	if (misc::rand<float>() < Params::direct::m_rate_add_neuron)
	  _add_neuron();
	
	if (misc::rand<float>() < Params::direct::m_rate_del_neuron)
	  _del_neuron();
	
      }

      // no cross-over
      void cross(const Direct& o, Direct& c1, Direct& c2) 
      {
	if (misc::flip_coin())
	  {
	    c1 = *this;
	    c2 = o;
	  }
	else
	  {
	    c2 = *this;
	    c1 = o;
	  }
      }
      size_t nb_inputs() const { return Params::direct::nb_inputs; }
      size_t nb_outputs() const { return Params::direct::nb_outputs; }
      const std::vector<direct::Connection>& conns() const { return _conns; }
      const std::vector<float>& neurons() const { return _neurons; }
      template<typename Archive>
	void serialize(Archive& a, unsigned v)
      {
	a & BOOST_SERIALIZATION_NVP(_neurons);
	a & BOOST_SERIALIZATION_NVP(_conns);
      }
    protected:
      // float = bias of each neuron
      std::vector<float> _neurons;
      std::vector<direct::Connection> _conns;
      direct::RandomWeight_f<Params::direct::weight_type, Params> _random_weight_f;
      direct::RandomBias_f<Params::direct::weight_type, Params> _random_bias_f;
      direct::MutationWeight_f<Params::direct::weight_type, Params> _mutation_weight_f;
      

      void _change_neurons()
      {
	BOOST_FOREACH(float& b, _neurons)
	  _change_weight(b);
      }
      void _add_conn()
      {
	assert(_neurons.size());
	size_t s = _rand_src();
	size_t t = _rand_tgt();
	float w = _random_weight_f();
	direct::Connection c(s, t, w);
	if (std::find(_conns.begin(), _conns.end(), c) == _conns.end())
	  _conns.push_back(c);	
      }
      
      void _del_conn()
      {
	if (_conns.size() <= 1)
	  return;
	assert(_conns.size());
	size_t k = misc::rand(_conns.size());
	_conns.erase(_conns.begin() + k);	
      }
      
      void _add_neuron()
      {
	_neurons.push_back(_random_bias());
      }
      float _random_bias()
      {
	return _random_bias_f();
      }
      
      void _del_neuron()
      {
	if (_neurons.size() <= 1)
	  return;
	size_t n = misc::rand(_neurons.size());
	_neurons.erase(_neurons.begin() + n);

	BOOST_FOREACH(direct::Connection& c, _conns)
	  {
	    _remove_ref_neuron(c.src, n, true);
	    _remove_ref_neuron(c.target, n, false);
	  }
      }

      void _remove_ref_neuron(size_t& r, size_t n, bool src)
      {
	if (r == n)
	  if (src)
	    r = _rand_src();
	  else
	    r = _rand_tgt();
	else if (r > n)
	  r = r - 1;			
      }

      // order : input / output / hidden
      // src cannot be an output
      size_t _rand_src()
      {
	size_t k = misc::rand(_neurons.size() - Params::direct::nb_outputs);
	if (k >= Params::direct::nb_inputs)
	  k += Params::direct::nb_outputs;
	assert(k < _neurons.size());
	return k;
      }
      size_t _rand_tgt()
      {
	size_t k = misc::rand(_neurons.size() - Params::direct::nb_inputs);
	k += Params::direct::nb_inputs;
	assert(k < _neurons.size());
	return k;
      }
      float _change_weight(float &w)
      {
	if (misc::rand<float>() < Params::direct::m_rate_weight)
	  w = _mutation_weight_f(w);	
	return w;
      }

      void _change_conn(direct::Connection& c)
      {
	if (misc::rand<float>() < Params::direct::m_rate_change_conn) {
	  if (misc::flip_coin()) {
	    c.src = _rand_src();
		}
	  else{
	    c.target = _rand_tgt();
		}
	}
	_change_weight(c.weight);
      }
    };

    // partial specialization for weight types
    namespace direct
    {
      //---  continous type ----
      template<typename Params>
      struct MutationWeight_f<continuous, Params>
      {
	float operator()(float w) 
	{
	  w += misc::gaussian_rand(0.0f, Params::direct::weight_sigma);	  
	  return misc::put_in_range(w, -Params::direct::max_weight, Params::direct::max_weight);
	}
      };

      template<typename Params>
      struct RandomWeight_f<continuous, Params>
      {
	float operator()() 
	{
	  return misc::rand<float>(Params::direct::max_weight * 2.0f)
	    - Params::direct::max_weight;   
	}
      };

      template<typename Params>
      struct RandomBias_f<continuous, Params>
      {
	float operator()() 
	{
	  return misc::rand<float>(Params::direct::max_bias * 2.0f)
	    - Params::direct::max_bias;   
	}
      };
      
      // ---  sampled type ---
      //---  continous type ----
      template<typename Params>
      struct MutationWeight_f<sampled, Params>
      {
	float operator()(float w) 
	{
	  // find index
	  int index = -1;
	  for (size_t i = 0; i < Params::direct::weights_size(); ++i)
	    if (Params::direct::weights(i) == w)
	      { 
		index = i; 
		break; 
	      }
	  assert(index != -1);
	  // move -1/+1
	  int m = misc::flip_coin() ? -1 : 1;
	  return Params::direct::weights(misc::put_in_range(m + index, 0, 
							    (int) Params::direct::weights_size() - 1));
	}
      };

      template<typename Params>
      struct RandomWeight_f<sampled, Params>
      {
	float operator()() 
	{
	  return Params::direct::weights(misc::rand(Params::direct::weights_size()));
	}
      };

      template<typename Params>
      struct RandomBias_f<sampled, Params>
      {
	float operator()() 
	{
	  return Params::direct::weights(misc::rand(Params::direct::weights_size()));
	}
      };
      
    }

  }
}
#endif
