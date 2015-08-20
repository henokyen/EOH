#include <boost/assign/list_of.hpp>
#include <boost/random.hpp>

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
#include <modules/nn2/gen_hyper_nn.hpp>
#include <modules/nn2/phen_hyper_nn.hpp>

#include <modules/modularity/modularity.hpp>

#include "behavior_div.hpp"
#include "gen_spatial.hpp"
#include "stat_modularity.hpp"
#include "mvg.hpp"
#include "modularity.hpp"
#include "mod2.hpp"

#ifdef PNSGA
 #include "pnsga.hpp"
#endif

#include "arm_env.hpp"

#include <boost/algorithm/string.hpp>
#define SFERES_PARAMS(P)                                                    \
  struct Ps__ { Ps__() {                                                    \
                  static std::string __params = # P;                        \
                  boost::replace_all(__params, ";", ";\n");                 \
                  boost::replace_all(__params, "{", "{\n");                 \
                  boost::replace_all(__params, "static const", "");         \
                  std::cout << "Parameters:" << __params << std::endl; } }; \
  P;                                                                        \
  static Ps__ ____p;

using namespace sferes;
using namespace nn;
using namespace sferes::gen::evo_float;
using namespace sferes::gen::dnn;


// -1 to +1 sigmoid
template<typename P>
struct AfRetina : public Af<P>
{
  typedef P params_t;
  static const float lambda = 20.0f;
  AfRetina() {
    assert(trait<P>::size(this->_params) == 1);
  }
  float operator() (float p) const
  {
    return tanh(lambda * (p + trait<P>::single_value(this->_params)));
  }
  protected:
};

SFERES_PARAMS(
  struct Params
  {
    static long seed;
    struct pop
    {
       //      static const unsigned size = 200;//default
      static const unsigned size = 1000;
      static const unsigned nb_gen = 10001;
       //      static const unsigned nb_gen = 11;
       //static const unsigned nb_gen = 20001; //change me
       //static const int dump_period = 1000; //nb_gen - 1;
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
    struct dnn
    {
       // for weights
      struct weights
      {
        struct evo_float
        {
          static const float cross_rate = 0.5f;
           //1.0 because dnn handle this globally
          static const float mutation_rate = 1.0f;
          static const float eta_m = 10.0f;
          static const float eta_c = 10.0f;
          static const mutation_t mutation_type = polynomial;
          static const cross_over_t cross_over_type = sbx;
        };
        struct sampled
        {
           // no 0 !
          SFERES_ARRAY(float, values, 2, -1, 1, 2); //???-2??
          static const float mutation_rate = 1.0f;
          static const float cross_rate = 0.5f;
          static const bool ordered = true;
        };
      };
      struct bias
      {
        struct evo_float
        {
          static const float cross_rate = 0.5f;
          static const float mutation_rate = 1.0f / 24.0f;
          static const float eta_m = 10.0f;
          static const float eta_c = 10.0f;
          static const mutation_t mutation_type = polynomial;
          static const cross_over_t cross_over_type = sbx;
        };
        struct sampled
        {
          SFERES_ARRAY(float, values, -2, -1, 0, 1, 2);
          static const float mutation_rate = 1.0 / 24.0f;
          static const float cross_rate = 0.0f;
          static const bool ordered = false;
        };
      };
      struct spatial
      {
        static const float min_coord = -4.0f;
        static const float max_coord = 4.0f;
#ifdef RANDCOORDS
        static const bool rand_coords = true;
#else
        static const bool rand_coords = false;
#endif

        SFERES_ARRAY(int, layers, 2, 5, 14);

        SFERES_ARRAY(float, x,
                     -1, 1,
                     -2, -1, 0, 1, 2,
                     -3, -2.5, -2, -1.5, -1, -0.5, 0, 0.5, 1, 1.5, 2, 2.5, 3, 3.5
                     );
        SFERES_ARRAY(float, y,
                     0, 0,
                     1, 1, 1, 1, 1,
                     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2);
        SFERES_ARRAY(float, z,
                     0, 0,
                     1, 1, 1, 1, 1,
                     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
      };
      static const size_t nb_inputs = 2;
      static const size_t nb_outputs = 14;

       // slope of the sigmoid
      static const size_t lambda = 20;

      static const size_t min_nb_neurons = 10; //useless
      static const size_t max_nb_neurons = 30; //useless
      static const size_t min_nb_conns = 0; // ?? TODO
      static const size_t max_nb_conns = 120; // ?? TODO

      static const float m_rate_add_conn = 0.2f;
      static const float m_rate_del_conn = 0.2f;
       //static const float m_rate_change_conn = 0.15; //old default
      static const float m_rate_change_conn = 0.0; //new default?
      static const float m_rate_add_neuron = 0.0f;
      static const float m_rate_del_neuron = 0.0f;
       // average number of weigth changes
      static const float m_avg_weight = 2.0f;

      static const int io_param_evolving = true;
      static const init_t init = ff;

    };

    struct parameters
    {
#ifdef FLOAT
       // maximum value of parameters (weights & bias)
      static const float min = -2;
       // minimum value
      static const float max = 2;
#else
       // maximum value of parameters (weights & bias)
      static const float min = 0;
       // minimum value
      static const float max = 1;
#endif
    };

    struct stats
    {
       //static const size_t period = 1; //print stats every N generations
      static const size_t period = 1; //print stats every N generations

    };
  };
  )

SFERES_FITNESS(FitArmBall, sferes::fit::Fitness)
{
  public:
    static const float dmax = 500;
    static const size_t b_size = 14 * 4;
    template<typename Indiv>
    void eval(Indiv& indiv)
    {
      _values.resize(2);
      std::vector<float> inputs(2);
      float fit = 0;
      ArmEnv arm_env;

      indiv.nn().init();
      for (size_t e = 0; e < 4; ++e)
      {
        arm_env.set_env(e);
        inputs[0] = arm_env.ball_side() ? -1 : 1;
        inputs[1] = arm_env.ball_size() ? -1 : 1;

        for (size_t i = 0; i < indiv.gen().get_depth(); ++i)
          indiv.nn().step(inputs);
        std::vector<float> p = indiv.nn().get_outf();
        assert(p.size() == 14);
        std::vector<float> apos(7);
        for (size_t i = 0, j = 0; i < p.size(); i += 2, j += 1)
          apos[j] = this->_angle(p[i], p[i + 1]);
        for (size_t i = 0; i < p.size(); ++i)
          _behavior[i + e * 14] = p[i] > 0 ? 1 : 0;
        arm_env.arm().set_apos(apos);
        float d1 = fabs((arm_env.arm().finger1() - arm_env.ball_pos()).norm()
                        - arm_env.ball_radius());
        float d2 = fabs((arm_env.arm().finger2() - arm_env.ball_pos()).norm()
                        - arm_env.ball_radius());
        if (arm_env.arm().finger1() == arm_env.arm().finger2())
          fit -= 1e5;

        fit += 1 - (d1 + d2) / (2 * dmax);
        if (this->mode() == fit::mode::view)
          arm_env.save("env" + boost::lexical_cast<std::string>(e) + ".svg");
      }

      this->_objs.resize(1);
      this->_objs[0] = fit / 4.0;
      this->_value = this->_objs[0];
      this->_values[0] = this->_objs[0];

       // compute the modularity
      typename Indiv::nn_t::graph_t gsimp = indiv.gen().simplified_nn().get_graph();
      if (boost::num_edges(gsimp) == 0 || boost::num_vertices(gsimp) == 0)
        _mod = 0;
      else
        _mod = mod::modularity(gsimp);

#if defined(LENGTH) && defined(ONP)
      bool write_svg = (this->mode() == fit::mode::view);
      this->_objs.push_back(-indiv.gen().compute_optimal_length(0, write_svg));
#endif
#ifdef MOD
      this->_objs.push_back(_mod);
#endif
#ifdef DIV
      this->_objs.push_back(0);
#endif
    }
    template<typename Indiv>
    float dist(const Indiv& o) const
    {
      return (o.fit()._behavior ^ _behavior).count() / (float) _behavior.size();
    }

    float mod() const { return _mod; }
    float length() const { return -1; }
    float optlength() const { return -1; }
    int nb_values() const { return 0; }
    float values(size_t i) const {
      assert(i < _values.size());
      return _values[i];
    }
    const std::vector<float>& values() const { return _values; }

  protected:
    std::vector<float> _values;
    float _mod;
    std::bitset<b_size> _behavior;
    float _angle(float x1, float x2) const
    {
      if (x1 <= 0 && x2 <= 0)
        return -M_PI / 4.0;
      if (x1 <= 0 && x2 > 0)
        return -M_PI / 12.0;
      if (x1 > 0 && x2 <= 0)
        return M_PI / 12.0;
      if (x1 > 0 && x2 > 0)
        return M_PI / 4.0;
      std::cout << "x1:" << x1 << " x2:" << x2 << std::endl;
      assert(0);
      return 0;
    }
}
;


long Params::seed = -1;

int main(int argc, char **argv)
{
  typedef FitArmBall<Params> fit_t;
#ifdef FLOAT
  typedef phen::Parameters<gen::EvoFloat<1, Params::dnn::weights>,
                           fit::FitDummy<>, Params> weight_t;
  typedef phen::Parameters<gen::EvoFloat<1, Params::dnn::bias>,
                           fit::FitDummy<>, Params> bias_t;
#else
  typedef phen::Parameters<gen::Sampled<1, Params::dnn::weights>,
                           fit::FitDummy<>, Params> weight_t;
  typedef phen::Parameters<gen::Sampled<1, Params::dnn::bias>,
                           fit::FitDummy<>, Params> bias_t;

#endif
  typedef PfWSum<weight_t> pf_t;
  typedef AfRetina<bias_t> af_t;
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
#elif defined(AGE)
  typedef modif::Age<> modifier_t;
#else
  typedef modif::Dummy<> modifier_t;
#endif

#ifdef PNSGA
  typedef ea::Pnsga<phen_t, eval_t, stat_t, modifier_t, Params> ea_t;
#else
  typedef ea::Nsga2<phen_t, eval_t, stat_t, modifier_t, Params> ea_t;
#endif
  ea_t ea;

  if (argc == 2) // if a number is provided on the command line
  {
    int randomSeed = atoi(argv[1]);
    printf("randomSeed:%i\n", randomSeed);
    srand(randomSeed);  //set it as the random seed
    Params::seed = randomSeed;
    run_ea(argc, argv, ea, false);
  }
  else
    run_ea(argc, argv, ea, true);

  return 0;
}
