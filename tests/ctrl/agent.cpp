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
#define BOOST_TEST_MODULE agent
#include <boost/test/unit_test.hpp>
#include <boost/foreach.hpp>
#include <sferes/ctrl.hpp>

#include <iostream>
using namespace sferes::ctrl;

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

    SFERES_SENSOR(Sensor2, Sensor1)
    {
    public:
      Sensor2() : nb_refresh(0) { this->_output = 2; }
      template<typename T>
	void refresh(const T& env) { ++nb_refresh; } 
      int nb_refresh;
    };


    SFERES_ARCH(ArchTest, Arch)
    {
    public:

      ArchTest(size_t nb_in, size_t nb_out) : 
	SFERES_PARENT(ArchTest, Arch)(nb_in, nb_out),
	nb_refresh(0)
	{}
      void refresh() 
      {
	float sum = 0;
	BOOST_FOREACH(float v, this->_inputs)
	  sum += v;
	BOOST_FOREACH(float &v, this->_outputs)
	  v = sum;
	++nb_refresh;
      }
      int nb_refresh;
    };

    SFERES_AGENT(AgentTest, Agent)
    {
    public:
      AgentTest() : 
	arch1(2, 2), 
	arch2(1, 1),
	nb_refresh_params(0),
	nb_refresh_sensors(0),
	nb_refresh_arch(0)
	  {}
      
      template<typename Indiv>
	void refresh_params(const Indiv& indiv)
      {
	++nb_refresh_params;
      }
      
      template<typename Simu>
      void refresh_sensors(const Simu& s)
      {
	s1.refresh(s);
	s2.refresh(s);
	++nb_refresh_sensors;
      }

      template<typename Simu>
      void refresh_arch(const Simu& s)
      {
	arch1 << in(0) << out(s1)
 	      << in(1) << out(s2)
	      << end;
 	arch2 << out(arch1) << end;
 	a1 << out(arch2) << end;
 	a2 << out(s1) << end;

	++nb_refresh_arch;
      }

      template<typename Simu>
	void refresh_env(Simu& s)
      {}

      Sensor1<> s1;
      Sensor2<> s2;
      ArchTest<> arch1;
      ArchTest<> arch2;
      ActDummy<> a1, a2;
      
      int nb_refresh_params;
      int nb_refresh_sensors;
      int nb_refresh_arch;

    protected:
    };

  }
}

BOOST_AUTO_TEST_CASE(agent_dummy)
{
  sferes::ctrl::AgentDummy<> agent;
  agent.refresh(agent, agent);

}

BOOST_AUTO_TEST_CASE(agent_complex)
{
  using namespace sferes::ctrl;

  sferes::ctrl::AgentTest<> agent;
  for (unsigned i = 0; i < 10; ++i)
    {
      agent.refresh(agent, agent);
      BOOST_CHECK_EQUAL(agent.a1.output(0),  3.0f);
      BOOST_CHECK_EQUAL(agent.a2.output(0),  1.0f);
    }

  BOOST_CHECK_EQUAL(agent.nb_refresh_params, 1);
  BOOST_CHECK_EQUAL(agent.nb_refresh_sensors, 10);
  BOOST_CHECK_EQUAL(agent.nb_refresh_arch, 10);

  BOOST_CHECK_EQUAL(agent.s1.nb_refresh, 10);
  BOOST_CHECK_EQUAL(agent.s2.nb_refresh, 10);
  BOOST_CHECK_EQUAL(agent.arch1.nb_refresh, 10);
  BOOST_CHECK_EQUAL(agent.arch2.nb_refresh, 10);

}

