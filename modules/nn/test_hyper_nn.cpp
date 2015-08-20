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

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <sferes/fit/fitness.hpp>
#include "gen_dnn.hpp"
#include "gen_hyper_nn.hpp"
#include "phen_hyper_nn.hpp"

using namespace sferes;
using namespace sferes::gen::dnn;

struct Params
{
  struct dnn
  {
    static const weight_t weight_type	= continuous;
    static const size_t nb_inputs	= 5;
    static const size_t nb_outputs	= 1;
    static const float  max_weight	= 2.0f;
    static const float  max_bias	= 2.0f;

    static const float m_rate_add_conn	= 1.0f;
    static const float m_rate_del_conn	= 0.3f;
    static const float m_rate_change_conn = 1.0f;
    static const float m_rate_add_neuron  = 1.0f;
    static const float m_rate_del_neuron  = 0.2f;

    static const float weight_sigma	= 0.5f;
    static const float vect_sigma	= 0.5f;
    static const float m_rate_weight	= 1.0f;
    static const float m_rate_fparams   = 1.0f;
    static const int io_param_evolving  = 1;     
    static const init_t init = ff;
  };
  struct hyper_nn
  {
    typedef nn::Neuron<nn::PfWSum, nn::AfTanh> neuron_t;
    typedef nn::Connection connection_t;
    SFERES_ARRAY(float, substrate, 
		 0.2f, 0.2f, // in 1
		 0.2f, 0.8f, // in 2
		 0.5f, 0.5f, // out 1
		 0.8f, 0.8f, // hidden 1
		 0.8f, 0.2f,  // hidden 2
		 0.2f, 0.5f, // hidden 3
		 0.5f, 0.2f  // hidden 4
		 ); 
    static const size_t nb_inputs	= 2;
    static const size_t nb_outputs	= 1;
    static const float  conn_threshold  = 0.2f;
    static const float	max_weight	= 2.0f;
  };
};


BOOST_AUTO_TEST_CASE(gen_cppn)
{
  srand(time(0));  
  typedef gen::HyperNn<Params> cppn_t;
  
  cppn_t gen1, gen2, gen3, gen4;
      
  gen1.random();
  for (size_t i = 0; i < 20; ++i)
    gen1.mutate();
  gen1.init();
  BOOST_CHECK(gen1.get_depth() >= 1);
  std::ofstream ofs2("/tmp/nn.dot");
  gen1.write(ofs2);

  //  generate a picture
#if 0
  char* pic = new char[512 * 512];
  std::vector<float> in(3);
  in[0] = 1;
  for (size_t i = 0; i < 512; ++i)
    for (size_t j = 0; j < 512; ++j)
      {
        in[1] = i / 256.0f - 1.0;
        in[2] = j / 256.0f - 1.0;
        for (size_t k = 0; k < 40; ++k)
          gen1.step(in);
        pic[512 * i + j] = (int)(gen1.get_outf(0) * 256 + 255);
      }
  std::ofstream ofs("/tmp/pic.pgm");
  ofs << "P5" << std::endl;
  ofs << "512 512" << std::endl;
  ofs << "255" << std::endl;
  ofs.write(pic, 512 * 512);
#endif
}

BOOST_AUTO_TEST_CASE(phen_hyper_nn)
{ 
  srand(time(0));  
  typedef gen::HyperNn<Params> gen_t;
  typedef phen::HyperNn<gen_t, fit::FitDummy<>, Params> phen_t;

  phen_t indiv;
  indiv.random();
  for (size_t i = 0; i < 5; ++i)
    indiv.mutate();
  std::ofstream ofs("/tmp/nn_substrate.dot");
  indiv.develop();
  indiv.nn().write(ofs);
  BOOST_CHECK_EQUAL(indiv.nn().get_nb_neurons(), 7);
  
}




