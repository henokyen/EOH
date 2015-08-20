#define NO_PARALLEL
#include <iostream>
#include <bitset>
#include <boost/assign/list_of.hpp>
#include <boost/random.hpp>

#include <sferes/phen/parameters.hpp>
#include <sferes/gen/evo_float.hpp>
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

using namespace sferes; 
using namespace nn;
using namespace sferes::gen::evo_float;
using namespace sferes::gen::dnn;
struct Params
{
  struct pop
  {
    static const unsigned size = 200;
    static const unsigned nb_gen = 25001;
    //static const unsigned nb_gen = 25001;
    static const int dump_period = 1000;
    static const int initial_aleat = 1;
  };
  
  struct dnn
  {
    struct bias
    {
      struct evo_float
      {
	static const float cross_rate = 0.5f;
	static const float mutation_rate = 1.0 / (25 + 36 + 18);
	static const float eta_m = 10.0f;
	static const float eta_c = 10.0f;
	static const mutation_t mutation_type = polynomial;
	static const cross_over_t cross_over_type = sbx;
      };
    };
    struct weight
    {
      struct evo_float
      {
	static const float cross_rate = 0.5f;
	static const float mutation_rate = 1.0;
	static const float eta_m = 10.0f;
	static const float eta_c = 10.0f;
	static const mutation_t mutation_type = polynomial;
	static const cross_over_t cross_over_type = sbx;
      };
    };

    static const size_t nb_inputs	= 25;
    static const size_t nb_outputs	= 18;

    static const size_t min_nb_neurons	= 10;//useless
    static const size_t max_nb_neurons	= 30;//useless
    static const size_t min_nb_conns	= 33;
    static const size_t max_nb_conns	= 70;

    static const float m_rate_add_conn	= 0.10f;
    static const float m_rate_del_conn	= 0.10f;
    static const float m_rate_change_conn = 0.0f;
    static const float m_rate_add_neuron  = 0.05f;
    static const float m_rate_del_neuron  = 0.05f;
    static const float m_avg_weight = 2.0f;

    static const int io_param_evolving = true;
    static const init_t init = ff;
    struct spatial
    {

      SFERES_ARRAY(int, layers, 25, 36, 18); // one hiden layer
      SFERES_ARRAY(float, x, 
		   // inputs (25)
		   -3, -1.5, 0, 1.5, 3,
		   -3, -1.5, 0, 1.5, 3,
		   -3, -1.5, 0, 1.5, 3,
		   -3, -1.5, 0, 1.5, 3,
		   -3, -1.5, 0, 1.5, 3,
		   // hidden layer (36)
		   -3.0,-2.83333333333,-2.66666666667,-2.5,-2.33333333333,-2.16666666667,-2.0,-1.83333333333,-1.66666666667,-1.5,-1.33333333333,-1.16666666667,-1.0,-0.833333333333,-0.666666666667,-0.5,-0.333333333333,-0.166666666667,0.0,0.166666666667,0.333333333333,0.5,0.666666666667,0.833333333333,1.0,1.16666666667,1.33333333333,1.5,1.66666666667,1.83333333333,2.0,2.16666666667,2.33333333333,2.5,2.66666666667,2.83333333333,
		   // output (18)
		   -3.0, -2.875, -2.75, -2.625, -2.5, -2.375, -2.25, -2.125, -2.0, 
		   2.0, 2.125, 2.25, 2.375, 2.5, 2.625, 2.75, 2.875, 3.0,
		   );
      SFERES_ARRAY(float, y,
		   // inputs (25)
		   -3, -3, -3, -3, -3
		   -2, -2, -2, -2, -2,
		   -1, -1, -1, -1, -1,
		   0, 0, 0, 0, 0,
		   1, 1, 1, 1, 1,
		   2, 2, 2, 2, 2,
		   3, 3, 3, 3, 3,
		   // hidden layer (36)
		   0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
		   // output (18)
		   0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0
		   );
      SFERES_ARRAY(float, z,
		   // inputs (25)
		   0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
		   // hidden (36)
		   0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
		   // output (18)
		   0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0
		   );
    };
  };

  struct parameters
  {
    // maximum value of parameters (weights & bias)
    static const float min = -5;
    // minimum value
    static const float max = 5;
  };
  struct stats
  {
    static const size_t period = 100;
  };
};



SFERES_FITNESS(FitWhatWhere, sferes::fit::Fitness)
{ 
 public:
  FitWhatWhere()  {}

  template<typename Indiv>
    float dist(const Indiv& o) const 
  {
    return (o.fit()._behavior ^ _behavior).count() / (float) _behavior.size();   
  }

  static const size_t b_size = 9 * 9 * 18;


  template<typename C>
    void display_mat(const C& c, size_t nrows, size_t ncols)
  {
    for (size_t i = 0; i < nrows; ++i)
      {
	for (size_t j = 0; j < ncols; ++j)
	  std::cout << c[i * ncols + j] << " ";
	std::cout << std::endl;
      }
  }
  
  template<typename Indiv>
    void eval(Indiv& ind) 
  {
    _create_objects();
    float err = 0;

    std::vector<float> inputs(25);
    for (size_t i = 0; i < _objects.size(); ++i)
      for (size_t j = 0; j < 9; ++j)
	{
	  size_t row = j / 3;
	  size_t col = j % 3;
	  // fill the retina
	  std::fill(inputs.begin(), inputs.end(), 0.0f);
	  display_mat(_objects[i], 3, 3);
	  for (size_t r = 0; r < 3; ++r)
	    for (size_t c = 0; c < 3; ++c)
	      inputs[(row + r) * 5 + (col + c)] = _objects[i][r * 3 + c];
	  display_mat(inputs, 5, 5);
	  // eval nn
	  //	  ind.nn().init(); // ?
	  for (size_t s = 0; s < ind.gen().get_depth(); ++s)
	    ind.nn().step(inputs);
	  // compute error
	  const std::vector<float>& o = ind.nn().get_outf();
	  // what
	  for (size_t k = 0; k < 9; ++k)
	    if (k == i)
	      err += (o[k] > 0.9 ? 0 : 1);
	    else
	      err += (o[k] < 0.1 ? 0 : 1);
	  // where
	  for (size_t k = 0; k < 9; ++k)
	    if (k == j)
	      err += (o[k + 9] > 0.9 ? 0 : 1);
	    else
	      err += (o[k + 9] < 0.1 ? 0 : 1);
	}

    this->_objs.clear();
    this->_value =  1 - err / b_size;
    
#ifdef FG
    this->_objs.push_back(this->_value);
#endif
    
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

    if (this->mode() == sferes::fit::mode::view)
      {
	std::cout<<"behavior:"<<_behavior<<std::endl;
	std::ofstream ofs("/tmp/nn.dot");
	ind.nn().write(ofs);
	// dump in nn.svg
	std::ofstream ofs2("nn.svg");
	ind.gen().write_svg(ofs2);
      }
  }
  const std::bitset<b_size>& behavior() const { return _behavior; }
  float mod() const { return _mod; }
  float length() const { return _length; }
 protected:
  std::vector<std::bitset<9> > _objects;
  std::bitset<b_size> _behavior;

  float _mod;
  float _length;
  std::bitset<9> _object(bool x0, bool x1, bool x2,
			 bool x3, bool x4, bool x5,
			 bool x6, bool x7, bool x8)
  {
    std::bitset<9> b;
    b[0] = x0; b[1] = x1; b[2] = x2;
    b[3] = x3; b[4] = x4; b[5] = x5;
    b[6] = x6; b[7] = x7; b[8] = x8;
    return b;
  }
  void _create_objects()
  {
    // from Rueckl et al
    _objects.push_back(_object(1, 1, 1,
			       0, 1, 0,
			       0, 1, 0));
    _objects.push_back(_object(1, 0, 1,
			       1, 1, 0,
			       1, 0, 0));
    _objects.push_back(_object(0, 1, 0,
			       1, 1, 1,
			       0, 1, 0));
    _objects.push_back(_object(0, 0, 1,
			       0, 0, 1,
			       1, 1, 1));
    _objects.push_back(_object(1, 1, 0,
			       0, 1, 0,
			       0, 1, 1));
    _objects.push_back(_object(1, 0, 1,
			       0, 1, 0,
			       1, 0, 1));
    _objects.push_back(_object(0, 1, 0,
			       1, 0, 1,
			       1, 0, 1));
    _objects.push_back(_object(1, 0, 1,
			       1, 1, 1,
			       0, 0, 0));
    _objects.push_back(_object(0, 1, 1,
			       0, 0, 0,
			       1, 1, 1));
  }
};


int main(int argc, char **argv)
{
  typedef FitWhatWhere<Params> fit_t;
  typedef phen::Parameters<gen::EvoFloat<1, Params::dnn::weight>, 
    fit::FitDummy<>, Params> weight_t;
  typedef phen::Parameters<gen::EvoFloat<1, Params::dnn::bias>, 
    fit::FitDummy<>, Params> bias_t;
  typedef PfWSum<weight_t> pf_t;
  typedef AfSigmoidBias<bias_t> af_t;
  typedef sferes::gen::DnnSpatial<Neuron<pf_t, af_t>,  Connection<weight_t>, Params> gen_t;
  typedef phen::Dnn<gen_t, fit_t, Params> phen_t;
  typedef eval::Parallel<Params> eval_t;
  typedef boost::fusion::vector
    <
  stat::ParetoFront<phen_t, Params> // comment this line to remove pareto.dat
    //    stat::Mod<phen_t, Params>
    >  stat_t;

  // MVG cannot be used with DIV !
#ifdef DIV
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
