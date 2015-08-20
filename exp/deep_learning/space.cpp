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
#include "pareto_front_cppn.hpp"
#include "stat_cppn.hpp"
#include "pnsga.hpp"

#include "modif_div.hpp"
#include "modif_kmedoids.hpp"
#include "modif_unique.hpp"

using namespace sferes;
using namespace sferes::gen::dnn;
using namespace sferes::gen::evo_float;

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
                 7, 21, 16, // input layer
		 1, 21, 16,
                 1, 3, 1); // output layer
    typedef float weight_t;
    typedef float io_t;
    typedef nn::params::Vectorf<1> params_t;
    typedef nn::Neuron<nn::PfWSum<io_t>,
                       nn::AfSigmoidBias<params_t>, io_t > neuron_t;
    typedef nn::Connection<weight_t, io_t> connection_t;
  };
  struct fit
  {
#ifdef DUMP_POP
    static const size_t nb_steps = 100;
#else

    static const size_t nb_steps = 5000; //num frames in atari game
#endif
  };
};

namespace global
{
  OSystem *theOSystem = (OSystem *) NULL;
  InternalController *game_controller;
  RomSettings *rom_settings;
  SettingsUNIX*settings;
#ifdef DUMP_POP
  std::ofstream ofs_pop("pop.dat");
#endif
}

namespace sferes
{
  namespace misc
  {
    template<typename T1, typename T2, typename T3>
    T1 min(T1 x1, T2 x2, T3 x3)
    {
      return std::min(std::min(x1, x2), x3);
    }
  }
}
template<typename P>
int edit_distance(const std::vector<P>& s1, 
		  const std::vector<P>& s2)
{
  size_t m = s1.size() + 1;
  size_t n = s2.size() + 1;
  Eigen::MatrixXi d = Eigen::MatrixXi::Zero(m, n);
  for (size_t i = 0; i < m; ++i)
    d(i, 0) = i;
  for (size_t j = 0; j < n; ++j)
    d(0, j) = j;
  for (size_t j = 1; j < n; ++j)
    for (size_t i = 1; i < m; ++i)
      {
	if (i < s1.size() && j < s2.size() && s1[i] == s2[j])
	  d(i, j) = d(i - 1, j - 1);
	else
	  d(i, j) = sferes::misc::min(
				      d(i - 1, j) + 1, // a deletion
				      d(i, j - 1) + 1, // an insertion
				      d(i - 1, j - 1) + 1 // a substitution
				      );
      }
  return d(s1.size(), s2.size());
}

template<size_t K>
float hamming_distance(const std::bitset<K>& n1, const std::bitset<K>& n2)
{
  return (n1 ^ n2).count() / (float) n1.size();
}

std::pair<float, int> max(float a, float b, float c)
{
  float max_v = std::max(a, std::max(b, c));
  int max_i = 0;
  if (a > b && a > c)
    max_i = 0;
  if (b > a && b > c)
    max_i = 1;
  if (c > a && c > b)
    max_i = 2;
  return std::make_pair(max_v, max_i);
}

void print(const char* s)
{
#ifdef VISU
  std::cout<<s;
#endif
}

#ifdef VISU
int total_frames = 0;
#endif

template<typename Indiv>
class HnnPlayer : public PlayerAgent
{

public:
  HnnPlayer(OSystem * osystem, RomSettings* settings,
	    Indiv & ind, bool dump) :
    PlayerAgent(osystem, settings),
    _screen_height(p_osystem->getGameController()->screen_height()),
    _screen_width(p_osystem->getGameController()->screen_width()),
    _indiv(ind),
    _nb_frames(0),
    _actions(0)
  {      
    
    _input_layer.resize(boost::extents
			[7]
			[_screen_height / 10]
			[_screen_width / 10]);
    _input.resize(_input_layer.shape()[0]
		  * _input_layer.shape()[1]
		  * _input_layer.shape()[2]);
  }
  virtual Action act()
  {
    _nb_frames++;
  
    std::fill(_input_layer.origin(),
	      _input_layer.origin() +
	      _input_layer.size(), 0.0);
    for (size_t i = 0; i < _screen_height / 10 ; ++i)//here
      for (size_t j = 0; j < _screen_width / 10 ; ++j)
	{
	  std::bitset<6> r = 
	    _scan_square(i * 10, i * 10 + 10,
			 j * 10, j * 10 + 10,
			 p_osystem->getGameController()->screen_matrix(), 
			 _prev_screen,
			 _prev_screen2);
	  for (size_t k = 0; k < 5; ++k)
	    _input_layer[k][i][j] = r[k];
	  _input_layer[6][i][j] = sin(_nb_frames / 30.0) + 1 ;
	}


#ifdef VISU
    total_frames++;
    for (size_t k = 0; k < 7; ++k)
      {
	std::ostringstream fname;
	fname <<  "exported_screens/" << k << "_" 
	      << std::setw(9) << std::setfill('0') << (total_frames - 1)
	      << ".pbm";
	std::ofstream ofs(fname.str().c_str());
	ofs << "P1" << std::endl
	    <<  _screen_width / 10 << " " <<  _screen_height / 10<< std::endl;
	for (size_t i = 0; i < _screen_height / 10 ; ++i)
	  {
	    for (size_t j = 0; j < _screen_width / 10; ++j)
	      ofs << (int) _input_layer[k][i][j] << " ";
	    ofs << std::endl;
	  }
      }
#endif
	  
    size_t z = 0;
    for (size_t i = 0; i < _input_layer.shape()[0]; ++i)
      for (size_t j = 0; j < _input_layer.shape()[1]; ++j)
	for (size_t k = 0; k < _input_layer.shape()[2]; ++k, ++z)
	  _input[z] = _input_layer[i][j][k];
    assert(_input.size() == _indiv.nn().get_nb_inputs());
    _prev_screen2 = _prev_screen;
    _prev_screen = p_osystem->getGameController()->screen_matrix();

    for (size_t i = 0; i < Params::hnn::nb_layers - 1; ++i)
      _indiv.nn().step(_input);

    float a1 = _indiv.nn().get_outf(0);
    float a2 = _indiv.nn().get_outf(1);
    float a3 = _indiv.nn().get_outf(2);
    int action = 0;
    std::pair<int, float> ma = max(a1, a2, a3);
    if (ma.first > 0.5)
      action = ma.second + 1;
    Action r_action = PLAYER_A_NOOP;
#ifdef NOISE
    if (sferes::misc::rand<float>() < 0.1)
      {
	action = sferes::misc::rand<int>(0, 3) + 1 ;
	print("r");
      }
#endif
    switch (action)
      {
      case 0: 
	r_action = PLAYER_A_NOOP;
	print("_");
	break;
      case 1: 
	_actions[_nb_frames * 3] = 1;
	r_action = PLAYER_A_LEFT;
	print("<");
	break;
      case 2: 
	r_action = PLAYER_A_FIRE;
	_actions[_nb_frames * 3 + 2] = 1;
	print("!");
	break;
      case 3: 
	r_action = PLAYER_A_RIGHT; 
	_actions[_nb_frames * 3 + 1] = 1;
	print(">");
	break;
      default: assert(0);
      }
    //    std::cout.flush();
    return r_action;
  }
  const std::bitset<Params::fit::nb_steps*3>& actions() const { return _actions; }
  int nb_frames() const { return _nb_frames; }
protected:
  int _screen_height;
  int _screen_width;
  std::vector<std::vector<int> > _prev_screen;
  std::vector<std::vector<int> > _prev_screen2;
  boost::multi_array<float, 3> _input_layer;
  std::vector<float> _input;
  Indiv& _indiv;
  int _nb_frames;
  std::bitset<Params::fit::nb_steps * 3> _actions;

  std::bitset<6> _scan_square(int y1, int y2, int x1, int x2,
			      const IntMatrix& screen,
			      const IntMatrix& prev_screen,
			      const IntMatrix& prev_screen2
			      )
  {
    static const int space_ship = 196;
    static const int shelter = 52;
    static const int alien = 20;
    static const int bullet = 4;
    static const int mothership = 82;
    std::bitset<6> r(0);
    if (!prev_screen2.size())
      return r;
    for (size_t i = y1; i < y2; ++i)
      for (size_t j = x1; j < x2; ++j)
	{
	  assert(i < _screen_height);
	  assert(j < _screen_width);
	  r[0] = (r[0] || screen[i][j] == space_ship) && y1>100;
	  r[1] = (r[1] || screen[i][j] == shelter);
	  if (screen[i][j] == bullet 
	      && prev_screen[i][j] == bullet)
	    {
	      float yc = _identify_bullet(i, j, screen);
	      float yp = _identify_bullet(i, j, prev_screen);
	      float yp2 = _identify_bullet(i, j, prev_screen2);
	      float ypp = (yp == -1 || yp2 == -1) ? -1 : (yp + yp2) / 2.0f;
	      
	      if (yc != 1 && ypp != -1 && yc < ypp) // our bullets
		r[2] = 1;
	      else if (yc != -1 && ypp != -1)
		r[3] = 1;
	    }
	  r[4] = (r[4] || screen[i][j] == alien);
	  r[5] = (r[5] || screen[i][j] == mothership);
	}
    return r;
  }
  
  float _identify_bullet(int y, int x, const IntMatrix& screen)
  {
    int d1 = 0, d2 = 0;
    for (d1 = y; d1 < screen.size() && screen[d1][x] == 4; ++d1);
    for (d2 = y; d2 > 0 && screen[d2][x] == 4; --d2);
    if (d1 - d2 > 5)
      return d2 + (d1 - d2) / 2.0f;
    else
      return -1;
  }

  

  // spaceship : 196
  // shelter : 52
  // aliens : 20
  // bullet : 4
  // mothership : 82  
  void _dump_screen(const IntMatrix &screen)
  {
    static size_t c = 0;
    std::string s = "/tmp/screen" + boost::lexical_cast<std::string>(c++) + ".dat";
    std::ofstream ofs(s.c_str());
    for (int i = 0; i < _screen_height; ++i)
      {
        for (int j = 0; j < _screen_width; j++)
	  ofs << screen[i][j]<<" ";	      
  	ofs << std::endl;
      }
  }
};


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
    if (this->mode() == sferes::fit::mode::view)
      _eval(indiv, 101);
    _actions.clear();
    _rewards.clear();
    _rewards.push_back(_eval(indiv, 2));
    _rewards.push_back(_eval(indiv, 26));
    _rewards.push_back(_eval(indiv, 101));
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
    
    bool dump_images = (this->mode() == sferes::fit::mode::view);
    HnnPlayer<Indiv> player(global::theOSystem, global::rom_settings, indiv,
			    dump_images);
    global::game_controller->set_player_agent(&player);
    float reward =
      global::theOSystem->mainLoop(global::game_controller, 
				   Params::fit::nb_steps, 
				   k, dump_images);
    _actions.push_back(player.actions());
    _cppn_nodes = indiv.gen().cppn().get_nb_neurons();
    _cppn_conns = indiv.gen().cppn().get_nb_connections();
    _nn_nodes = indiv.nn().get_nb_neurons();
    _nn_conns = indiv.nn().get_nb_connections();
    std::cout<<"eval done, nb_frames="<<player.nb_frames() << std::endl;
    return reward;
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
    boost::dynamic_bitset<> actions_total(Params::fit::nb_steps * 3);
    for (size_t i = 0; i < _actions.size(); ++i)
      for (size_t j = 0; j < _actions[i].size(); ++j)
	actions_total[i * Params::fit::nb_steps * 3 + j] = _actions[i][j];
    return actions_total; 
  }

  const std::vector<std::bitset<Params::fit::nb_steps * 3> >& behavior() const
  {
    return _actions;
  }
  size_t cppn_nodes() const { return _cppn_nodes; }
  size_t cppn_conns() const { return _cppn_conns; }
  size_t nn_nodes() const { return _nn_nodes; }
  size_t nn_conns() const { return _nn_conns; }
  const std::vector<float>& rewards() const { return _rewards ;}
 protected:
  std::vector<std::bitset<Params::fit::nb_steps * 3> > _actions;
  size_t _cppn_nodes;
  size_t _cppn_conns;
  size_t _nn_nodes;
  size_t _nn_conns;
  std::vector<float> _rewards;
};


void init_ale()
{
  global::theOSystem = new OSystemUNIX();
  global::settings = new SettingsUNIX(global::theOSystem);
  setDefaultSettings(global::theOSystem->settings());
  global::theOSystem->settings().loadConfig();

  char*romfile = SFERES_ROOT "/exp/deep_learning/ale/roms/space_invaders.bin";
  char*argv[] = { "ale", 
		  "-max_num_episodes", "1", 
		  "-max_num_frames", "100000",
		  "-max_num_frames_per_episode", "10000",
		  0x0 };
  global::theOSystem->settings().loadCommandLine(7, argv);
  global::theOSystem->settings().validate();
  global::theOSystem->create();
  int rom = global::theOSystem->createConsole(romfile);
  assert(rom);
  global::theOSystem->settings().setString("rom_file", romfile);
  global::game_controller = new InternalController(global::theOSystem);
  global::theOSystem->setGameController(global::game_controller);
  global::rom_settings = buildRomRLWrapper(romfile);
  global::theOSystem->console().setPalette("standard");
}

long Params::seed = -1;
int main(int argc, char **argv)
{
  init_ale();

  using namespace nn;
  typedef FitSpace<Params> fit_t;
  typedef phen::Parameters<gen::EvoFloat<1, Params::cppn>, 
    fit::FitDummy<>, Params::cppn> weight_t;
typedef gen::Hnn3d<weight_t, Params, Params::cppn> gen_t;
typedef phen::Hnn3d<gen_t, fit_t, Params> phen_t;

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

  
 delete global::theOSystem;
 return 0;
}
