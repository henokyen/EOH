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
#define BOOST_TEST_MODULE cartpole_ea

#include <boost/test/unit_test.hpp>
#include <boost/foreach.hpp>


#include <sferes/phen/parameters.hpp>
#include <sferes/gen/evo_float.hpp>
#include <sferes/ea/rank_simple.hpp>
#include <sferes/eval/eval.hpp>
#include <sferes/stat/best_fit.hpp>
#include <sferes/stat/mean_fit.hpp>
#include <sferes/modif/dummy.hpp>

#include "simu_cartpole.hpp"
#include "ctrl_cartpole.hpp"
#include "fit_cartpole.hpp"

using namespace sferes;
using namespace sferes::ctrl;
using namespace sferes::gen::evo_float;
// all the params
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
 struct evo_float
  {

    static const float mutation_rate = 0.1f;
    static const float cross_rate = 0.5f;
    static const float eta_m = 15.0f;
    static const float eta_c = 10.0f;
    static const mutation_t mutation_type = polynomial;
    static const cross_over_t cross_over_type = sbx;
  };

  struct pop
  {
    static const unsigned size = 25;
    static const unsigned nb_gen = 10;
    static const int dump_period = -1;
    static const int initial_aleat = 2;
    static const float coeff = 1.1f;
    static const float keep_rate = 0.6f;    
  };

  struct parameters
  {
    static const float min = -200.0f;
    static const float max = 200.0f;
  };

};



// our control architecture (PID)
SFERES_AGENT(AgentCart, Agent)
{
 public:
  AgentCart() : sum(2, 1) {}
  template<typename Indiv>
  void init(const Indiv&) {}
  template<typename Indiv>
    void refresh_params(const Indiv& indiv)
  {
    pid_theta.set_pid(indiv.data(0), indiv.data(1), indiv.data(2));
    pid_x.set_pid(indiv.data(3), indiv.data(4), indiv.data(5));
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

// our fitness scheduler
SFERES_FITNESS_SIMU(FitTest, fit::FitCartpole)
{
 public:
  SFERES_SCHEDULER() // define yours !
  { 
    NEW_EXP(0);
    AT(0)	this->simu().set_theta(0.01);
    AT(0)	this->simu().set_x(0.2);
    END_EXP(999);

    NEW_EXP(1000);
    AT(1000)	this->simu().set_theta(-0.1);
    AT(1000)	this->simu().set_x(-0.05);
    END_EXP(1999);
    END_EVAL(1999);
  }
};


void init_debug()
{
  // attach our streams
  dbg::init();
  dbg::enable(dbg::all, "fit", true);
  dbg::attach_ostream(dbg::info, "fit", std::cout);
  dbg::enable(dbg::all, "ea", true);
  dbg::attach_ostream(dbg::info, "ea", std::cout);
}


BOOST_AUTO_TEST_CASE(cartpole_ea)
{
  // cartpole
  typedef simu::SimuCartpole<Params> simu_t;
  typedef AgentCart<Params> agent_t;
  typedef FitTest<simu_t, agent_t, Params> fit_t;

  // ea
  typedef gen::EvoFloat<6, Params> gen_t;
  typedef phen::Parameters<gen_t, fit_t, Params> phen_t;
  typedef eval::Eval<Params> eval_t;
  typedef modif::Dummy<> modifier_t;
  typedef boost::fusion::vector<stat::BestFit<phen_t, Params>, stat::MeanFit<Params> >  stat_t;
  typedef ea::RankSimple<phen_t, eval_t, stat_t, modifier_t, Params> ea_t;
  
  // code
  init_debug();
  ea_t ea;
  ea.run();
}
