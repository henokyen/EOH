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




#ifndef PHEN_HYPER_NN_HPP
#define PHEN_HYPER_NN_HPP

#include <map>
#include <sferes/phen/indiv.hpp>
#include <modules/nn/nn.hpp>
#include <modules/nn/io_trait.hpp>

#include "gen_hyper_nn.hpp"

namespace sferes
{
  namespace phen
  {
    namespace hyper_nn
    {
      template <typename T, typename Params>
      struct ConnectionThreshold
      {
	float operator()(const std::vector<float>& outf)
	{
	  if (outf[0] > Params::hyper_nn::conn_threshold)
	    return true;
	  return false;
	}
      };
      template <typename T, typename Params>
      struct ExtractConnection
      {
	float operator()(const std::vector<float>& outf)
	{
	  return outf[1] * Params::hyper_nn::max_weight;
	}
      };
      template <typename Params>
      struct ConnectionThreshold<std::pair<float, float>, Params>
      {
	bool operator()(const std::vector<float>& outf)
	{
	  if (fabs(outf[0]) > Params::hyper_nn::conn_threshold)
	    return true;
	  return false;
	}
      };
      template <typename Params>
      struct ExtractConnection<std::pair<float, float>, Params>
      {
	std::pair<float, float> operator()(const std::vector<float>& outf)
	{
	  return std::make_pair(outf[1] * Params::hyper_nn::max_weight, 
				outf[2] * Params::hyper_nn::max_weight);
	}
      };
    }

    // hyperneat-inspired phenotype, based on a cppn
    SFERES_INDIV(HyperNn, Indiv)
      {
      public:
	typedef typename Params::hyper_nn::neuron_t neuron_t;
	typedef typename Params::hyper_nn::connection_t connection_t;
	typedef typename nn::NN<neuron_t, connection_t> nn_t;

	void develop()
	{
	  this->gen().init();
	  size_t d = this->gen().get_depth();
	  assert(this->gen().get_nb_inputs() == 1 + 2 + 2);
	  assert(this->gen().get_nb_outputs() == 
		 1
		 + nn::io_trait<typename neuron_t::io_t>::size());

	  // all neurons are either outputs or outputs
	  _nn.set_nb_inputs(Params::hyper_nn::nb_inputs);
	  _nn.set_nb_outputs(Params::hyper_nn::nb_outputs);

	   BGL_FORALL_VERTICES_T(v, _nn.get_graph(), typename nn_t::graph_t)
	    _all_neurons.push_back(v);
	  assert(_all_neurons.size() == Params::hyper_nn::substrate_size() / 2);

	//   // set afparams & pfparams using the cppn
// 	  for (size_t i = 0; 
//  	       i < Params::hyper_nn::substrate_size(); 
// 	       i += 2)
// 	    {
// 	      std::vector<float> in(5);
// 	      in[0] = 1.0f;
// 	      in[1] = Params::hyper_nn::substrate(i);
// 	      in[2] = Params::hyper_nn::substrate(i + 1);
// 	      in[3] = -1; // ??
// 	      in[4] = -1; // ??
// 	      this->gen().init();
// 	      for (size_t k = 0; k < d; ++k)
// 		this->gen().step(in);
// 	      std::vector<float> afparams, pfparams;
// 	      size_t k = 0, s =  nn::io_trait<typename neuron_t::io_t>::size();
// 	      for (k = s; k < s +  neuron_t::pf_t::nb_params; ++k)
// 		pfparams.push_back(this->gen().get_outf(k));
// 	      for (; k < s +  neuron_t::af_t::nb_params + neuron_t::pf_t::nb_params; ++k)
// 		afparams.push_back(this->gen().get_outf(k));
// 	      this->_nn.get_neuron_by_vertex(_all_neurons[i / 2 ]).set_afparams(afparams);
// 	      this->_nn.get_neuron_by_vertex(_all_neurons[i / 2 ]).set_pfparams(pfparams);
// 	    }

	 
	  // create connections
	  for (size_t i = 0; i <  Params::hyper_nn::substrate_size(); i += 2)
	    for (size_t j = 0; j <  Params::hyper_nn::substrate_size(); j += 2)
	      if (!_nn.is_input(_all_neurons[j / 2 ]))
		{
		  std::vector<float> in(5);
		  this->gen().init();
		  in[0] = 1.0f;
		  in[1] = Params::hyper_nn::substrate(i);
		  in[2] = Params::hyper_nn::substrate(i + 1);
		  in[3] = Params::hyper_nn::substrate(j);
		  in[4] = Params::hyper_nn::substrate(j + 1);
		  for (size_t k = 0; k < d; ++k)
		    this->gen().step(in);
		  if (_connection_threshold(this->gen().get_outf()))
		    _nn.add_connection(_all_neurons[i / 2 ], 
				       _all_neurons[j / 2],
				       _extract_connection(this->gen().get_outf()));
		}
	}
	void show(std::ostream& os) 
	{
	  _nn.write(os); 

	  std::ofstream ofs("/tmp/nn.svg");
	  ofs << "<svg width=\"100px\" height=\"100px\" viewbox=\"0 0 100 100\">";
	  for (size_t i = 0; i <  Params::hyper_nn::substrate_size(); i += 2)
	    {
	        float x = Params::hyper_nn::substrate(i);
		float y = Params::hyper_nn::substrate(i + 1);
		ofs << "<circle cx=\"" << x * 100
		    <<"\" cy=\""<<y * 100
		    <<"\" r=\"2\" fill=\"blue\" />"<<std::endl;
	    }
	  typedef typename nn_t::graph_t graph_t;      
	  typedef typename nn_t::vertex_desc_t v_d_t;
	  const graph_t& g = this->nn().get_graph();
	  int nb_conns = 0;
	  for (size_t i = 0; i <  Params::hyper_nn::substrate_size(); i += 2)
	    for (size_t j = 0; j <  Params::hyper_nn::substrate_size(); j += 2)
	      {
		v_d_t vi = this->_all_neurons[i / 2];
		v_d_t vj = this->_all_neurons[j / 2];
		BGL_FORALL_ADJ_T(vi, u, g, graph_t) 
		  if (vj == u)
		    {
		      float x1 = Params::hyper_nn::substrate(i) * 100;// + misc::rand<float>() * 2 - 1;
		      float y1 = Params::hyper_nn::substrate(i + 1) * 100;// + misc::rand<float>() * 2 - 1;
		      float x2 = Params::hyper_nn::substrate(j) * 100;// + misc::rand<float>() * 2 - 1;
		      float y2 = Params::hyper_nn::substrate(j + 1) * 100;// + misc::rand<float>() * 2 - 1;
		      ofs<<"<line x1=\"" << x1 << "\" y1=\"" << y1 <<"\" "
			 << "x2=\""<<x2<<"\" y2=\""<<y2
			 <<"\" stroke=\"black\" stroke-width=\"0.2\" opacity=\"0.25\"  />"
			 <<std::endl;
		      ++nb_conns;
		    }
	      }
	  ofs << "</svg>";
	  assert(nb_conns == _nn.get_nb_connections());
	}
	nn_t& nn() { return _nn; }
	const nn_t& nn() const { return _nn; }
	const std::vector<typename nn_t::vertex_desc_t>& 
	  all_neurons() const { return _all_neurons; }
      protected:	
	nn_t _nn;
	std::vector<typename nn_t::vertex_desc_t> _all_neurons;
	hyper_nn::ConnectionThreshold<typename neuron_t::io_t, Params> 
	  _connection_threshold;
	hyper_nn::ExtractConnection<typename neuron_t::io_t, Params> 
	  _extract_connection;
      };
  }
}


#endif
