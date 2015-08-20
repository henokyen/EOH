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

#include "behavior_div.hpp"
#include "gen_spatial.hpp"
#include "stat_modularity.hpp"
#include "mvg.hpp"

using namespace sferes;
using namespace nn;
using namespace sferes::gen::evo_float;
using namespace sferes::gen::dnn;

#include <boost/algorithm/string.hpp>

#define SFERES_PARAMS(P)						\
  struct Ps__ { Ps__() {						\
    static std::string __params = #P;					\
    boost::replace_all(__params, ";", ";\n");				\
    boost::replace_all(__params, "{", "{\n");				\
    boost::replace_all(__params, "static const", "");	       		\
    std::cout<<"Parameters:"<<__params<<std::endl; } };			\
  P;									\
  static Ps__ ____p;

SFERES_PARAMS(
struct Params
{
  struct pop
  {
    static const unsigned size = 200;
    //    static const unsigned nb_gen = 25001;
    static const unsigned nb_gen = 100001;
    static const int dump_period = 10;//nb_gen-1;
    static const int initial_aleat = 1;

  };

  struct dnn
  {
    struct bias
    {
      struct evo_float
      {
	static const float cross_rate = 0.5f;
	static const float mutation_rate = 0.1f;
	static const float eta_m = 10.0f;
	static const float eta_c = 10.0f;
	static const mutation_t mutation_type = polynomial;
	static const cross_over_t cross_over_type = sbx;
      };
      struct sampled
      {
	SFERES_ARRAY(float, values, -2,-1, 0, 1, 2);
	static const float mutation_rate = 0.05f;
	static const float cross_rate = 0.5f;
	static const bool ordered = true;
      };
    };
    struct weight
    {
      struct evo_float
      {
	static const float cross_rate = 0.5f;
	static const float mutation_rate = 1.0f;
	static const float eta_m = 10.0f;
	static const float eta_c = 10.0f;
	static const mutation_t mutation_type = polynomial;
	static const cross_over_t cross_over_type = sbx;
      };
      struct sampled
      {
	SFERES_ARRAY(float, values, -2, -1,  1, 2);
	static const float mutation_rate = 1.0f;
	static const float cross_rate = 0.5f;
	static const bool ordered = true;
      };
    };

    static const size_t nb_inputs	= 4;
    static const size_t nb_outputs	= 1;

    static const size_t min_nb_neurons	= 10;//useless
    static const size_t max_nb_neurons	= 20;//useless
    static const size_t min_nb_conns	= 10;
    static const size_t max_nb_conns	= 50;

    static const float m_rate_add_conn	= 0.20f;
    static const float m_rate_del_conn	= 0.20f;
    static const float m_rate_change_conn = 0.0f;
    static const float m_rate_add_neuron  = 0.05f;
    static const float m_rate_del_neuron  = 0.05f;
    static const float m_avg_weight  = 2.0f;

    static const int io_param_evolving = true;
    static const init_t init = ff;
    struct spatial
    {
      SFERES_ARRAY(int, layers, 4, 4, 4, 2, 1);
      SFERES_ARRAY(float, x,
		   -2, -1, 1, 2,
		   -2, -1, 1, 2,
		   -2, -1, 1, 2,
		   -1, 1,
		   0);
      SFERES_ARRAY(float, y,
		   0, 0, 0, 0,
		   1, 1, 1, 1,
		   2, 2, 2, 2,
		   3, 3,
		   4);
      SFERES_ARRAY(float, z,
		   0, 0, 0, 0,
		   0, 0, 0, 0,
		   0, 0, 0, 0,
		   0, 0,
		   0);
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
    static const size_t period = 100;
  };
};
	      )// end of params


SFERES_FITNESS(FitXorXor, sferes::fit::Fitness)
{
public:
  FitXorXor()  {}

  template<typename Indiv>
    float dist(const Indiv& o) const
  {
    return (o.fit()._behavior ^ _behavior).count() / (float) _behavior.size();
  }
  // size of the behavior vector

  static const size_t b_size = 16;

  bool xor_and_xor(bool a, bool b, bool c, bool d)
  {
    return (a ^ b) && (c ^ d);
  }
  bool xor_or_xor(bool a, bool b, bool c, bool d)
  {
    return (a ^ b) || (c ^ d);
  }
   template<typename Indiv>
   void eval(Indiv& ind)
   {
    std::vector<float> inputs(4);
    float err1 = 0, err2 = 0;
    for (size_t s = 0; s < 16; ++s)
      {
	std::bitset<4> in(s);
	for (size_t i = 0; i < in.size(); ++i)
	  inputs[i] = in[i] ? 1 : -1;
	// step
	for (size_t i = 0; i < ind.gen().get_depth(); ++i)
	  ind.nn().step(inputs);
	float out = ind.nn().get_outf(0);
	bool res = out > 0;
	bool wanted_and = xor_and_xor(in[0], in[1], in[2], in[3]);
	bool wanted_or = xor_or_xor(in[0], in[1], in[2], in[3]);
	err1 += (wanted_and == res) ? 0 : 1;
	err2 += (wanted_or == res) ? 0 : 1;
	_behavior[s] = res;
      }
    this->_objs.clear();
#ifdef MVG
    this->_objs.push_back(-42);
#endif

    this->_values.resize(2);
    this->_values[0] =  1 - err1/ 16.0f;
    this->_values[1] =  1 - err2/ 16.0f;



#ifdef FG_AND
    this->_objs.push_back(1 - err1 / 16.0f);
#endif
    this->_value = 1 - err1 / 16.0f;

    // compute the modularity
    typename Indiv::nn_t nn = ind.nn();
    nn.simplify();
    _mod = mod::modularity(nn.get_graph());
#ifdef MOD
    this->_objs.push_back(_mod);
#endif

    _length = ind.gen().compute_length();
#ifdef LENGTH
    this->_objs.push_back(-_length);
#endif


#ifdef DIV
    this->_objs.push_back(0);
#endif

#ifdef RANDOBJ
    this->_objs.push_back(rand() / (float)RAND_MAX);
#endif


    if (this->mode() == sferes::fit::mode::view)
      {
	std::cout<<"behavior:"<<_behavior<<std::endl;
	std::ofstream ofs("/tmp/nn.dot");
	ind.nn().write(ofs);
	std::cout<<"AND:"<< 1 - err1 / 100.0f << std::endl;
	std::cout<<"OR:"<< 1 - err2 / 100.0f << std::endl;
	std::cout<<"MOD:"<< _mod <<std::endl;
	std::cout<<"length:"<<_length<<std::endl;
	std::cout<<"nb neurons:"<<ind.nn().get_nb_neurons()<<std::endl;
	// dump in nn.svg
	std::ofstream ofs2("nn.svg");
	ind.gen().write_svg(ofs2);

      }
  }
  void set_value(float v) { this->_value = v; }
  float values(size_t k) const { return _values[k]; }
  const std::bitset<b_size>& behavior() const { return _behavior; }
  float mod() const { return _mod; }
  float length() const { return _length; }
 protected:
  std::vector<float> _values;
  std::bitset<b_size> _behavior;
  float _mod;
  float _length;
};


int main(int argc, char **argv)
{
  typedef FitXorXor<Params> fit_t;
  typedef phen::Parameters<gen::Sampled<1, Params::dnn::weight>,
    fit::FitDummy<>, Params> weight_t;
  typedef phen::Parameters<gen::Sampled<1, Params::dnn::bias>,
			   fit::FitDummy<>, Params> bias_t;
  typedef PfWSum<weight_t> pf_t;
  typedef AfTanh<bias_t> af_t;
  typedef sferes::gen::DnnSpatial<Neuron<pf_t, af_t>,  Connection<weight_t>, Params> gen_t;
  typedef phen::Dnn<gen_t, fit_t, Params> phen_t;
  typedef eval::Parallel<Params> eval_t;
  typedef boost::fusion::vector
    <
  //stat::ParetoFront<phen_t, Params>, // comment this line to remove pareto.dat
    stat::Mod<phen_t, Params>
    >  stat_t;

  // MVG cannot be used with DIV !
#if defined(DIV) and !defined(MVG)
  typedef modif::BehaviorDiv<> modifier_t;
#elif defined(MVG)
  typedef modif::Mvg<> modifier_t;
#else
  typedef modif::Dummy<> modifier_t;
#endif

  typedef ea::Nsga2<phen_t, eval_t, stat_t, modifier_t, Params> ea_t;
  ea_t ea;

  if(argc>1) // if a number is provided on the command line
    {
      int randomSeed = atoi(argv[1]);
      printf("randomSeed:%i\n", randomSeed);
      srand(randomSeed);  //set it as the random seed
      run_ea(argc, argv, ea, false);
    }
  else
    {
      run_ea(argc, argv, ea);
    }


  return 0;
}
