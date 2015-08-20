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




#ifndef ARCH_FIXED_NN_HPP_
#define ARCH_FIXED_NN_HPP_

#include <tbb/spin_mutex.h>
#include <sferes/ctrl/arch.hpp>
#include <modules/nn/nn.hpp>

namespace sferes
{
  typedef tbb::spin_mutex nn_mutex_t;
  nn_mutex_t nn_mutex; //TODO : should be in a .cpp ...

  namespace ctrl
  {
    // Fixed topology
    SFERES_ARCH(ArchFixedNN, Arch)
    {
    public:
      ArchFixedNN() : 
	SFERES_PARENT(ArchFixedNN, Arch)(Params::nn::nb_inputs, Params::nn::nb_outputs),
	_nn(_get_nn())
	{
	  _nn.init();
	}
      void set_weights(const std::vector<float>& params)
      {
	_nn.set_weights(params);
	_nn.init();
      }
      void refresh()
      {
	assert(_nn.get_nb_inputs() == this->_inputs.size());
	assert(_nn.get_nb_outputs() == this->_outputs.size());
	_nn.step(this->_inputs);
	for (size_t i = 0; i < this->_outputs.size(); ++i)
 	  this->_outputs[i] = _nn.get_outf(i);	
	
      }
      typedef nn::NN<typename Params::nn::potential_t, typename Params::nn::activation_t> nn_t;
    protected:
      nn_t _nn;

      static const nn_t& _get_nn()
      {
	tbb::spin_mutex::scoped_lock lock(nn_mutex);
	static bool loaded = false;
	static nn_t nn;
	if (!loaded)
	  {
	    nn.set_nb_inputs(Params::nn::nb_inputs);
	    nn.set_nb_outputs(Params::nn::nb_outputs);
	    nn.load(Params::nn::dot_file());
	    loaded = true;
	  }
	return nn;
      }
    };
  }
}
#endif
