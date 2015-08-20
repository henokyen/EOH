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

#ifndef PHEN_HNN_FIXED_HPP
#define PHEN_HNN_FIXED_HPP

#include <map>
#include <queue>
#include <sferes/phen/indiv.hpp>
#include <modules/nn2/nn.hpp>

#include <modules/nn2/params.hpp>
#include "gen_hnn_3d.hpp"


namespace sferes
{
  namespace phen
  {
    // hyperneat-inspired phenotype, based on a cppn
    SFERES_INDIV(HnnFixed, Indiv)
    {
    public:
      typedef Gen gen_t;
      typedef typename Params::hnn::neuron_t neuron_t;
      typedef typename Params::hnn::connection_t connection_t;
      typedef typename nn::NN<neuron_t, connection_t> nn_t;
      typedef typename nn_t::vertex_desc_t v_d_t;
      typedef typename std::vector<v_d_t> layer_t;
      HnnFixed() : _developed(false) {}
      void develop()
      {
	develop_simple();
      }
      void develop_simple()
      {
	if (_developed)
	  return;
	_developed = true;
	this->gen().init();

	static const size_t nb_inputs = Params::hnn::layer_dim(0)
	  * Params::hnn::layer_dim(1)
	  * Params::hnn::layer_dim(2);
	static const size_t last_layer = Params::hnn::nb_layers - 1;
	static const size_t nb_outputs = Params::hnn::layer_dim(last_layer)
	  * Params::hnn::layer_dim(last_layer + 1)
	  * Params::hnn::layer_dim(last_layer + 2);
	
	// create neurons
	_nn = nn_t();
	_nn.set_nb_inputs(nb_inputs);
	_layers.resize(Params::hnn::nb_layers);
	_layers[0] = _nn.get_inputs();
	assert(this->gen().substrate().size() == Params::hnn::nb_layers);
	assert(this->gen().substrate().size() == Params::hnn::layer_dim_size() / 3);

	for (size_t i = 1; i < Params::hnn::nb_layers - 1; ++i)
	  for (size_t n = 0; n < this->gen().substrate()[i].size(); ++n)
	    _layers[i].push_back(_nn.add_neuron("h"));
	_nn.set_nb_outputs(nb_outputs);
	_layers[last_layer] = _nn.get_outputs();
	std::cout << " ... develop ... " << std::endl;

	// create connections
	// 1. isolate useful neurons
	std::vector<v_d_t> bullets_inputs;
	std::vector<v_d_t> ship_inputs;
	size_t z = 0;
	for (size_t i = 0; i <  Params::hnn::layer_dim(0); ++i) // 2
	  for (size_t j = 0;  j < Params::hnn::layer_dim(1); ++j) // 21
	    for (size_t k = 0; k <  Params::hnn::layer_dim(2); ++k, ++z) // 16
	      {
		if (i == 0 && j >= 16) // bullets 17
		  bullets_inputs.push_back(_layers[0][z]);
		if (i == 1 && j == 20) // ship // 20
		  ship_inputs.push_back(_layers[0][z]);
	      }
	for (size_t i = 0; i < bullets_inputs.size(); ++i)
	  {
	    _nn.add_connection(bullets_inputs[i], _layers[1][i % 16], 1.0);
	  }
	for (size_t i = 0; i < ship_inputs.size(); ++i)
	  _nn.add_connection(ship_inputs[i], _layers[1][i], 1.0);

	for (size_t i = 0; i < ship_inputs.size(); ++i)
	  if (i > ship_inputs.size() / 2)
	    _nn.add_connection(_layers[1][i], _layers[2][0], 1.4);
	  else
	    _nn.add_connection(_layers[1][i], _layers[2][1], 2.0);

	// bias parameters
	typename neuron_t::af_t::params_t afparams;
	static const boost::tuple<float, float, float> v =
	  boost::make_tuple(10, 10, 10);
	for (size_t i = 0; i < Params::hnn::nb_layers; ++i)
	  for (size_t n = 0; n < this->gen().substrate()[i].size(); ++n)
            {
              afparams[0] = 0.5;
              _nn.get_graph()[_layers[i][n]].set_afparams(afparams);;
            }
	afparams[0] = 0;
	_nn.get_graph()[_layers[2][1]].set_afparams(afparams);;

	save_svg("fixed_nn.svg");
	std::cout<<"nn saved"<<std::endl;
      }


      void develop_complex()
      {
	if (_developed)
	  return;
	_developed = true;
	this->gen().init();

	static const size_t nb_inputs = Params::hnn::layer_dim(0)
	  * Params::hnn::layer_dim(1)
	  * Params::hnn::layer_dim(2);
	static const size_t last_layer = Params::hnn::nb_layers - 1;
	static const size_t nb_outputs = Params::hnn::layer_dim(last_layer)
	  * Params::hnn::layer_dim(last_layer + 1)
	  * Params::hnn::layer_dim(last_layer + 2);
	
	// create neurons
	_nn = nn_t();
	_nn.set_nb_inputs(nb_inputs);
	_layers.resize(Params::hnn::nb_layers);
	_layers[0] = _nn.get_inputs();
	assert(this->gen().substrate().size() == Params::hnn::nb_layers);
	assert(this->gen().substrate().size() == Params::hnn::layer_dim_size() / 3);

	for (size_t i = 1; i < Params::hnn::nb_layers - 1; ++i)
	  for (size_t n = 0; n < this->gen().substrate()[i].size(); ++n)
	    _layers[i].push_back(_nn.add_neuron("h"));
	_nn.set_nb_outputs(nb_outputs);
	_layers[last_layer] = _nn.get_outputs();
	std::cout << " ... develop ... " << std::endl;

	// create connections
	// 1. isolate useful neurons
	std::vector<v_d_t> bullets_inputs;
	std::vector<v_d_t> ship_inputs;
	size_t z = 0;
	for (size_t i = 0; i <  Params::hnn::layer_dim(0); ++i) // 2
	  for (size_t j = 0;  j < Params::hnn::layer_dim(1); ++j) // 21
	    for (size_t k = 0; k <  Params::hnn::layer_dim(2); ++k, ++z) // 16
	      {
		if (i == 0 && j >= 0) // bullets
		  bullets_inputs.push_back(_layers[0][z]);
		if (i == 1 && j == 20) // ship // 20
		  ship_inputs.push_back(_layers[0][z]);
	      }
	for (size_t i = 0; i < bullets_inputs.size(); ++i)
	  {
	    _nn.add_connection(bullets_inputs[i], _layers[1][i % 16], i/(32.0));
	   
	    //	     if (i > 0)
	    // _nn.add_connection(bullets_inputs[i], _layers[1][(i - 1)% 16], 1.0);
	    //	     if (i % 16 < 21)
	    //   _nn.add_connection(bullets_inputs[i], _layers[1][(i + 1) % 16], 1.0);
	  }
	for (size_t i = 0; i < ship_inputs.size(); ++i)
	  _nn.add_connection(ship_inputs[i], _layers[1][i], 1.0);

	for (size_t i = 0; i < ship_inputs.size(); ++i)
	  if (i > ship_inputs.size() / 2)
	    _nn.add_connection(_layers[1][i], _layers[2][0], 2.0);
	  else
	    _nn.add_connection(_layers[1][i], _layers[2][1], 2.0);

	// bias parameters
	typename neuron_t::af_t::params_t afparams;
	static const boost::tuple<float, float, float> v =
	  boost::make_tuple(10, 10, 10);
	for (size_t i = 0; i < Params::hnn::nb_layers; ++i)
	  for (size_t n = 0; n < this->gen().substrate()[i].size(); ++n)
            {
              afparams[0] = 0.5;
              _nn.get_graph()[_layers[i][n]].set_afparams(afparams);;
            }
	afparams[0] = 0;
	_nn.get_graph()[_layers[2][1]].set_afparams(afparams);;

	save_svg("fixed_nn.svg");
	std::cout<<"nn saved"<<std::endl;
      }
      nn_t& nn() { return _nn; }
      const nn_t& nn() const { return _nn; }
      float compute_connection_cost() const
      {
	typedef std::map<typename nn_t::vertex_desc_t, 
	  boost::tuple<float, float, float> > nmap_t;
	nmap_t nmap;
	for (size_t i = 0; i < Params::hnn::nb_layers; ++i)
	  for (size_t n = 0; n < this->gen().substrate()[i].size(); ++n)
	    {
	      float z = this->gen().substrate()[i][n].get<0>();
	      float x = this->gen().substrate()[i][n].get<1>();
	      float y = this->gen().substrate()[i][n].get<2>();
	      nmap[_layers[i][n]] = boost::make_tuple(x, y, z);
	    }
	float dist = 0.0f;
	BGL_FORALL_EDGES_T(e, _nn.get_graph(), typename nn_t::graph_t)
	  {
	    typename nn_t::vertex_desc_t src = 
	      boost::source(e, _nn.get_graph());
	    typename nn_t::vertex_desc_t tgt = 
	      boost::target(e, _nn.get_graph());
	    dist += _dist2(nmap[src], nmap[tgt]);
	  }
	return dist;
      }
      void save_svg(const std::string& fname) const
      {
	std::ofstream ofs(fname.c_str());
	ofs << "<?xml version=\"1.0\" standalone=\"no\"?>" << std::endl;
	ofs << "<svg width=\"3000\" height=\"1100\" version=\"1.1\""
	    << " xmlns=\"http://www.w3.org/2000/svg\">"
	    << "<g transform=\"translate(1500,200)\">" // WARNING translation
	    << std::endl;
	static const float scale = 100;
	static const float layer_shift = 4 * scale;
	static const float z_shift = 10 * scale;
	typedef std::map<typename nn_t::vertex_desc_t, 
	  std::pair<float, float> > nmap_t;
	nmap_t nmap;
	for (size_t i = 0; i < Params::hnn::nb_layers; ++i)
	  for (size_t n = 0; n < this->gen().substrate()[i].size(); ++n)
	    {
	      float m1 = this->gen().substrate()[i][n].get<0>();
	      float m2 = this->gen().substrate()[i][n].get<1>();
	      float m3 = this->gen().substrate()[i][n].get<2>();
	      float x = m3 * scale + z_shift * m1;// Inversion to match the screen
	      float y = m2 * scale + layer_shift * i;	      
	      nmap[_layers[i][n]] = std::make_pair(x, y);
	    }
	
	BGL_FORALL_EDGES_T(e, _nn.get_graph(), typename nn_t::graph_t)
	  {
	    typename nn_t::vertex_desc_t src = 
	      boost::source(e, _nn.get_graph());
	    typename nn_t::vertex_desc_t tgt = 
	      boost::target(e, _nn.get_graph());
	    float x1 = nmap[src].first ;
	    float y1 = nmap[src].second;
	    float x2 = nmap[tgt].first;
	    float y2 = nmap[tgt].second;
	    double weight = _nn.get_graph()[e].get_weight();
	    ofs << "<line x1=\"" << x1 << "\" y1=\"" << y1 << "\" "
		<< "x2=\"" << x2 << "\" y2=\"" << y2 << "\""
		<< " style=\"stroke:"
		<< (weight > 0 ? "#00ff00" : "#ff0000")
		<< ";stroke-width:" << fabs(weight)
		<<";stroke-opacity:0.5"
		<< "\"/>"
		<< std::endl;
	  }
	BGL_FORALL_VERTICES_T(v, _nn.get_graph(), typename nn_t::graph_t)
	  {
	    float x = nmap[v].first;
	    float y = nmap[v].second;
	    float c = (int)(_nn.get_graph()[v].get_current_output() * 255);
	    ofs << "<circle cx=\"" << x << "\" cy=\"" << y
		<< "\" r=\"4\" stroke=\"black\" stroke-width=\"0.25\" fill=\""
		<< "rgb(" << c << "," << c << ", " << c << ")"
		<< "\"/>" << std::endl;
	  }
	ofs << "</g>" << std::endl;
	ofs << "</svg>" << std::endl;


      }
    protected:
      bool _developed;
      // squared distance
      float _dist2(const boost::tuple<float, float, float>& p1,
		   const boost::tuple<float, float, float>& p2) const
      {
	float x = p1.get<0>() - p2.get<0>();
	float y = p1.get<1>() - p2.get<1>();
	float z = p1.get<2>() - p2.get<2>();
	return (x * x + y * y + z * z);
      }
      nn_t _nn;
      std::vector<layer_t> _layers;
    };
  }
}
#endif
