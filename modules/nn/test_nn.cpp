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
#define BOOST_TEST_MODULE nn


#include <boost/test/unit_test.hpp>

#include <iostream>
#include <cmath>
#include <algorithm>
#include "nn.hpp"

BOOST_AUTO_TEST_CASE(nn_leaky)
{
  using namespace nn;
  NN<Neuron<PfLeaky, AfThreshold>, Connection> nn, nn2;
  nn.set_nb_inputs(1);
  nn.set_nb_outputs(2);
  nn.load("oscill.dot");
  
  std::vector<float> in(1); 
  in[0] = 1.0f;
  for (unsigned i = 0; i < 2000; ++i)
    nn.step(in);
  float out = nn.get_outf(0);
  
  nn2 = nn;
  nn2.init();
  BOOST_CHECK_EQUAL(nn2.get_nb_inputs(), nn.get_nb_inputs());
  BOOST_CHECK_EQUAL(nn2.get_nb_outputs(), nn.get_nb_outputs());
  BOOST_CHECK_EQUAL(nn2.get_nb_connections(), nn.get_nb_connections());
  BOOST_CHECK_EQUAL(nn2.get_nb_neurons(), nn.get_nb_neurons());
  
  for (unsigned i = 0; i < 2000; ++i)
    nn.step(in);
  float out2 = nn.get_outf(0);
  BOOST_CHECK(fabs(out - out2) < 0.01);
}


BOOST_AUTO_TEST_CASE(nn_simplify)
{
  using namespace nn;
  NN<Neuron<PfWSum, AfSigmoid, float>, Connection> nn1, nn2;
  nn1.load("simplify1.dot");
  // 4 5 and 6 should be removed
  nn1.init();
  BOOST_CHECK_EQUAL(nn1.get_nb_neurons(), 2 + 2 + 6);
  nn1.simplify(true);
  BOOST_CHECK_EQUAL(nn1.get_nb_neurons(), 2 + 2 + 6 - 3);
  nn1.write("blah.dot");

  nn2.load("simplify1.dot");
  // 6 should be removed
  nn2.init();
  BOOST_CHECK_EQUAL(nn2.get_nb_neurons(), 2 + 2 + 6);
  nn2.simplify();
  BOOST_CHECK_EQUAL(nn2.get_nb_neurons(), 2 + 2 + 6 - 1);
  nn2.write("blah2.dot");
}


BOOST_AUTO_TEST_CASE(nn_osc)
{
  srand(time(0));
  using namespace nn;
  typedef std::pair<float, float> io_t;
  typedef Neuron<PfIjspeert, AfDirectT<io_t>, io_t> n_t;

  typedef ConnectionT<io_t> c_t;
  typedef NN<n_t, c_t> nn_t;

  nn_t nn;
  std::vector<io_t> in; 
  in.push_back(std::make_pair(0.0f, 0.0f));
  nn.set_nb_inputs(1);
  nn.set_nb_outputs(1);
  nn_t::vertex_desc_t n = nn.add_neuron("h1");
  nn.add_connection(nn.get_input(0), n, std::make_pair(1.0f, 1.0f));
  nn.add_connection(n, nn.get_output(0), std::make_pair(1.0f, 1.0f));
  nn.init();
  std::ofstream ofs("out.dat");
  int pos = 0, neg = 0;
  for (size_t i = 0; i < 2000; ++i)
    {
      nn.step(in);
      io_t o = nn.get_outf(0);
      ofs<<o.first<<std::endl;
      if (o.first < 0)
	++neg;
      else
	++pos;      
    }
  //  BOOST_CHECK(neg > 900);
  //  BOOST_CHECK(pos > 900);
  nn.write("test.dot");
}
