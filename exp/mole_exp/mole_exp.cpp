// define this if you need to deactivate paralell processing
//#define NO_PARALLEL

#include <sferes/phen/parameters.hpp>
#include <sferes/gen/evo_float.hpp>
#include <sferes/phen/parameters.hpp>
#include <sferes/ea/nsga2.hpp>
#include <sferes/eval/eval.hpp>
#include <sferes/stat/pareto_front.hpp>
#include <sferes/stat/best_fit.hpp>
#include <sferes/run.hpp>
#include "modif_novmap.hpp"
#include "stat_archive.hpp"

MOLE_FITNESS(MyMole)
{
  public:
  template<typename Indiv>
    void eval(Indiv& ind)
  {
    // PUT YOUR CODE HERE
    // compute the fitness (this->_value)
    // optimized parameters are in ind.data(i)
    // (functions are _maximised_)
      std::vector<float> inputs(Params::nb_inputs);
      int k = (int)pow(2, Params::nb_inputs);
      float err1 = 0, err2 = 0, nba = 0;
      for (size_t s = 0; s < k; ++s)
      {
          //set inputs
         std::bitset<Params::nb_inputs> in(s);
         for (size_t i = 0; i < in.size(); ++i)
            inputs[i] = in[i] ? 1 : -1;
           //step
         for (size_t i = 0; i < ind.gen().get_depth(); ++i)
            ind.nn().step(inputs);
            //behaviors
          std::vector<bool> xorand_res;
         for (size_t i = 0; i < Params::nb_inputs; i+=2)
          xorand_res.push_back(((bool)(inputs[i] > 0)) && ((bool)(inputs[i + 1] > 0)));
    
          xorand_res.push_back(xorand_res[0] ^ xorand_res[1]);
          xorand_res.push_back(xorand_res[2] ^ xorand_res[3]);
         if (xorand_res[4] && xorand_res[5])
          err1 += ind.nn().get_outf(0) > 0 ? 0 : 1;
        else
          err1 += ind.nn().get_outf(0) > 0 ? 1 : 0;
     }

 float norm = k * Params::nb_outputs;
      this->_values = 1 - err1 / norm;

    // set descriptors (must be in [0-1])
    // e.g. 
    this->set_desc(ind.gen().data(0), ind.gen().data(1));
  }
};


using namespace sferes;
using namespace sferes::gen::evo_float;

// Here you can tune the parameters of the algorithm
struct Params
{
  static long seed;
  struct pop
  {
    static const unsigned size = 1000;
    static const unsigned nb_gen = 10001;
    static const int dump_period = nb_gen-1;
    static const int initial_aleat = 1;
  };
  struct parameters
  {
    static const float min = -5;
    static const float max = 5;
  };
  struct evo_float
  {
    static const float cross_rate = 0.25f;
    static const float mutation_rate = 0.1f;
    static const float eta_m = 10.0f;
    static const float eta_c = 10.0f;
    static const mutation_t mutation_type = polynomial;
    static const cross_over_t cross_over_type = sbx;
  };

static const size_t nb_outputs = 1;
static const size_t nb_inputs = 8;  
};


long Params::seed = -1;

int main(int argc, char **argv)
{
  typedef MyMole<Params> fit_t;
  typedef gen::EvoFloat<10, Params> gen_t;
  typedef phen::Parameters<gen_t, fit_t, Params> phen_t;
  typedef eval::Parallel<Params> eval_t;
  typedef boost::fusion::vector<
  stat::Archive<phen_t, Params>    
    >  stat_t;
  typedef modif::Novmap<phen_t, Params> modifier_t;
  typedef ea::Nsga2<phen_t, eval_t, stat_t, modifier_t, Params> ea_t;
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
  {
    run_ea(argc, argv, ea);
  }

  return 0;
}
