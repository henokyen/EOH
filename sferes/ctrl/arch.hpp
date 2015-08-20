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




#ifndef ARCH_HPP
#define ARCH_HPP

#include <vector>
#include <cassert>
#include "sferes/stc.hpp"


// Example :
//     STC_CLASS_D(ArchSum, Arch)
//     {
//     public:

//       ArchSum(size_t nb_in, size_t nb_out) : 
// 	STC_PARENT(ArchSum, Arch)(nb_in, nb_out)
// 	{}
//       void refresh() 
//       {
// 	float sum = 0;
// 	BOOST_FOREACH(float v, this->_inputs)
// 	  sum += v;
// 	BOOST_FOREACH(float &v, this->_outputs)
// 	  v = sum;
//       }
//     protected:

//     };

namespace sferes
{
  namespace ctrl
  {
    enum special_e { end };
    
    SFERES_CLASS(Arch)
    {
    public:

      Arch(size_t nb_inputs, size_t nb_outputs) : 
	_inputs(nb_inputs), _outputs(nb_outputs), _i(0) {}

      float input(size_t i = 0) const { assert(i < _inputs.size()); return _inputs[i]; }
      float output(size_t i = 0) const { assert(i < _outputs.size()); return _outputs[i]; }
      Arch& operator<<(size_t i) { _i = i; return stc::exact(*this);}
      Arch& operator<<(double f) { this->_inputs[_i] =f; return stc::exact(*this); }
      Arch& operator<<(float f) { this->_inputs[_i] =f; return stc::exact(*this); }
      Arch& operator<<(special_e e) { _i =0; stc::exact(*this).refresh(); return stc::exact(*this); }

    protected:
      std::vector<float> _inputs;
      std::vector<float> _outputs;
      size_t _i;
    };

    SFERES_CLASS_D(ActDummy, Arch)
    {
    public:
      ActDummy() : SFERES_PARENT(ActDummy, Arch)(1, 1) {}
      void refresh(){ this->_outputs[0] = this->_inputs[0]; };
    protected:
    };
  }
}
#define SFERES_ARCH SFERES_CLASS_D
#define SFERES_ACT SFERES_CLASS_D

#endif
