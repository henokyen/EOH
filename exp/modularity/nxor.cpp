
 //#define NO_PARALLEL

#include <iostream>
#include <bitset>

#include <sferes/phen/parameters.hpp>
#include <sferes/gen/evo_float.hpp>
#include <sferes/gen/sampled.hpp>

#include <sferes/ea/nsga2.hpp>
#include <sferes/eval/eval.hpp>
#include <sferes/stat/pareto_front.hpp>
#include <sferes/modif/dummy.hpp>
#include <sferes/run.hpp>
#include <modules/nn2/nn.hpp>
#include <modules/nn2/gen_dnn_ff.hpp>
#include <modules/nn2/phen_dnn.hpp>
#include <modules/modularity/modularity.hpp>

#include "pnsga.hpp"
#include "behavior_div.hpp"
 //#include "behavior_nov.hpp"
#include "gen_spatial.hpp"
#include "stat_modularity.hpp"
#include "mvg.hpp"
#include "opt_placement.hpp"
#include "pnsga.hpp"

#ifdef NOVMAP
 # include "modif_novmap.hpp"
#endif


using namespace sferes;
using namespace nn;
using namespace sferes::gen::evo_float;
using namespace sferes::gen::dnn;

#include <boost/algorithm/string.hpp>

#define SFERES_PARAMS(P)                                                \
  struct Ps__ { Ps__() {                                                    \
                  static std::string __params = # P;                                   \
                  boost::replace_all(__params, ";", ";\n");                           \
                  boost::replace_all(__params, "{", "{\n");                           \
                  boost::replace_all(__params, "static const", "");                   \
                  std::cout << "Parameters:" << __params << std::endl; } };                 \
  P;                                                                    \
  static Ps__ ____p;

SFERES_PARAMS(
  struct Params
  {
    static long seed;
    struct pop
    {
      static const unsigned size = 1000;
       //static const unsigned nb_gen = 5001;
#ifdef NOVMAP
      static const unsigned nb_gen = 100001;
#else
      static const unsigned nb_gen = 25001;
#endif
      static const int dump_period = nb_gen - 1;
      static const int initial_aleat = 1;

    };

    struct ea
    {
#ifdef PR10
      SFERES_ARRAY(float, obj_pressure, 1.0, 0.1, 1.0);
#elif defined(PR25)
      SFERES_ARRAY(float, obj_pressure, 1.0, 0.25, 1.0);
#elif defined(PR50)
      SFERES_ARRAY(float, obj_pressure, 1.0, 0.5, 1.0);
#else
      SFERES_ARRAY(float, obj_pressure, 1.0, 1.0, 1.0);
#endif
    };
    struct rand_obj
    {
      static const float std_dev = 0.1;
    };

    struct dnn
    {
      struct bias
      {
        struct evo_float
        {
          static const float cross_rate = 0.5f;
#ifdef NF4
          static const float mutation_rate = 1.0 / 20.0f;
#elif defined (NF5)
          static const float mutation_rate = 1.0 / 30.0f;
#elif defined (NF8)
          static const float mutation_rate = 1.0 / 80.0f;
#endif
          static const float eta_m = 10.0f;
          static const float eta_c = 10.0f;
          static const mutation_t mutation_type = polynomial;
          static const cross_over_t cross_over_type = sbx;
        };
        struct sampled
        {
          SFERES_ARRAY(float, values, -2, -1, 0, 1, 2);
          static const float mutation_rate = 1.0 / 15.0f;
          static const float cross_rate = 0.5f;
          static const bool ordered = true;
        };
      };
      struct weight
      {
        struct sampled
        {
          SFERES_ARRAY(float, values, -2, -1, 1, 2);
          static const float mutation_rate = 1.0f;
          static const float cross_rate = 0.5f;
          static const bool ordered = true;
        };
        struct evo_float
        {
          static const float cross_rate = 0.5f;
          static const float mutation_rate = 1.0f;
          static const float eta_m = 10.0f;
          static const float eta_c = 10.0f;
          static const mutation_t mutation_type = polynomial;
          static const cross_over_t cross_over_type = sbx;
        };
      };


#ifdef NF4
      static const size_t nb_outputs = 4;
#elif defined(NF5)
      static const size_t nb_outputs = 5;
#elif defined(NF8)
      static const size_t nb_outputs = 8;
#elif defined(SUM4)
      static const size_t nb_outputs = 1;
#endif

#ifdef SUM4
      static const size_t nb_inputs = 8;
#else
      static const size_t nb_inputs = nb_outputs * 2;
#endif

      static const size_t min_nb_neurons = 10; //useless
      static const size_t max_nb_neurons = 30; //useless

      //      static const size_t min_nb_conns = 20; //default that works well
      static const size_t min_nb_conns = 0;
      static const size_t max_nb_conns = 100; //nb_outputs * 10;

      static const float m_rate_add_conn = 0.2f;
      static const float m_rate_del_conn = 0.2f;

      static const float m_rate_change_conn = 0.0f;
      static const float m_rate_add_neuron = 0.05f;
      static const float m_rate_del_neuron = 0.05f;
      static const float m_avg_weight = 2.0f;

      static const int io_param_evolving = true;
      static const init_t init = ff;
      struct spatial
      {
#ifdef NF4
        SFERES_ARRAY(int, layers, 8, 8, 4);
        SFERES_ARRAY(float, x,
                     -3.5, -2.5, -1.5, -0.5, 0.5, 1.5, 2.5, 3.5,
                     -3.5, -2.5, -1.5, -0.5, 0.5, 1.5, 2.5, 3.5,
                     -3, -1, 1, 3
                     );
        SFERES_ARRAY(float, y,
                     0, 0, 0, 0, 0, 0, 0, 0,
                     1, 1, 1, 1, 1, 1, 1, 1,
                     2, 2, 2, 2);
        SFERES_ARRAY(float, z,
                     0, 0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0);
#elif defined(NF5)
        SFERES_ARRAY(int, layers, 10, 10, 5);
        SFERES_ARRAY(float, x,
                     -4.5, -3.5, -2.5, -1.5, -0.5, 0.5, 1.5, 2.5, 3.5, 4.5,
                     -4.5, -3.5, -2.5, -1.5, -0.5, 0.5, 1.5, 2.5, 3.5, 4.5,
                     -4, -2, 0, 2, 4
                     );
        SFERES_ARRAY(float, y,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                     1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                     2, 2, 2, 2, 2);
        SFERES_ARRAY(float, z,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0);
#elif defined(NF8)
        SFERES_ARRAY(int, layers, 16, 16, 8);
        SFERES_ARRAY(float, x,
                     -7.5, -6.5, -5.5, -4.5, -3.5, -2.5, -1.5, -0.5, 0.5, 1.5, 2.5, 3.5, 4.5, 5.5,
                     6.5, 7.5,
                     -7.5, -6.5, -5.5, -4.5, -3.5, -2.5, -1.5, -0.5, 0.5, 1.5, 2.5, 3.5, 4.5, 5.5,
                     6.5, 7.5,

                     -7, -5, -3, -1, 1, 3, 5, 7,
                     );
        SFERES_ARRAY(float, y,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                     2, 2, 2, 2, 2, 2, 2, 2);
        SFERES_ARRAY(float, z,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 2, 2, 2, 2);
#elif defined(SUM4)
        SFERES_ARRAY(int, layers, 8, 8, 4, 1);
        SFERES_ARRAY(float, x,
                     -3.5, -2.5, -1.5, -0.5, 0.5, 1.5, 2.5, 3.5,
                     -3.5, -2.5, -1.5, -0.5, 0.5, 1.5, 2.5, 3.5,
                     -3, -1, 1, 3,
                     0
                     );
        SFERES_ARRAY(float, y,
                     0, 0, 0, 0, 0, 0, 0, 0,
                     1, 1, 1, 1, 1, 1, 1, 1,
                     2, 2, 2, 2,
                     3);
        SFERES_ARRAY(float, z,
                     0, 0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0,
                     0);
#endif
        static const float min_coord = -4.0f;
        static const float max_coord = 4.0f;
#ifdef RANDCOORDS
        static const bool rand_coords = true;
#else
        static const bool rand_coords = false;
#endif
      };
    };

    struct parameters
    {
       // maximum value of parameters (weights & bias)
      static const float min = 0;
       // minimum value
      static const float max = 1;
    };
    struct stats
    {
      static const size_t period = 1;
    };
    struct fitness
    {
#if defined(NF4)
      static const size_t b_size = 256 * Params::dnn::nb_outputs;
#elif defined(SUM4)
      static const size_t b_size = 256;
#elif defined(NF5)
      static const size_t b_size = 1024 * Params::dnn::nb_outputs;
#elif defined(NF8)
      static const size_t b_size = 65536 * Params::dnn::nb_outputs;
#endif
    };
  };
  ) // end of params



// -1 to +1 sigmoid
namespace nn
{
  template<typename P>
  struct AfTanhNxor : public Af<P>
  {
    typedef P params_t;
    static const float lambda = 20.0f;
    AfTanhNxor() {
      assert(trait<P>::size(this->_params) == 1);
    }
    float operator() (float p) const
    {
      return tanh(lambda * (p + trait<P>::single_value(this->_params)));
    }
  };
}

SFERES_FITNESS(FitXorXor, sferes::fit::Fitness)
{
  public:
    FitXorXor()  {
    }

    template<typename Indiv>
    float dist(const Indiv& o) const
    {
      return (o.fit()._behavior ^ _behavior).count() / (float) _behavior.size();
    }

     // size of the behavior vector
    static const size_t b_size = Params::fitness::b_size;
    typedef std::bitset<b_size> behavior_t;

    template<typename Indiv>
    void eval(Indiv& ind)
    {
      std::vector<float> inputs(Params::dnn::nb_inputs);
      int k = (int)pow(2, Params::dnn::nb_inputs);
      float err1 = 0, err2 = 0, nba = 0;
      for (size_t s = 0; s < k; ++s)
      {
        std::bitset<Params::dnn::nb_inputs> in(s);
        for (size_t i = 0; i < in.size(); ++i)
          inputs[i] = in[i] ? 1 : -1;
         // step
        for (size_t i = 0; i < ind.gen().get_depth(); ++i)
          ind.nn().step(inputs);
         // errors
#ifdef SUM4
        float sum_wanted = 0, sum_observed = 0;
        for (size_t i = 0; i < Params::dnn::nb_inputs; i += 2)
        {
          bool wanted = ((bool)(inputs[i] > 0)) ^ ((bool)(inputs[i + 1] > 0));
          sum_wanted += wanted;
        }
        bool out = ind.nn().get_outf(0) > 0;
        _behavior[s] = out;
        bool wanted = (sum_wanted >= 2);
        nba += wanted;
        if (wanted != out)
          err1 += 1;
        if (this->mode() == sferes::fit::mode::view)
          std::cout << "wanted: " << wanted << " out:" << out << std::endl;
#else
        for (size_t i = 0; i < Params::dnn::nb_inputs; i += 2)
        {
          bool wanted = ((bool)(inputs[i] > 0)) ^ ((bool)(inputs[i + 1] > 0));
          bool out = ind.nn().get_outf(i / 2) > 0 ? true : false;
          if (wanted != out)
            err1++;

          bool wanted2 = false;
          if (i <= 2)
            wanted2 = ((bool)(inputs[i] > 0)) ^ ((bool)(inputs[i + 1] > 0));
          else
            wanted2 = ((bool)(inputs[i] > 0)) && ((bool)(inputs[i + 1] > 0));
          bool out2 = ind.nn().get_outf(i / 2) > 0 ? true : false;
          if (wanted2 != out2)
            err2++;
          _behavior[s * Params::dnn::nb_outputs + i / 2] = out;
        }
#endif
      }
      this->_objs.clear();

#ifdef MVG
      this->_objs.push_back(-42); // this will be set later
#endif
      float norm = k * Params::dnn::nb_outputs;
      this->_values.resize(2);
      this->_values[0] = 1 - err1 / norm;
      this->_values[1] = 1 - err2 / norm;

#ifdef FG
      this->_objs.push_back(1 - err1 / norm);
#endif
      this->_value = 1 - err1 / norm;

       // compute the modularity
      typename Indiv::nn_t::graph_t gsimp = ind.gen().simplified_nn().get_graph();
      if (boost::num_edges(gsimp) == 0 || boost::num_vertices(gsimp) == 0)
        _mod = 0;
      else
        _mod = mod::modularity(gsimp);

#ifdef MOD
      this->_objs.push_back(_mod);
#endif

      _length = ind.gen().compute_length();
      _optlength = ind.gen().compute_optimal_length();

#ifdef MAX_LENGTH
      this->_objs.push_back(-ind.gen().max_length());
#endif


      this->_nb_conns = boost::num_edges(gsimp);
#ifdef NCONNS
      this->_objs.push_back(-(int)boost::num_edges(gsimp));
#endif
      
      this->_nb_nodes = boost::num_vertices(gsimp);
#ifdef NNODES
      this->_objs.push_back(-(int)boost::num_vertices(gsimp));
#endif

#ifdef NCNODES
      this->_objs.push_back(-(int)boost::num_edges(gsimp) - (int)boost::num_vertices(gsimp));
#endif



#if defined(TLENGTH) && !defined(ONP)
      this->_objs.push_back(-ind.gen().compute_length(1.5)); //default
#endif

#if defined(TLENGTH) && defined(ONP)
      bool write_svg = (this->mode() == fit::mode::view);
      this->_objs.push_back(-ind.gen().compute_optimal_length(1.5, write_svg));
#endif

#if defined(LENGTH) && !defined(ONP)
      this->_objs.push_back(-_length);
#endif

#if defined(LENGTH) && defined(ONP)
      bool write_svg = (this->mode() == fit::mode::view);
      this->_objs.push_back(-ind.gen().compute_optimal_length(0, write_svg));
#endif


#if defined(DIV) || defined(NOV) || defined(NOVMAP)
      this->_objs.push_back(0);
#endif

#ifdef RANDOBJ
      this->_objs.push_back(rand() / (float)RAND_MAX);
#endif

      if (this->mode() == sferes::fit::mode::view)
      {
        std::cout << "behavior:" << _behavior << std::endl;
        std::ofstream ofs("/tmp/nn.dot");
        ind.nn().write(ofs);
        std::cout << "- FIT:" << 1 - err1 / norm << std::endl;
        std::cout << "- nb conns:" << ind.nn().get_nb_connections() << std::endl;
        std::cout << "- MOD:" << _mod << std::endl;
        std::cout << "- length:" << _length << std::endl;
        std::cout << "- nb neurons:" << ind.nn().get_nb_neurons() << std::endl;
         // dump in nn.svg
        std::ofstream ofs2("nn.svg");
        ind.gen().write_svg(ofs2);
      }
    }
    void set_value(float v) { this->_value = v; }
    float values(size_t k) const { return _values[k]; }
    size_t nb_values() const { return _values.size(); }
    const behavior_t& behavior() const { return _behavior; }
    float mod() const { return _mod; }
    float length() const { return _length; }
    float optlength() const { return _optlength; }
    int nb_nodes() const { return _nb_nodes; }
    int nb_conns() const { return _nb_conns; }
  protected:
    std::vector<float> _values;
    behavior_t _behavior;
    float _mod;
    float _length, _optlength;
    int _nb_nodes, _nb_conns;
};

long Params::seed = -1;
int main(int argc, char **argv)
{
  typedef FitXorXor<Params> fit_t;
  typedef phen::Parameters<gen::Sampled<1, Params::dnn::weight>,
                           fit::FitDummy<>, Params> weight_t;
  typedef phen::Parameters<gen::Sampled<1, Params::dnn::bias>,
                           fit::FitDummy<>, Params> bias_t;
  typedef PfWSum<weight_t> pf_t;
  typedef AfTanhNxor<bias_t> af_t;
  typedef sferes::gen::DnnSpatial<Neuron<pf_t, af_t>, Connection<weight_t>, Params> gen_t;
  typedef phen::Dnn<gen_t, fit_t, Params> phen_t;
  typedef eval::Parallel<Params> eval_t;
  typedef boost::fusion::vector
  <
    //    stat::ParetoFront<phen_t, Params>, // comment this line to remove pareto.dat
    stat::Mod<phen_t, Params>
  >  stat_t;

   // MVG cannot be used with DIV !
#if defined(DIV)and !defined(MVG)
  typedef modif::BehaviorDiv<> modifier_t;
#elif defined(MVG)
  typedef modif::Mvg<> modifier_t;
#elif defined(NOVMAP)
  typedef modif::Novmap<> modifier_t;
#else
  typedef modif::Dummy<> modifier_t;
#endif

#ifdef PNSGA
  typedef ea::Pnsga<phen_t, eval_t, stat_t, modifier_t, Params> ea_t;
#else
  typedef ea::Nsga2<phen_t, eval_t, stat_t, modifier_t, Params> ea_t;
#endif
  ea_t ea;

  if (argc > 1) // if a number is provided on the command line
  {
    int randomSeed = atoi(argv[1]);
    printf("randomSeed:%i\n", randomSeed);
    srand(randomSeed);  //set it as the random seed
    Params::seed = randomSeed;
    run_ea(argc, argv, ea, false);
  }
  else
  {
    run_ea(argc, argv, ea);
  }


  return 0;
}
