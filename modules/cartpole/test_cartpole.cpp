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
#define BOOST_TEST_MODULE cartpole

#include <boost/test/unit_test.hpp>
#include <boost/foreach.hpp>

#include "simu_cartpole.hpp"
#include "ctrl_cartpole.hpp"
#include "fit_cartpole.hpp"

using namespace sferes;
using namespace sferes::ctrl;
struct Params
{
  struct simu
  {
    static const float x_min = -1;
    static const float x_max = 1;
    static const float pole_mass = 0.1f;
    static const float pole_length = 0.5f;
    static const float dt = 0.005;
    static const bool display = false;
    static const float theta_max = 0.3f;
  };
  struct fit
  {
    static const float x_max = 1.0f;
    static const float theta_max = 0.3f;
  };
};


SFERES_ARCH(ArchSimple, Arch)
{
 public:
  
  ArchSimple(size_t nb_in, size_t nb_out) : 
    SFERES_PARENT(ArchSimple, Arch)(nb_in, nb_out)
      {}
  void refresh() 
  {
    BOOST_FOREACH(float &v, this->_outputs)
      v = 0;
  }
};

SFERES_AGENT(AgentTest, Agent)
{
 public:
  AgentTest() : a1(2, 1) {}
  void init() { }
  template<typename Indiv>
    void refresh_params(const Indiv& indiv)
  {}
      
  template<typename Simu>
    void refresh_sensors(const Simu& s)
  {
    s1.refresh(s);
    s2.refresh(s);
  }

  template<typename Simu>
    void refresh_arch(const Simu& s)
  {
    a1 << in(0) << out(s1)
       << in(1) << out(s2)
       << end;
  }
  template<typename Simu>
    void refresh_env(Simu& s)
  {}

  SensorTheta<Params> s1;
  SensorX<Params> s2;
  ArchSimple<Params> a1;
};


BOOST_AUTO_TEST_CASE(cartpole_instance)
{
  typedef simu::SimuCartpole<Params> simu_t;
  typedef AgentTest<Params> agent_t;
  simu_t s;
  agent_t a;

  s.refresh();
  a.refresh(s, a);
}

SFERES_AGENT(AgentCart, Agent)
{
 public:
  AgentCart() : sum(2, 1) {}
  template<typename Indiv>
  void init(const Indiv&) {}
  template<typename Indiv>
    void refresh_params(const Indiv& indiv)
  {
    pid_theta.set_pid(100.0f, 0.0f, 1.0f);
    pid_x.set_pid(1.0f, 0.0f, 1.0f);
  }
  
  template<typename Simu>
    void refresh_sensors(const Simu& s)
  {
    s_theta.refresh(s);
    s_x.refresh(s);
  }
  
  template<typename Simu>
    void refresh_arch(const Simu& s)
  {
    pid_theta << out(s_theta) << end;
    pid_x << out(s_x) << end;
    sum << in(0) << out(pid_theta)
	<< in(1) << out(pid_x)
	<< end;
  }
  template<typename Simu>
    void refresh_env(Simu& s)
  {
    float f = sum.output();
    if (f > 10) f = 10;
    if (f < -10) f = -10;
    s.set_force(f);
  }
  SensorTheta<Params> s_theta;
  SensorX<Params> s_x;
  ArchPid<Params> pid_theta;
  ArchPid<Params> pid_x;
  ArchSum<> sum;
  
};

SFERES_FITNESS_SIMU(FitTest, fit::FitCartpole)
{
 public:
  SFERES_SCHEDULER()// define yours !
  { 
    NEW_EXP(0);
    AT(0)	this->simu().set_theta(0.01);
    AT(10)	this->simu().set_x(0.2);
    END_EXP(100);

    NEW_EXP(100);
    END_EXP(200);
    END_EVAL(200);
  }
};

BOOST_AUTO_TEST_CASE(cartpole_ctrl)
{
  typedef simu::SimuCartpole<Params> simu_t;
  typedef AgentCart<Params> agent_t;
  simu_t s;
  agent_t a;

  s.set_theta(0.01);
  for (unsigned i = 0; i < 1000; ++i)
    {
      s.refresh();
      a.refresh(s, a);
    }
  BOOST_CHECK(s.get_theta() < 0.01);
  BOOST_CHECK(s.get_x() < 0.01);

  FitTest<simu_t, agent_t, Params> fit;
  fit.eval(a);
  std::cout<<fit.objs()[0] <<" "<<fit.objs()[1]<<std::endl;
  BOOST_CHECK(fit.objs()[0] > 0.7);
  BOOST_CHECK(fit.objs()[0] > 0.7);
  
}
