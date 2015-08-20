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
#define BOOST_TEST_MODULE direct_ff


#include <boost/test/unit_test.hpp>

#include <iostream>
#include <cmath>
#include <algorithm>

#include <sferes/fit/fitness.hpp>
#include <sferes/ctrl.hpp>
#include "direct.hpp"
#include "direct_nn.hpp"
#include "arch_nn.hpp"

using namespace sferes;
using namespace sferes::gen::direct;

struct Params
{
  struct direct
  {
    static const weight_t weight_type	= continuous;

    static const size_t nb_inputs	= 2;
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
    static const float m_rate_weight	= 1.0f;
  };
  struct nn
  {
    typedef ::nn::AfSigmoid activation_t;
    typedef ::nn::PfWSum potential_t;
    static const size_t nb_inputs = 2;
    static const size_t nb_outputs = 1;    
  };
};

namespace sferes
{
  namespace ctrl
  {
    SFERES_SENSOR(Sensor1, Sensor)
    {
    public:
      Sensor1() : nb_refresh(0) { this->_output = 1; }
      template<typename T>
	void refresh(const T& env) { ++nb_refresh; }
      int nb_refresh;
    };

    SFERES_AGENT(AgentTest, Agent)
    {
    public:
      template<typename Indiv>
	void refresh_params(const Indiv& indiv)
      {
	arch1.set_nn(indiv.nn());
      }
      
      template<typename Simu>
	void refresh_sensors(const Simu& s)
      {
	s1.refresh(s);
	s2.refresh(s);
      }

      template<typename Simu>
	void refresh_arch(const Simu& s)
      {
	arch1 << in(0) << out(s1)
	      << in(1) << out(s2)
	      << end;
      }

      template<typename Simu>
	void refresh_env(Simu& s)
      {}

      Sensor1<> s1;
      Sensor1<> s2;
      ArchNN<Params> arch1;
    protected:
    };

  }
}


BOOST_AUTO_TEST_CASE(direct_nn)
{
  srand(time(0));
  typedef phen::Direct<gen::Direct<Params>, fit::FitDummy<Params>, Params> phen_t;
  sferes::ctrl::AgentTest<Params> agent;
  phen_t indiv;

  for (size_t i = 0; i < 5; ++i)
    {
      indiv.random();
      indiv.develop();
      for (size_t j = 0; j < 100; ++j)
	agent.refresh(indiv, indiv);
    }
}



