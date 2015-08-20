#ifdef NOISE
# define EA_EVAL_ALL
#endif

// #define NO_PARALLEL
#define NO_PARALLEL

#include <fstream>
#include <iostream>
#include <bitset>
#include <limits>
#include <bitset>

#include <boost/random.hpp>
#include <boost/multi_array.hpp>
#include <boost/serialization/bitset.hpp>

#include <sferes/dbg/dbg.hpp>
#include <sferes/stc.hpp>
#include <sferes/run.hpp>
#include <sferes/misc.hpp>
#include <sferes/phen/parameters.hpp>
#include <sferes/gen/evo_float.hpp>
#include <sferes/ea/nsga2.hpp>
#include <sferes/fit/fitness.hpp>
#include <sferes/modif/dummy.hpp>
#include <sferes/eval/parallel.hpp>
#include <sferes/eval/eval.hpp>
#include <sferes/stat/best_fit.hpp>
#include <modules/nn2/phen_dnn.hpp>

#if defined(EVAL_MPI)
#include <sferes/eval/mpi.hpp>
#endif

// --- ALE --
#include "bspf.hxx"
#include "Console.hxx"
#include "Event.hxx"
#include "PropsSet.hxx"
#include "Settings.hxx"
#include "FSNode.hxx"
#include "OSystem.hxx"
#include "SettingsUNIX.hxx"
#include "OSystemUNIX.hxx"
#include "fifo_controller.h"
#include "internal_controller.h"
#include "Constants.h"
#include "Roms.hpp"
#include "random_tools.h"
#include "RandomAgent.hpp"
#include "Defaults.hpp"


#include "gen_hnn_3d.hpp"
#include "phen_hnn_3d.hpp"
#include "phen_hnn_fixed.hpp"

#include "pareto_front_cppn.hpp"
#include "stat_cppn.hpp"
#include "pnsga.hpp"

#include "modif_div.hpp"
#include "modif_kmedoids.hpp"
#include "modif_unique.hpp"

using namespace sferes;
using namespace sferes::gen::dnn;
using namespace sferes::gen::evo_float;


namespace nn
{
  // basic bias = 1
  template <typename P = float>
  struct Afb1 : public Af<P>
  {
    typedef P params_t;
    static const float lambda = 5.0f;
    float operator()(float p) const
    {
      //      std::cout<<"tanh:"<<p<<" => "<< 1.0 / (exp((-p + 1 + trait<P>::single_value(this->_params)) * lambda) + 1) << std::endl;
      return 1.0 / (exp((-p + 1 + trait<P>::single_value(this->_params)) * lambda) + 1);
    }
  };
}

struct Params
{
  static long seed;
  struct pop
  {
    static const unsigned size = 60;
    static const unsigned nb_gen = 1001;
    static const int dump_period = 20;
    static const int initial_aleat = 1;
  };
  struct ea
  {
    SFERES_ARRAY(float, obj_pressure, 1.0, 0.5, 1.0); //pressure for each objective (performance, connection cost, diversity)
  };
  struct cppn
  {
    // params of the CPPN
    struct sampled
    {
      SFERES_ARRAY(float, values, 0, 1, 2, 3);
      static const float mutation_rate = 0.01f;
      static const float cross_rate = 0.25f;
      static const bool ordered = false;
    };
    struct evo_float
    {
      static const float mutation_rate = 0.1f;//per conn mut rate
      static const float cross_rate = 0.1f; //we don't use this
      static const mutation_t mutation_type = polynomial;
      static const cross_over_t cross_over_type = sbx;
      static const float eta_m = 10.0f;
      static const float eta_c = 10.0f;
    };
    struct parameters
    {
      // maximum value of parameters
      static const float min = -2.0f;
      // minimum value
      static const float max = 2.0f;
    };

    struct dnn
    {
      static const size_t nb_inputs = 9;
      static const size_t nb_outputs = 7;
      static const init_t init = ff;
      static const float m_rate_add_conn = 0.1f;
      static const float m_rate_del_conn = 0.1f;
      static const float m_rate_change_conn = 0.0f; //move conn to new neurons
      static const float m_rate_add_neuron = 0.2f;
      static const float m_rate_del_neuron = 0.01f;

      //these only count w/ random init, instead of feed forward
      static const size_t min_nb_neurons = 6;
      static const size_t max_nb_neurons = 20;
      static const size_t min_nb_conns = 30;
      static const size_t max_nb_conns = 150;
    };
  };
  struct hnn
  {
    static const size_t nb_rand_mutations = 0;
    static const size_t nb_layers = 3;
    SFERES_ARRAY(float, layer_dim,
                 2, 210/10, 160/10, // input layer
		 1, 210/10, 160/10,
                 1, 1, 2); // output layer
    typedef float weight_t;
    typedef float io_t;
    typedef nn::params::Vectorf<1> params_t;
    typedef nn::Neuron<nn::PfWSum<io_t>,
                       nn::Afb1<params_t>, io_t > neuron_t;
    typedef nn::Connection<weight_t, io_t> connection_t;
  };
  struct fit
  {
#ifdef DUMP_POP
    static const size_t nb_steps = 100;
#else

    static const size_t nb_steps = 350; //num frames in atari game
#endif
  };
};

inline bool _out_of_screen(const std::pair<int, int> & p)
{
  return p.first >= 210;
}
class BulletSimulator
{
public:
  BulletSimulator(long seed1, long seed2, bool dump_screen) : 
    _rand_pos(base_generator_t(seed1), 
	      boost::uniform_int<>(0, 159)),
    _rand_fire(base_generator_t(seed2), 
	       boost::uniform_01<>()),
    _player_pos(80),
    _dump_screen(dump_screen),
    _dead(false)
  {
    _screen.resize(boost::extents[210][160]);
  }
  bool dead() const { return _dead; }
  void step(int action)
  {
#ifdef NOISE
    if (sferes::misc::rand<float>() < 0.1)
      action = sferes::misc::rand<int>(0, 2) + 1 ;     
#endif

    // move player
    switch (action)
      {
      case 1:
	_player_pos = std::max(10, _player_pos - 4);
	break;
      case 2:
	_player_pos = std::min(159 - 10, _player_pos + 4);
	break;
      }

    // update current bullets
    for (blist_t::iterator it = _blist.begin(); it != _blist.end(); ++it)
      it->first += 5;
    
    // check if the player is alive
    for (blist_t::iterator it = _blist.begin(); it != _blist.end(); ++it)
      if (it->first >= 210 && abs(_player_pos - it->second) <= 10)
	_dead = true;

    // remove bullets that are out of the screen
    _blist.remove_if(_out_of_screen);

    // generate new bullets
    if (_rand_fire() < 0.05)
      _blist.push_back(std::make_pair(0, _rand_pos()));
    
    _draw_screen();
    _write_screen_pbm();
  }
  const boost::multi_array<int, 2>& screen() const { return _screen; }
private:
  void _draw_screen()
  {
    for (size_t i = 0; i < _screen.shape()[0] ; ++i)
      for (size_t j = 0; j < _screen.shape()[1]; ++j)
	_screen[i][j] = 0;

    // bullets
    for (blist_t::iterator it = _blist.begin(); it != _blist.end(); ++it)
      for (size_t i = it->first; i < std::min((int)_screen.shape()[0], it->first + 5); ++i)
	_screen[i][it->second] = 1;

    // spaceship
    for (int i = -10; i < 10; ++i)
      {
	int p = std::min(_player_pos + i, (int)_screen.shape()[1] - 1);
	p = std::max(0, p);
	assert(p < _screen.shape()[1]);
	_screen[205][p] = 2;
      }
  }
  void _write_screen_pbm()
  {
    static int nb_frames = 0;
    std::ostringstream fname;
    fname <<  "exported_screens/bullets_"
	  << std::setw(9) << std::setfill('0') << nb_frames
	  << ".pbm";
    std::ofstream ofs(fname.str().c_str());
    ofs << "P1" << std::endl
	<<  _screen.shape()[1] << " " <<  _screen.shape()[0] << std::endl;
    for (size_t i = 0; i < _screen.shape()[0] ; ++i)
      {
	for (size_t j = 0; j < _screen.shape()[1]; ++j)
	  if (_dead)
	    ofs << 1 << " ";
	  else
	    ofs << (bool) _screen[i][j] << " ";
	ofs << std::endl;
      } 
    ++nb_frames;
  }

  typedef boost::mt19937 base_generator_t;
  typedef boost::variate_generator<base_generator_t, boost::uniform_int<> > rand_int_t;
  typedef boost::variate_generator<base_generator_t, boost::uniform_01<> > rand_float_t;
  
  typedef std::list<std::pair<int, int> > blist_t;
  rand_int_t _rand_pos;
  rand_float_t _rand_fire;
  boost::multi_array<int, 2> _screen;
  blist_t _blist;
  int _player_pos;
  bool _dump_screen;
  bool _dead;
};

template<size_t K>
float hamming_distance(const std::bitset<K>& n1, const std::bitset<K>& n2)
{
  return (n1 ^ n2).count() / (float) n1.size();
}


SFERES_FITNESS(FitSpace, sferes::fit::Fitness)
{
 public:
  template<typename Indiv>
    float dist(const Indiv& o) const
  {
    float d = 0;
    assert(_actions.size() == o.fit()._actions.size());
    for (size_t i = 0; i < _actions.size(); ++i)
      d =+ hamming_distance(_actions[i], o.fit()._actions[i]);
    return d / _actions.size();
  }

  template<typename Indiv>
    void eval(Indiv& indiv)
  {
    this->_objs.resize(3);
#ifdef VISU
    indiv.nn().init();
    std::cout << "nb_conns =" 
	      << indiv.nn().get_nb_connections()
	      << "%"<<"  |hnn:"<<indiv.gen().cppn().get_nb_neurons()
	      <<" "<<indiv.gen().cppn().get_nb_connections()<<std::endl;
    indiv.save_svg("nn.svg");

#endif
    _actions.clear();
    _rewards.clear();
    _rewards.push_back(_eval(indiv, 2));
    _rewards.push_back(_eval(indiv, 26));
    _rewards.push_back(_eval(indiv, 101));
    _rewards.push_back(_eval(indiv, 102));
    _rewards.push_back(_eval(indiv, 200));
    _rewards.push_back(_eval(indiv, 122));
#ifdef VISU
    //test generalisation
    for (size_t i = 3; i < 20; ++i)
    _rewards.push_back(_eval(indiv, i));
#endif
    float reward = std::accumulate(_rewards.begin(), _rewards.end(), 0.0f) / _rewards.size();
    float connection_cost = - indiv.compute_connection_cost();
    std::cout<<"rewards:";
    for (size_t i = 0; i < _rewards.size(); ++i)
      std::cout<<_rewards[i]<<" ";
    std::cout<<std::endl;
    std::cout<<" connection cost:"<<connection_cost
	     << " |" << indiv.nn().get_nb_connections() << std::endl;
    this->_objs[0] = reward;
    this->_objs[1] = connection_cost;

  }
  
  template<typename Indiv>
    float _eval(Indiv& indiv, int k)
  {
    indiv.nn().init();
    BulletSimulator simu(k, k * 2, (this->mode() == sferes::fit::mode::view));
    std::bitset<Params::fit::nb_steps * 2> actions;
    size_t nb_steps = 0;
    _cppn_nodes = indiv.gen().cppn().get_nb_neurons();
    _cppn_conns = indiv.gen().cppn().get_nb_connections();
    _nn_nodes = indiv.nn().get_nb_neurons();
    _nn_conns = indiv.nn().get_nb_connections();

    _input_layer.resize(boost::extents
			[2]
			[210 / 10]// / 10]
			[160 / 10]);// / 10]);
    _input.resize(_input_layer.shape()[0]
		  * _input_layer.shape()[1]
		  * _input_layer.shape()[2]);
    for (nb_steps = 0; !simu.dead() && nb_steps < Params::fit::nb_steps; ++nb_steps)
      {
	for (size_t i = 0; i < _input_layer.shape()[1] ; ++i)//here
	  for (size_t j = 0; j < _input_layer.shape()[2] ; ++j)
	{
	  std::bitset<2> r = 
	    // _scan_square(i * 10, i * 10 + 10,
	    // 		 j * 10, j * 10 + 10,
	    // 		 simu.screen());
	    _scan_square(i * 10, i * 10 + 10,
	    		 j * 10, j * 10 + 10,
	    		 simu.screen());

	  for (size_t k = 0; k < _input_layer.shape()[0]; ++k)
	    _input_layer[k][i][j] = r[k];
	}


	size_t z = 0;
	for (size_t i = 0; i < _input_layer.shape()[0]; ++i)
	  for (size_t j = 0; j < _input_layer.shape()[1]; ++j)
	    for (size_t k = 0; k < _input_layer.shape()[2]; ++k, ++z)
	      _input[z] = _input_layer[i][j][k];
	assert(_input.size() == indiv.nn().get_nb_inputs());
	
	
	for (size_t i = 0; i < Params::hnn::nb_layers - 1; ++i)
	  indiv.nn().step(_input);

	float a1 = indiv.nn().get_outf(0);
	float a2 = indiv.nn().get_outf(1);
	int action = 0;
	if (a1 > 0.25 && a1 > a2)
	  {
	    action = 1;
	    actions[nb_steps * 2] = 1;
	  }
	if (a2 > 0.25 && a2 > a1)
	  {
	    action = 2;
	    actions[nb_steps * 2 + 1] = 1;
	  }

	simu.step(action);
	//	if (this->mode() == sferes::fit::mode::view)
	  _write_nn(indiv);
      }
    _actions.push_back(actions);
    std::cout<<"=>"<<nb_steps<<std::endl;
    return nb_steps;
  }
  template <typename I>
  void _write_nn(const I& indiv)
  {
    static int nb_frames = 0;
    std::ostringstream fname;
    fname <<  "exported_screens/nn_"
	  << std::setw(9) << std::setfill('0') << nb_frames++
	  << ".svg";
    indiv.save_svg(fname.str());
  }
  template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
  {
    ar & boost::serialization::make_nvp("_value", this->_value);
    ar & boost::serialization::make_nvp("_objs", this->_objs);
    ar & BOOST_SERIALIZATION_NVP(_actions);
    ar & BOOST_SERIALIZATION_NVP(_cppn_nodes);
    ar & BOOST_SERIALIZATION_NVP(_cppn_conns);
    ar & BOOST_SERIALIZATION_NVP(_nn_nodes);
    ar & BOOST_SERIALIZATION_NVP(_nn_conns);
    ar & BOOST_SERIALIZATION_NVP(_rewards);    
  }

  boost::dynamic_bitset<> actions_total() const 
  { 
    boost::dynamic_bitset<> actions_total(Params::fit::nb_steps * 2);
    for (size_t i = 0; i < _actions.size(); ++i)
      for (size_t j = 0; j < _actions[i].size(); ++j)
	actions_total[i * Params::fit::nb_steps * 2 + j] = _actions[i][j];
    return actions_total; 
  }

  const std::vector<std::bitset<Params::fit::nb_steps * 2> >& behavior() const
  {
    return _actions;
  }
  size_t cppn_nodes() const { return _cppn_nodes; }
  size_t cppn_conns() const { return _cppn_conns; }
  size_t nn_nodes() const { return _nn_nodes; }
  size_t nn_conns() const { return _nn_conns; }
  const std::vector<float>& rewards() const { return _rewards ;}
 protected:
  std::bitset<2> _scan_square(int y1, int y2, int x1, int x2,
			      const boost::multi_array<int, 2>& screen)
  {
    std::bitset<2> r(0);
    for (size_t i = y1; i < y2; ++i)
      for (size_t j = x1; j < x2; ++j)
	{
	  r[0] = (r[0] || screen[i][j] == 1);
	  r[1] = (r[1] || screen[i][j] == 2);
	}
    return r;
  }

  std::vector<std::bitset<Params::fit::nb_steps * 2> > _actions;
  size_t _cppn_nodes;
  size_t _cppn_conns;
  size_t _nn_nodes;
  size_t _nn_conns;
  std::vector<float> _rewards;
  boost::multi_array<float, 3> _input_layer;
  std::vector<float> _input;
};

long Params::seed = -1;
int main(int argc, char **argv)
{

  using namespace nn;
  typedef FitSpace<Params> fit_t;
  typedef phen::Parameters<gen::EvoFloat<1, Params::cppn>, 
    fit::FitDummy<>, Params::cppn> weight_t;
  typedef gen::Hnn3d<weight_t, Params, Params::cppn> gen_t;
#ifdef FIXED
  typedef phen::HnnFixed<gen_t, fit_t, Params> phen_t;
#else
  typedef phen::Hnn3d<gen_t, fit_t, Params> phen_t;
#endif

#if defined(EVAL_MPI)
typedef eval::Mpi<Params> eval_t;
#else
typedef eval::Eval<Params> eval_t;
#endif

typedef boost::fusion::vector<
  sferes::stat::ParetoFrontCppn<phen_t, Params>,
  sferes::stat::Cppn<phen_t, Params>
  >  stat_t;
#ifdef KMEDOIDS
 typedef modif::Kmedoids<Params> modifier_t;
#elif defined(DIV_UNIQUE)
 typedef modif::Unique<Params> modifier_t;
#else
 typedef modif::BehaviorDiv<Params> modifier_t;
#endif
 typedef ea::Pnsga<phen_t, eval_t, stat_t, modifier_t, Params> ea_t;
 ea_t ea;
  
 if (argc == 2) // if a number is provided on the command line
   {
     int randomSeed = atoi(argv[1]);
     std::cout<<"seed:"<<randomSeed <<std::endl;
     srand(randomSeed);  //set it as the random seed
     Params::seed = randomSeed;
     run_ea(argc, argv, ea, false);
   }
 else
   run_ea(argc, argv, ea);

 return 0;
}
