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




#define BOOST_TEST_DYN_LINK 
#define BOOST_TEST_MODULE dnn

#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <boost/test/unit_test.hpp>
#include <boost/serialization/nvp.hpp>
#include <iostream>
#include <cmath>
#include <algorithm>

#include <sferes/fit/fitness.hpp>
#include "phen_dnn.hpp"

using namespace sferes;
using namespace sferes::gen::dnn;


template<typename T1, typename T2>
void check_list_equal(const T1& v1, const T2& v2)
{
  BOOST_CHECK_EQUAL(v1.size(), v2.size());
  typename T1::const_iterator it1 = v1.begin();
  typename T1::const_iterator it2 = v2.begin();
  for (; it1 != v1.end(); ++it1, ++it2)
    BOOST_CHECK(fabs(*it1 - *it2) < 1e-3);
}
 
template<typename NN>
void check_nn_equal(NN& nn1, NN& nn2)
{
  nn1.init();
  nn2.init();

  BOOST_CHECK_EQUAL(nn1.get_nb_inputs(), nn2.get_nb_inputs());
  BOOST_CHECK_EQUAL(nn1.get_nb_outputs(), nn2.get_nb_outputs());
  BOOST_CHECK_EQUAL(nn1.get_nb_neurons(), nn2.get_nb_neurons());
  BOOST_CHECK_EQUAL(nn1.get_nb_connections(), nn2.get_nb_connections());
//   nn1.write("/tmp/tmp1.dot");
//   nn2.write("/tmp/tmp2.dot");
//   std::ifstream ifs1("/tmp/tmp1.dot"), ifs2("/tmp/tmp2.dot");
//   while(!ifs1.eof() && !ifs2.eof())
//     {
//       //if (ifs1.get() != ifs2.get()) exit(1);
//       BOOST_CHECK_EQUAL((char)ifs1.get(), (char)ifs2.get());
//     }

  std::pair<typename NN::vertex_it_t, typename NN::vertex_it_t> vp1 = 
    boost::vertices(nn1.get_graph());
  std::pair<typename NN::vertex_it_t, typename NN::vertex_it_t> vp2 = 
    boost::vertices(nn2.get_graph());
  while (vp1.first != vp1.second)
    {
      BOOST_CHECK_EQUAL(nn1.get_graph()[*vp1.first].get_in_degree(),
			nn2.get_graph()[*vp2.first].get_in_degree());
      check_list_equal(nn1.get_graph()[*vp1.first].get_afparams(),
		       nn2.get_graph()[*vp1.first].get_afparams());
      check_list_equal(nn1.get_graph()[*vp1.first].get_pfparams(),
		       nn2.get_graph()[*vp1.first].get_pfparams());
      ++vp1.first;
      ++vp2.first;
    }

}

 
struct Params
{
  struct dnn
  {
    static const weight_t weight_type	= continuous;
    static const size_t nb_inputs	= 4;
    static const size_t nb_outputs	= 1;
    static const size_t min_nb_neurons	= 4;
    static const size_t max_nb_neurons	= 5;
    static const size_t min_nb_conns	= 100;
    static const size_t max_nb_conns	= 101;
    static const float  max_weight	= 2.0f;
    static const float  max_bias	= 2.0f;

    static const float m_rate_add_conn	= 1.0f;
    static const float m_rate_del_conn	= 1.0f;
    static const float m_rate_change_conn = 1.0f;
    static const float m_rate_add_neuron  = 1.0f;
    static const float m_rate_del_neuron  = 1.0f;

    static const float weight_sigma	= 0.5f;
    static const float vect_sigma	= 0.5f;
    static const float m_rate_weight	= 1.0f;
    static const float m_rate_fparams  = 1.0f;
    //    static const float m_rate_afparams  = 1.0f;
    //   static const float m_rate_pfparams  = 1.0f;
    static const int io_param_evolving = 0;
    static const init_t init = random_topology;
  };
  struct nn
  {
    typedef ::nn::AfSigmoid activation_t;
    typedef ::nn::PfWSum potential_t;
  };
};

struct Params2
{
  struct dnn
  {
    static const weight_t weight_type	= sampled;
    SFERES_ARRAY(float, weights, -1.0f, 0.0f, 1.0f);
    SFERES_ARRAY(float, params, -1.0f, 0.0f, 1.0f);

    static const size_t nb_inputs	= 4;
    static const size_t nb_outputs	= 1;
    static const size_t min_nb_neurons	= 4;
    static const size_t max_nb_neurons	= 5;
    static const size_t min_nb_conns	= 100;
    static const size_t max_nb_conns	= 101;
    static const float  max_weight	= 2.0f;
    static const float  max_bias	= 2.0f;

    static const float m_rate_add_conn	= 1.0f;
    static const float m_rate_del_conn	= 1.0f;
    static const float m_rate_change_conn = 1.0f;
    static const float m_rate_add_neuron  = 1.0f;
    static const float m_rate_del_neuron  = 1.0f;
    static const float weight_sigma	= 0.5f;
    static const float vect_sigma	= 0.5f;
    static const float m_rate_weight	= 1.0f;
    static const float m_rate_fparams  = 1.0f;
    //    static const float m_rate_afparams  = 1.0f;
    //    static const float m_rate_pfparams  = 1.0f;
    static const int io_param_evolving = 0; 
    static const init_t init = random_topology;
  };
  struct nn
  {
    typedef ::nn::AfSigmoid activation_t;
    typedef ::nn::PfWSum potential_t;
  };
};
 
struct Params3
{
  struct dnn
  {
    static const weight_t weight_type	= bitstring;
    SFERES_ARRAY(float, weights, -1.0f, 0.0f, 1.0f);
    SFERES_ARRAY(float, params, -1.0f, 0.0f, 1.0f);

    static const size_t nb_inputs	= 4;
    static const size_t nb_outputs	= 1;
    static const size_t min_nb_neurons	= 4;
    static const size_t max_nb_neurons	= 5;
    static const size_t min_nb_conns	= 100;
    static const size_t max_nb_conns	= 101;
    static const float  max_weight	= 2.0f;
    static const float  max_bias	= 2.0f;
    static const size_t nb_bits		= 8;

    static const float m_rate_add_conn	= 1.0f;
    static const float m_rate_del_conn	= 1.0f;
    static const float m_rate_change_conn = 1.0f;
    static const float m_rate_add_neuron  = 1.0f;
    static const float m_rate_del_neuron  = 1.0f;

    static const float m_rate_weight	= 1.0f;
    static const float m_rate_bit_flip	= 1.0f;
    static const float m_rate_fparams  = 1.0f;
    //    static const float m_rate_afparams  = 1.0f;
    //    static const float m_rate_pfparams  = 1.0f;
    static const int io_param_evolving = 0; 
    static const init_t init = random_topology;
  };
  struct nn
  {
    typedef ::nn::AfSigmoid activation_t;
    typedef ::nn::PfWSum potential_t;
  };
};


BOOST_AUTO_TEST_CASE(direct_nn_serialize_sampled_bin)
{
  srand(time(0));
  typedef phen::Dnn<gen::Dnn<Params2>, fit::FitDummy<Params2>, Params2> phen_t;
  typedef boost::archive::binary_oarchive oa_t;
  typedef boost::archive::binary_iarchive ia_t;

  for (size_t i = 0; i < 10; ++i)
    {
      phen_t indiv[3];
      indiv[0].random();
      indiv[0].mutate();
      indiv[0].mutate();
      indiv[0].mutate();
      indiv[0].nn().init();
      
      std::ofstream ofs("/tmp/serialize_nn1.dot"); 
      indiv[0].nn().write(ofs);
      {
	std::ofstream ofs("/tmp/serialize_nn1.bin", std::ios::binary);
	oa_t oa(ofs); 
	oa & indiv[0];
      }
      //      exit(1);
      {
	std::ifstream ifs("/tmp/serialize_nn1.bin", std::ios::binary);
	std::cout<<"IAT start"<<std::endl;
	ia_t ia(ifs);
	ia & indiv[1];
	std::cout<<"IAT end"<<std::endl;
      }
      std::cout<<"reading binary ok"<<std::endl;
      indiv[2].nn() = indiv[0].nn();      
      check_nn_equal(indiv[0].nn(), indiv[1].nn());
      check_nn_equal(indiv[0].nn(), indiv[2].nn());    
    }
}



BOOST_AUTO_TEST_CASE(direct_gen)
{
  sferes::gen::Dnn<Params> gen1, gen2, gen3, gen4;
      
  gen1.random();
  gen2.random();

  gen1.cross(gen2, gen3, gen4);
  gen3.mutate();
  gen4.mutate();
  gen2.mutate();
}

BOOST_AUTO_TEST_CASE(direct_nn_cont)
{
  srand(time(0));
  typedef phen::Dnn<gen::Dnn<Params>, fit::FitDummy<Params>, Params> phen_t;
  phen_t i;
  i.random();
  i.develop();
  std::vector<float> in(4);
  std::fill(in.begin(), in.end(), 0);
  i.nn().step(in);
  BOOST_CHECK(i.nn().get_nb_inputs() == 4);
  BOOST_CHECK(i.nn().get_nb_outputs() == 1);
  BOOST_CHECK(i.nn().get_nb_neurons() > 5);
  BOOST_CHECK(i.nn().get_nb_connections() > 2);
  std::ofstream ofs("/tmp/nn.dot");
  i.nn().write(ofs);
}



BOOST_AUTO_TEST_CASE(direct_nn_serialize_sampled)
{
  srand(time(0));
  typedef phen::Dnn<gen::Dnn<Params2>, fit::FitDummy<Params2>, Params2> phen_t;
  typedef boost::archive::xml_oarchive oa_t;
  typedef boost::archive::xml_iarchive ia_t;

  for (size_t i = 0; i < 10; ++i)
    {
      phen_t indiv[3];
      indiv[0].random();
      indiv[0].mutate();
      indiv[0].mutate();
      indiv[0].mutate();
      indiv[0].nn().init();
      
      std::ofstream ofs("/tmp/serialize_nn1.dot");
      indiv[0].nn().write(ofs);
      {
	std::ofstream ofs("/tmp/serialize_nn1.xml");
	oa_t oa(ofs);
	oa << boost::serialization::make_nvp("nn", indiv[0]);
      }
      { 
 	std::ifstream ifs("/tmp/serialize_nn1.xml");
	std::cout<<"IAT start"<<std::endl;
	ia_t ia(ifs);
	std::cout<<"IAT end"<<std::endl; 
	ia >> boost::serialization::make_nvp("nn", indiv[1]);
      }
      indiv[2].nn() = indiv[0].nn();      
      check_nn_equal(indiv[0].nn(), indiv[1].nn());
      check_nn_equal(indiv[0].nn(), indiv[2].nn());    
    }
}




BOOST_AUTO_TEST_CASE(direct_nn_serialize_bitstring)
{
  srand(time(0));
  typedef phen::Dnn<gen::Dnn<Params3>, fit::FitDummy<Params3>, Params3> phen_t;
  typedef boost::archive::xml_oarchive oa_t;
  typedef boost::archive::xml_iarchive ia_t;

  for (size_t i = 0; i < 10; ++i)
    {
      phen_t indiv[2];
      indiv[0].random();
      indiv[0].mutate();
      indiv[0].mutate();
      indiv[0].mutate();
      indiv[0].nn().init();
      

      std::ofstream ofs("/tmp/serialize_nn1.dot");
      indiv[0].nn().write(ofs);
      {
	std::ofstream ofs("/tmp/serialize_nn1.xml");
	oa_t oa(ofs);
	oa << boost::serialization::make_nvp("nn", indiv[0]);
      }
      {
	std::ifstream ifs("/tmp/serialize_nn1.xml");
	ia_t ia(ifs);
	ia >> boost::serialization::make_nvp("nn", indiv[1]);
      }

      check_nn_equal(indiv[0].nn(), indiv[1].nn());
    }
}


BOOST_AUTO_TEST_CASE(direct_nn_sampled)
{
  BOOST_CHECK(true);
  srand(time(0));
  typedef phen::Dnn<gen::Dnn<Params2>, fit::FitDummy<Params2>, Params2> phen_t;
  phen_t i;
  i.random();
  i.develop();
  std::vector<float> in(4);
  std::fill(in.begin(), in.end(), 0);
  i.nn().step(in);
  BOOST_CHECK(i.nn().get_nb_inputs() == 4);
  BOOST_CHECK(i.nn().get_nb_outputs() == 1);
  BOOST_CHECK(i.nn().get_nb_neurons() > 5);
  BOOST_CHECK(i.nn().get_nb_connections() > 2);
  std::ofstream ofs("/tmp/nn.dot");
  i.nn().write(ofs);
  BGL_FORALL_EDGES(e, i.nn().get_graph(), phen_t::nn_t::graph_t)
    {
      float w = i.nn().get_graph()[e].get_weight();
      BOOST_CHECK(w == 0.0f || w == 1.0f || w == -1.0f);
    }
  i.mutate();
  i.mutate();
  i.develop();
  BOOST_CHECK(i.nn().get_nb_inputs() == 4);
  BOOST_CHECK(i.nn().get_nb_outputs() == 1);
  BOOST_CHECK(i.nn().get_nb_neurons() > 5);
  BOOST_CHECK(i.nn().get_nb_connections() > 2);
  BGL_FORALL_EDGES(e, i.nn().get_graph(), phen_t::nn_t::graph_t)
    {
      float w = i.nn().get_graph()[e].get_weight();
      BOOST_CHECK(w == 0.0f || w == 1.0f || w == -1.0f);
    }
}

BOOST_AUTO_TEST_CASE(direct_nn_bitstring)
{
  srand(time(0));
  typedef phen::Dnn<gen::Dnn<Params3>, fit::FitDummy<Params3>, Params3> phen_t;
  phen_t i;
  i.random();
  i.develop();
  std::vector<float> in(4);
  std::fill(in.begin(), in.end(), 0);
  i.nn().step(in);
  BOOST_CHECK(i.nn().get_nb_inputs() == 4);
  BOOST_CHECK(i.nn().get_nb_outputs() == 1);
  BOOST_CHECK(i.nn().get_nb_neurons() > 5);
  BOOST_CHECK(i.nn().get_nb_connections() > 2);
  std::ofstream ofs("/tmp/nn.dot");
  i.nn().write(ofs);
  BGL_FORALL_EDGES(e, i.nn().get_graph(), phen_t::nn_t::graph_t)
    {
      float w = i.nn().get_graph()[e].get_weight();
      BOOST_CHECK(w >= -Params3::dnn::max_weight && w <= Params3::dnn::max_weight);
    }
  i.mutate();
  i.mutate();
  i.develop();
  BOOST_CHECK(i.nn().get_nb_inputs() == 4);
  BOOST_CHECK(i.nn().get_nb_outputs() == 1);
  BOOST_CHECK(i.nn().get_nb_neurons() > 5);
  BOOST_CHECK(i.nn().get_nb_connections() > 2);
  BGL_FORALL_EDGES(e, i.nn().get_graph(), phen_t::nn_t::graph_t)
    {
      float w = i.nn().get_graph()[e].get_weight();
      BOOST_CHECK(w >= -Params3::dnn::max_weight && w <= Params3::dnn::max_weight);
    }
}


struct Params4
{
  struct dnn
  {
    static const weight_t weight_type	= continuous;
    static const float weight_sigma	= 0.5f;
    static const float vect_sigma	= 0.5f;

    static const size_t nb_inputs	= 4;
    static const size_t nb_outputs	= 1;
    static const size_t min_nb_neurons	= 4;
    static const size_t max_nb_neurons	= 5;
    static const size_t min_nb_conns	= 30;
    static const size_t max_nb_conns	= 50;
    static const float  max_weight	= 2.0f;
    static const float  max_bias	= 2.0f;
    static const size_t nb_bits		= 8;

    static const float m_rate_add_conn	= 1.0f;
    static const float m_rate_del_conn	= 1.0f;
    static const float m_rate_change_conn = 1.0f;
    static const float m_rate_add_neuron  = 1.0f;
    static const float m_rate_del_neuron  = 1.0f;

    static const float m_rate_weight	= 1.0f;
    static const float m_rate_fparams  = 1.0f;
    //    static const float m_rate_afparams  = 1.0f;
    //    static const float m_rate_pfparams  = 1.0f;
    static const int io_param_evolving = 0;     
    static const init_t init = random_topology;
  };
};

BOOST_AUTO_TEST_CASE(dnn_oscillator)
{
  using namespace nn;
  srand(time(0));
  typedef std::pair<float, float> io_t;
  typedef Neuron<PfIjspeert, AfDirectT<io_t>, io_t> n_t;  
  typedef ConnectionT<io_t> c_t;
  typedef phen::Dnn<gen::DnnT<n_t, c_t, Params4>, fit::FitDummy<Params4>, Params4> phen_t;
  phen_t i;
  i.random();
  i.mutate();  
  i.develop();
  std::vector<io_t> in(4);
  BOOST_FOREACH(io_t& io, in)
    io = std::make_pair(0.0f, 0.0f);
  i.nn().step(in);
  std::ofstream outf("/tmp/osc.dat");
  for (size_t k = 0; k < 6000; ++k)
    {
      i.nn().step(in);
      float out = i.nn().get_output_neuron(0).get_pf().x();
      assert(!isnan(out));
      assert(!isinf(out));
      outf<<out<<std::endl;
    }
  BOOST_CHECK(i.nn().get_nb_inputs() == 4);
  BOOST_CHECK(i.nn().get_nb_outputs() == 1);
  BOOST_CHECK(i.nn().get_nb_neurons() > 5);
  BOOST_CHECK(i.nn().get_nb_connections() > 2);
  std::ofstream ofs("/tmp/nn.dot");
  i.nn().write(ofs);

}

struct Params5
{
  struct dnn
  {
    static const weight_t weight_type	= continuous;
    static const float weight_sigma	= 0.5f;
    static const float vect_sigma	= 0.5f;

    static const size_t nb_inputs	= 0;
    static const size_t nb_outputs	= 1;
    static const size_t min_nb_neurons	= 4;
    static const size_t max_nb_neurons	= 5;
    static const size_t min_nb_conns	= 30;
    static const size_t max_nb_conns	= 50;
    static const float  max_weight	= 2.0f;
    static const float  max_bias	= 2.0f;
    static const size_t nb_bits		= 8;

    static const float m_rate_add_conn	= 1.0f;
    static const float m_rate_del_conn	= 1.0f;
    static const float m_rate_change_conn = 1.0f;
    static const float m_rate_add_neuron  = 1.0f;
    static const float m_rate_del_neuron  = 1.0f;

    static const float m_rate_weight	= 1.0f;
    static const float m_rate_fparams  = 1.0f;
    //    static const float m_rate_afparams  = 1.0f;
    //    static const float m_rate_pfparams  = 1.0f;
    static const int io_param_evolving = 0;     
    static const init_t init = random_topology;
  };
};




BOOST_AUTO_TEST_CASE(dnn_oscillator2)
{
  using namespace nn;
  srand(time(0));
  typedef std::pair<float, float> io_t;
  typedef Neuron<PfIjspeert, AfDirectT<io_t>, io_t > n_t;  
  typedef ConnectionT<io_t> c_t;
  typedef gen::DnnT<n_t, c_t, Params5> gen_t;
  typedef gen_t::nn_t nn_t;

  gen_t g;
  g.random();
  g.init();

  std::vector<io_t> in(0);
  nn_t nn = g;
  nn.init();

  check_nn_equal((nn_t&)g, nn);

  for (size_t i = 0; i < 100; ++i)
    {
      g.step(in); 
      nn.step(in);
      BOOST_CHECK_EQUAL(g.get_outf()[0].first,  nn.get_outf()[0].first);
      BOOST_CHECK_EQUAL(g.get_outf()[0].second, nn.get_outf()[0].second);
    }
}


BOOST_AUTO_TEST_CASE(direct_nn_serialize_oscillator)
{
  srand(time(0));
  using namespace nn;
  typedef std::pair<float, float> io_t;
  typedef Neuron<PfIjspeert, AfDirectT<io_t>, io_t > n_t;  
  typedef ConnectionT<io_t> c_t;
  typedef gen::DnnT<n_t, c_t, Params5> gen_t;
  typedef phen::Dnn<gen_t, fit::FitDummy<Params5>, Params5> phen_t;

  typedef boost::archive::xml_oarchive oa_t;
  typedef boost::archive::xml_iarchive ia_t;
  
  BOOST_CHECK(true);
  for (size_t i = 0; i < 10; ++i)
    {
      phen_t indiv[3];
      indiv[0].random();
      indiv[0].mutate();
      indiv[0].mutate();
      indiv[0].mutate();
      indiv[0].nn().init();
      BOOST_CHECK(true);
      std::ofstream ofs("/tmp/serialize_nn1.dot");
      indiv[0].nn().write(ofs);
      {
	std::ofstream ofs("/tmp/serialize_nn1.xml");
	oa_t oa(ofs);
	oa << boost::serialization::make_nvp("nn", indiv[0]);
      }
      BOOST_CHECK(true);
      {
	std::ifstream ifs("/tmp/serialize_nn1.xml");
	std::cout<<"IAT2 start"<<std::endl;
	ia_t ia(ifs);
	std::cout<<"IAT2 end"<<std::endl;

	ia >> boost::serialization::make_nvp("nn", indiv[1]);
      }
      BOOST_CHECK(true);
      // test copy
      indiv[2].nn() = indiv[0].nn();      
      BOOST_CHECK(true);
      check_nn_equal(indiv[0].nn(), indiv[1].nn());
      check_nn_equal(indiv[0].nn(), indiv[2].nn());    
    }
}
