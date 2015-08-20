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




#ifndef NAND_HPP
#define NAND_HPP

#include <vector>
#include <list>
#include <boost/foreach.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/list.hpp>
#include <sferes/stc.hpp> 
#include <sferes/misc.hpp>
#include <modules/nn/nn.hpp>

namespace sferes
{
  namespace nand
  {
    struct Gate
    {
      Gate() {}
      Gate(int s1, int s2) : 
	src1(s1), src2(s2){}
      int src1, src2, tgt;
      template<typename A>
      void serialize(A& a, unsigned v)
      {
	a & BOOST_SERIALIZATION_NVP(src1);
	a & BOOST_SERIALIZATION_NVP(src2);
      }
    };
  }
  
  namespace gen
  {
    
    SFERES_CLASS(Nand)
    {
    public:
      void random()
      {
	size_t nb_gates =
	  misc::rand(Params::nand::min_nb_gates, Params::nand::max_nb_gates) 
	  + Params::nn::nb_inputs 
	  + Params::nn::nb_outputs;

	// out
	for (size_t i = 0; i < Params::nn::nb_outputs; ++i)
	  this->_gates.push_back(nand::Gate(misc::rand(Params::nn::nb_outputs, nb_gates),
					    misc::rand(Params::nn::nb_outputs, nb_gates)));

	// in
	for (size_t i = 0; i < Params::nn::nb_inputs; ++i)
	  this->_gates.push_back(nand::Gate(-i - 1, -i - 1));


	// hidden
	size_t h = Params::nn::nb_inputs + Params::nn::nb_outputs;
	for (size_t i = h; i < nb_gates; ++i)
	  this->_gates.push_back(nand::Gate(misc::rand(Params::nn::nb_outputs, nb_gates),
					    misc::rand(Params::nn::nb_outputs, nb_gates)));

	assert(nb_gates == this->_gates.size());
	assert(this->_gates.size());
      };
      void cross(const Nand& o, Nand& c1, Nand& c2)  
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
      void mutate()
      {
	size_t nb_gates = _gates.size();
	assert(nb_gates);
	static const size_t sh = Params::nn::nb_outputs + Params::nn::nb_inputs;
// 	if (misc::rand<float>() < Params::nand::m_rate_add_gate)
// 	  _gates.push_back(nand::Gate(misc::rand((int) sh, nb_gates),
// 				      misc::rand((int) sh, nb_gates)));

// 	if (misc::rand<float>() < Params::nand::m_rate_del_gate && _gates.size() > sh)
// 	  {
// 	    size_t k = misc::rand(0, nb_gates);
// 	    BOOST_FOREACH(nand::Gate& g, _gates)
// 	      {
// 		if (g.src1 >= k)
// 		  g.src1 = std::max(g.src1 - 1, (int)sh);
// 		if (g.src2 >= k)
// 		  g.src2 = std::max(g.src2 - 1, (int)sh);
// 	      }
// 	    _gates.erase(_it(_gates, k));
// 	  }

	nb_gates = _gates.size();
	BOOST_FOREACH(nand::Gate& g, _gates)
	  if (g.src1 > 0 && g.src2 > 0 && misc::rand<float>() < Params::nand::m_rate_change_gate)
	    {
	      if (misc::flip_coin())
		g.src1 = misc::rand(Params::nn::nb_outputs, nb_gates);
	      else
		g.src2 = misc::rand(Params::nn::nb_outputs, nb_gates);
	    }
      }

      const std::list<nand::Gate>& gates() const { return _gates; }
      template<typename A>
	void serialize(A& a, unsigned v)
      {
	a & BOOST_SERIALIZATION_NVP(_gates);
      }
    protected:
      template<typename L>
	typename L::iterator _it(L& l, size_t k)
      {
	typename L::iterator it = l.begin();
	for (size_t i = 0; i < k; ++i)
	  ++it;
      return it;
    }
      std::list<nand::Gate> _gates;
    };
  }

  namespace phen
  {
    SFERES_INDIV(Nand, Indiv)
    {
    public:
      void develop()
      {
	_nn = nn_t();//fresh new graph
	n_map_t m;
	_nn.set_nb_inputs(Params::nn::nb_inputs);
	_nn.set_nb_outputs(Params::nn::nb_outputs);

	// create nodes
	for (size_t i = 0; i < _nn.get_nb_outputs(); ++i)
	  m.push_back(_nn.get_output(i));

	for (size_t i = 0; i < _nn.get_nb_inputs(); ++i)
	  m.push_back(_nn.get_input(i));

	const std::list<nand::Gate>& gates = this->gen().gates();
// 	BOOST_FOREACH(nand::Gate g, gates)
// 	  std::cout<<"==> "<<g.src1<<" "<<g.src2<<std::endl;


	for (unsigned i = _nn.get_nb_inputs() + _nn.get_nb_outputs();
	     i < gates.size(); ++i)
	  m.push_back(_nn.add_neuron("nand"));
	
	assert(m.size() == gates.size());
	// conns
	int i = 0;
	BOOST_FOREACH(nand::Gate g, gates)
	  {
	    if (g.src1 > 0 && g.src2 > 0) // in || out : create nothing
	      {
		assert(i < Params::nn::nb_outputs || i >= Params::nn::nb_outputs + Params::nn::nb_outputs );
		assert(g.src2 >= Params::nn::nb_outputs);
		assert(g.src1 >= Params::nn::nb_outputs);
		assert(g.src1 < gates.size());
		assert(g.src2 < gates.size());
		
		_nn.add_connection(m[g.src1], m[i], 1.0f);
		_nn.add_connection(m[g.src2], m[i], 1.0f);
	      }
	    ++i;
	  }
	_nn.init();
	//	_nn.write(std::cout);
      }


      void show(std::ostream& os) { _nn.write(os);  }
      typedef nn::NN<typename nn::PfNand, nn::AfDirect> nn_t;
      nn_t& nn() { return _nn; }
    protected:
      typedef std::vector<nn_t::vertex_desc_t> n_map_t;
      nn_t _nn;
    };
  }
}

#endif
