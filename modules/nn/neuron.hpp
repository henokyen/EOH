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




#ifndef NN_NEURON_HPP
#define NN_NEURON_HPP

#include <boost/graph/properties.hpp>
#include <valarray>

#include "io_trait.hpp"

namespace nn
{

  // generic neuron
  // Pot : potential functor (see pf.hpp)
  // Act : activation functor (see af.hpp)
  // IO : type of coupling between "neurons" (float or std::pair<float, float>)
  template<typename Pot, typename Act, typename IO = float>
  class Neuron
  {
  public:
    typedef IO io_t;
    typedef IO weight_t;
    typedef Pot pf_t;
    typedef Act af_t;
    static io_t zero() { return io_trait<IO>::zero(); }
    Neuron() : 
      _current_output(zero()), 
      _next_output(zero()), 
      _fixed(false), 
      _in(-1), 
      _out(-1) 
    {}
    bool get_fixed() const { return _fixed; }
    void set_fixed(bool b = true) { _fixed = b; }
    io_t activate()
    {
      if (!_fixed)
	_next_output = _af(_pf(_weights, _inputs));
      return _next_output;
    }
 
    void init() { _inputs = zero(); _current_output = zero(); _next_output = zero(); }
    void set_input(unsigned i, const io_t& in) { assert(i < _inputs.size()); _inputs[i] = in; }
    void set_weight(unsigned i, const weight_t& w) { assert(i < _weights.size()); _weights[i] = w; }
    void set_afparams(const std::vector<float>& params) { _af.set_params(params); }
    void set_pfparams(const std::vector<float>& params) { _pf.set_params(params); }
    const std::vector<float>& get_afparams() const { return _af.get_params(); }
    const std::vector<float>& get_pfparams() const { return _pf.get_params(); }

    void step() { _current_output = _next_output; }
    void set_in_degree(unsigned k){ _weights.resize(k); _inputs.resize(k); _inputs = zero();}
    unsigned get_in_degree() const { return _weights.size(); }

    // for input neurons
    void set_current_output(const io_t& v) { _current_output = v; }
    void set_next_output(const io_t& v) { _next_output = v; }
      
    // standard output
    const io_t& get_current_output() const { return _current_output; }

    // next output
    const io_t& get_next_output() const { return _next_output; }

    // i/o
    int get_in() const { return _in; }
    void set_in(int i) { _in = i; }
    int get_out() const { return _out; }
    void set_out(int o) { _out = o; }
    bool is_input() const { return _in != -1; }
    bool is_output() const { return _out != -1; }
    
    const Pot& get_pf() const { return _pf; }
    Pot& get_pf() { return _pf; }


    const Act& get_af() const { return _af; }
    Act& get_af() { return _af; }

    // public for dot reading
    std::string _label;
    std::string _id;
    // for graph algorithms
    boost::default_color_type _color;
    int _index;
  protected:
    // activation functor
    Act _af;
    // potential functor
    Pot _pf;
    // outputs
    io_t _current_output;
    io_t _next_output;
    // cache
    std::valarray<weight_t> _weights;
    std::valarray<io_t> _inputs;
    // fixed = current_output is constant
    bool _fixed;
    // -1 if not an input of the nn
    int _in;
    // -1 if not an output of the nn
    int _out;
  };    
}
#endif
