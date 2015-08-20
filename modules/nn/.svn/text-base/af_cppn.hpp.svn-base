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




#ifndef NN_CPPN_HPP_
#define NN_CPPN_HPP_

// classic activation functions
namespace nn
{
  namespace cppn
  {
    enum func_e { sigmoid = 0, sinus = 1, gaussian = 2 };
  }

  //  Compositional Pattern Producing Networks
  struct AfCppn
  {
    void set_params(const std::vector<float>& v) 
    {
      assert(v.size() == 3 || v.empty());
      if (v.empty())
	{
	  _type = -1;
	  return;
	}
      _params = v;
      _type = (int) (_params[0] * nb_funcs);
      if (_type >= nb_funcs)
	_type = nb_funcs - 1;
    }
    const std::vector<float>& get_params() const { return _params; }
    float operator()(float p) const 
    {
      if (_params.empty())
	return p;
      switch (_type)
	{
	case cppn::sigmoid:
	  return tanh(p * _params[1] * 10);
	case cppn::sinus:
	  return sin(p /(_params[1] / 5.0f));
	case cppn::gaussian:
	  return exp(-powf(p, 2));	  
	default:
	  assert(0);
	}
      return 0;
    }
    static const size_t nb_params = 3;
    static const size_t nb_funcs = 3;

  protected:
    std::vector<float> _params;
    int _type;
  };


}

#endif
