
// #define NO_PARALLEL
#define NO_PARALLEL

#include <fstream>
#include <iostream>
#include <bitset>
#include <limits>

#include <boost/random.hpp>
#include <boost/multi_array.hpp>

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
#include <sferes/stat/pareto_front.hpp>
#include <sferes/stat/best_fit.hpp>
#include <modules/nn2/phen_dnn.hpp>

#ifdef MPI
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
#include "common_constants.h"
#include "random_tools.h"
#include "modif_div.hpp"

#include "gen_hnn_3d.hpp"
#include "phen_hnn_3d.hpp"

using namespace sferes;
using namespace sferes::gen::dnn;
using namespace sferes::gen::evo_float;

struct Params
{
  static long seed;
  struct pop
  {
    static const unsigned size = 40;
    static const unsigned nb_gen = 1001;
    static const int dump_period = 1;
    static const int initial_aleat = 1;
  };

  struct cppn
  {
     // params of the CPPN
    struct sampled
    {
      SFERES_ARRAY(float, values, 0, 1, 2, 3);
      static const float mutation_rate = 0.1f;
      static const float cross_rate = 0.25f;
      static const bool ordered = false;
    };
    struct evo_float
    {
      static const float mutation_rate = 0.1f;
      static const float cross_rate = 0.1f;
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
      static const float m_rate_add_conn = 0.07f;
      static const float m_rate_del_conn = 0.06f;
      static const float m_rate_change_conn = 0.08f;
      static const float m_rate_add_neuron = 0.05f;
      static const float m_rate_del_neuron = 0.04f;

      static const size_t min_nb_neurons = 6;
      static const size_t max_nb_neurons = 20;
      static const size_t min_nb_conns = 30;
      static const size_t max_nb_conns = 150;
    };
  };
  struct hnn
  {
    static const size_t nb_rand_mutations = 0;
#ifdef FAST
    static const size_t nb_layers = 3;
    SFERES_ARRAY(float, layer_dim,
                 2, 21, 16, // input layer
		 1, 21, 16,
                 1, 1, 1); // output layer
#else
    static const size_t nb_layers = 3;
    SFERES_ARRAY(float, layer_dim,
                 11, 16, 21, // input layer
                 11, 16, 21, // hidden layer
                 1, 2, 1); // output layer
#endif
    typedef float weight_t;
    typedef float io_t;
    typedef nn::params::Vectorf<1> params_t;
    typedef nn::Neuron<nn::PfWSum<io_t>,
                       nn::AfSigmoidBias<params_t>, io_t > neuron_t;
    typedef nn::Connection<weight_t, io_t> connection_t;
  };
};

namespace global
{
  OSystem *theOSystem = (OSystem *) NULL;
  InternalController *game_controller;
  GameSettings *game_settings;
  SettingsUNIX*settings;
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
int edit_distance(const std::vector<P>& s1, const std::vector<P>& s2)
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




template<typename Indiv>
class HnnPlayer : public PlayerAgent
{

  public:
  HnnPlayer(GameSettings * game_settings, OSystem * osystem, 
	    Indiv & ind, bool dump) :
      PlayerAgent(game_settings, osystem),
      _best_y(-1), 
      _previous_action(PLAYER_A_NOOP),
      _indiv(ind),
      _dump(dump)
    {
      int cars_y[] = {175, 160, 145, 128, 111, 96, 80, 64, 47, 32};
      _car_y = std::vector<int>(cars_y, cars_y + 10);
      int cars_color_ind[] = {26, 216, 68, 136, 36, 130, 74, 18, 220, 66};
      _car_color = std::vector<int>(cars_color_ind, cars_color_ind + 10);
      _car_x.resize(10);
      std::fill(_car_x.begin(), _car_x.end(), -1);
      MediaSource& mediasrc = p_osystem->console().mediaSource();
      _screen_width = mediasrc.width();
      _screen_height = mediasrc.height();

       // layer, line, column
      _input_layer.resize(boost::extents
                          [1 + 1]
                          [_screen_height / 10]
                          [_screen_width / 10]);
      _input.resize(_input_layer.shape()[0]
                    * _input_layer.shape()[1]
                    * _input_layer.shape()[2]);
      if (_dump)
	_ofs = boost::shared_ptr<std::ofstream>(new std::ofstream("/tmp/nn.dat"));
      
    }
    virtual Action agent_step(const IntMatrix*screen_matrix,
                              const IntVect*console_ram,
                              int frame_number)
    {
      if (frame_number % 10 != 0)
        return _previous_action;
      Action special_action =
        PlayerAgent::agent_step(screen_matrix, console_ram, frame_number);
      if (special_action != UNDEFINED)
        return special_action;// We are resetting or in a delay

      int chicken_y = get_chicken_y(pm_curr_screen_matrix);

      _best_y = std::max(_screen_height - chicken_y, _best_y);
      if (_best_y > 180)
	_best_y = -1;
       if (_screen_height - chicken_y < _best_y)
       	 p_game_settings->stop_game();

      get_cars_x(pm_curr_screen_matrix);

      std::fill(_input_layer.origin(),
                _input_layer.origin() +
                _input_layer.size(), 0.0);
       // layer 0 : chicken
      assert(chicken_y / 10 < _screen_height / 10);
      _input_layer[0][chicken_y / 10][4] = 1.0f;
       // layers 1 : cars
      for (size_t i = 0; i < _car_x.size(); ++i)
        _input_layer[1][_car_y[i] / 10][_car_x[i] / 10] = 1.0f;

      size_t z = 0;
      for (size_t i = 0; i < _input_layer.shape()[0]; ++i)
        for (size_t j = 0; j < _input_layer.shape()[1]; ++j)
          for (size_t k = 0; k < _input_layer.shape()[2]; ++k, ++z)
            _input[z] = _input_layer[i][j][k];
      assert(_input.size() == _indiv.nn().get_nb_inputs());
      _indiv.nn().step(_input);
      float a1 = _indiv.nn().get_outf(0);
      
      if (_dump)
	_indiv.dump_layer(*_ofs, 1);
      int action = -1;
      if (a1 < 0.5)
        action = 0;
      else
        action = 2;
      if (_actions.size() < 200)
        _actions.push_back(action);

      switch (action)
	{
	case 0: _previous_action = PLAYER_A_NOOP; break;
	case 1: _previous_action = PLAYER_A_DOWN; break;
	default: _previous_action = PLAYER_A_UP; break;
	}
      return _previous_action;
    }
    const std::vector<int>& actions() const { return _actions; }
    int best_y() const { return _best_y; }
  protected:
  boost::shared_ptr<std::ofstream> _ofs;
    int _best_y;
    std::vector<int> _car_y;
  std::vector<int> _car_x;
    std::vector<int> _car_color;
    std::vector<int> _actions;
    int _screen_width;
    int _screen_height;
    Action _previous_action;
    boost::multi_array<float, 3> _input_layer;
    std::vector<float> _input;
    Indiv& _indiv;
  bool _dump;
    int get_chicken_y(const IntMatrix *pv_screen)
    {
      int chicken_color_ind = 30;
      int chicken_x_val = 48;
      int yellow_line_y_val1 = 102;
      int yellow_line_y_val2 = 104;
      for (int i = 0; i < _screen_height; i++)
      {
        for (int x = -2; x <= 2; x++)
        {
          if ((*pv_screen)[i][chicken_x_val + x] == chicken_color_ind &&
              i != yellow_line_y_val1 &&
              i != yellow_line_y_val2)
          {
            return i;
          }
        }
      }
      return -1; // cannot find the chicken
    }

    void get_cars_x(const IntMatrix *pm_screen)
    {
      for (int c = 0; c < 10; c++)
      {
        int car_y = _car_y[c];
        int color_ind = _car_color[c];
        bool car_found = false;
        for (int j = 0; j < _screen_width; j++)
        {
          for (int y = -2; y <= 2; y++)
          {
            if ((*pm_screen)[car_y + y][j] == color_ind)
            {
              car_found = true;
              _car_x[c] = j;
              break;
            }
          }
        }
        if (!car_found)
          _car_x[c] = 0;
      }
    }
};


SFERES_FITNESS(FitFreeway, sferes::fit::Fitness)
{
  public:
    template<typename Indiv>
    float dist(const Indiv& o) const
    {
      return edit_distance(this->_actions, o.fit()._actions);
    }

    template<typename Indiv>
    void eval(Indiv& indiv)
    {
      this->_objs.resize(2);
      indiv.nn().init();
      std::cout << ".. eval, nb_conns =" 
		<< indiv.nn().get_nb_connections() / 226128.0 * 100
		<< "%"<<"  |hnn:"<<indiv.gen().cppn().get_nb_neurons()
		<<" "<<indiv.gen().cppn().get_nb_connections()<<std::endl;
      bool dump_images = (this->mode() == sferes::fit::mode::view);
      HnnPlayer<Indiv> player(global::game_settings, global::theOSystem, indiv,
			      dump_images);
      global::game_controller->set_player_agent(&player);

      float reward =
        global::theOSystem->mainLoop(global::game_controller, 3500, dump_images);

      //      if (reward == 0)      
      reward += player.best_y() / 210.0f;
      std::cout << "reward:" << reward << std::endl;
      _actions = player.actions();
      this->_objs[0] = reward;
      //      this->_objs[1] = - (int)indiv.nn().get_nb_connections();
    }
    template<class Archive>
      void serialize(Archive & ar, const unsigned int version)
    {
      ar & boost::serialization::make_nvp("_value", this->_value);
      ar & boost::serialization::make_nvp("_objs", this->_objs);

      ar & BOOST_SERIALIZATION_NVP(_actions);
    }

  protected:
    std::vector<int> _actions;

};

void init_ale()
{
  global::theOSystem = new OSystemUNIX();
  global::settings = new SettingsUNIX(global::theOSystem);
  global::theOSystem->settings().loadConfig();

  char*romfile = SFERES_ROOT "/exp/deep_learning/ale/roms/freeway.bin";
  char*argv[] = { "ale", 0x0 };
  global::theOSystem->settings().loadCommandLine(1, argv);

  global::theOSystem->settings().validate();
  global::theOSystem->create();
  int rom = global::theOSystem->createConsole(romfile);
  assert(rom);
  global::theOSystem->settings().setString("rom_file", romfile);
  global::game_controller = new InternalController(global::theOSystem);
  global::game_settings = GameSettings::generate_game_Settings_instance(romfile);
  global::theOSystem->console().setPalette("standard");

}

long Params::seed = -1;
int main(int argc, char **argv)
{
  init_ale();

  using namespace nn;
  typedef FitFreeway<Params> fit_t;
  typedef phen::Parameters<gen::EvoFloat<1, Params::cppn>, fit::FitDummy<>, Params::cppn> weight_t;
  typedef gen::Hnn3d<weight_t, Params, Params::cppn> gen_t;
  typedef phen::Hnn3d<gen_t, fit_t, Params> phen_t;

#ifdef MPI
    typedef eval::Mpi<Params> eval_t;    
#else
  typedef eval::Eval<Params> eval_t;
#endif
  typedef boost::fusion::vector<
    sferes::stat::ParetoFront<phen_t, Params>
    >  stat_t;
  typedef modif::BehaviorDiv<Params> modifier_t;
  typedef ea::Nsga2<phen_t, eval_t, stat_t, modifier_t, Params> ea_t;
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
  delete global::game_controller;
  return 0;
}
