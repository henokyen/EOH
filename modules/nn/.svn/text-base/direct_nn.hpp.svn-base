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




#ifndef DIRECT_NN_HPP_
#define DIRECT_NN_HPP_

#include <map>
#include <sferes/phen/indiv.hpp>
#include <modules/nn/nn.hpp>

#include <modules/nn/direct.hpp>
namespace sferes
{
  namespace phen
  {
    SFERES_INDIV(Direct, Indiv)
    {
    public:
      typedef nn::Neuron<typename Params::nn::potential_t, 
	typename Params::nn::activation_t> neuron_t;
      typedef nn::NN<neuron_t, nn::Connection> nn_t;

      void develop()
      {
	_nn = nn_t();
	_v.clear();;
	_nn.set_nb_inputs(this->gen().nb_inputs());
	_nn.set_nb_outputs(this->gen().nb_outputs());
	for (size_t i = 0; i < _nn.get_nb_inputs(); ++i)
	 _v.push_back(_nn.get_input(i));
	for (size_t i = 0; i < _nn.get_nb_outputs(); ++i)
	 _v.push_back(_nn.get_output(i));
	
	std::vector<float> pf_params, af_params(1);
	for (size_t i =_v.size(); i < this->gen().neurons().size(); ++i)
	  {
	    af_params[0] = this->gen().neurons()[i];
	   _v.push_back(_nn.add_neuron("n", pf_params, af_params));
	  }
	assert(_v.size() == this->gen().neurons().size());
	BOOST_FOREACH(gen::direct::Connection c, this->gen().conns())
	  {
	    assert(c.src <_v.size());
	    assert(c.target <_v.size());
	    _nn.add_connection(_v[c.src], _v[c.target], c.weight);
	  }
	
	_nn.init();
      }

      void show(std::ostream& os) { _nn.write(os);  }
      nn_t& nn() { return _nn; }
      const nn_t& nn() const { return _nn; }
    protected:
      typedef typename std::vector<typename nn_t::vertex_desc_t> n_vect_t;
      n_vect_t _v;
      nn_t _nn;
    };
  };
}

#endif
