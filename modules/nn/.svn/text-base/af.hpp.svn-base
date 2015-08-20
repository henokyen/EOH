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




#ifndef _NN_AF_HPP_
#define _NN_AF_HPP_

// classic activation functions
namespace nn
{
  // -1 to +1 sigmoid
  struct AfTanh
  {
    void set_params(const std::vector<float>& v) {}
    const std::vector<float>& get_params() const  { return _params; }
    float operator()(float p) const { return tanh(p); }
    static const size_t nb_params = 0;
  protected:
    std::vector<float> _params;
  };

  // copy input to output
  struct AfDirect
  {
    void set_params(const std::vector<float>& v) {}
    const std::vector<float>& get_params() const { return _params; }
    float operator()(float p) const { return p; }
    static const size_t nb_params = 0;
  protected:
    std::vector<float> _params;
  };

  // template version of afdirect
  template<typename T, int N = 0>
  struct AfDirectT
  {
    void set_params(const std::vector<float>& v) { assert(v.size() == nb_params); _params = v; }
    const std::vector<float>& get_params() const { return _params; }
    T operator()(T p) const { return p; }
    static const size_t nb_params = N;
  protected:
    std::vector<float> _params;    
  };

  // 0 to +1 sigmoid
  struct AfSigmoid
  {
    float _bias;
    AfSigmoid() : _bias(0) {}
    void set_params(const std::vector<float>& v) 
    { 
      if (v.size() == 0) 
	_bias = 0;
      else
	_bias = v[0];
      _params = v;
    }
    const std::vector<float>& get_params() const  { return _params; }
    float operator()(float p) const 
    {  
      return 1.0f / (1.0f + exp((-(p + _bias)) * 5));
    }
    static const size_t nb_params = 1;
  protected:
    std::vector<float> _params;
  };

  
  struct AfSemiTanh
  {
    void set_params(const std::vector<float>& v) {}
    const std::vector<float>& get_params() const { return _params; }
    float operator()(float p) const { return (p > 0 ? tanh(p) : 0);}
    static const size_t nb_params = 0;
  protected:
    std::vector<float> _params;
  };

  struct AfThreshold
  {
    static const float eps = 0.0f;
    static const float m   = 1.0f;
    void set_params(const std::vector<float>& v) {}
    const std::vector<float>& get_params() const { return _params; }
    float operator()(float a) const 
    {
      float res = 0.0f;
      if (a < eps) 
	res = 0.0f;
      else if (a >= eps && a < eps + 1.0f / m) 
	res = m * (a - eps);
      else
	res = 1.0f;
      return res;
    }
    static const size_t nb_params = 0;    
  protected:
    std::vector<float> _params;

  };


}

#endif
