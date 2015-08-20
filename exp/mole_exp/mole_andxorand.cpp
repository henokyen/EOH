#include <iostream>
#include <bitset>
#include <algorithm>
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
#include <modules/modularity/hierarchy.hpp>
#include <string>
#include <iostream>
#include "exp/modularity/behavior_div.hpp"

#ifdef PNSGA
   #include "exp/modularity/pnsga.hpp"
#endif
#include "exp/modularity/gen_spatial.hpp"
#include "exp/modularity/modif_novmap.hpp"
//#include "modif_novmap.hpp"
#include "exp/modularity/stat_modularity.hpp"
//#include "stat_archive.hpp"
#include "exp/modularity/mvg.hpp"
#include "modules/nn2/trait.hpp"
using namespace sferes;
using namespace nn;
using namespace sferes::gen::evo_float;
using namespace sferes::gen::dnn;


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
SFERES_PARAMS(

struct Params
  {
    static long int seed;
    struct pop
    {
      static const unsigned size = 1000;
     // static const unsigned nb_gen = 5001;
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
#elif defined(PR100)
      SFERES_ARRAY(float, obj_pressure, 1.0, 1.0, 1.0);
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
          static const float mutation_rate = 1.0 / 20.0f;
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


      static const size_t nb_outputs = 1;
      static const size_t nb_inputs = 8;

      static const size_t min_nb_neurons = 10; //useless
      static const size_t max_nb_neurons = 30; //useless

      static const size_t min_nb_conns = 20;
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
        SFERES_ARRAY(int, layers, 8, 4, 4, 2, 1);
        SFERES_ARRAY(float, x,
                     -3.5, -2.5, -1.5, -0.5, 0.5, 1.5, 2.5, 3.5,
                     -3.0,-1.0,1.0,3.0,
                     -3.0,-1.0,1.0,3.0,
                     -2.0, 2.0,
                      0,
                     );
        SFERES_ARRAY(float, y,
                     0, 0, 0, 0, 0, 0, 0, 0,
                     1, 1, 1, 1,
                     2, 2, 2, 2,
                     3, 3,
		     4
                     );
        SFERES_ARRAY(float, z,
                     0, 0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0,
		     0, 0,
                     2);
        static const float min_coord = -6.0f;
        static const float max_coord = 6.0f;
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
       static const size_t b_size = 256 * Params::dnn::nb_outputs;
        };
 };
    )   
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

bool sort_pred (std::pair<bool,int> left, std::pair<bool,int> right)
    {
        return left.second > right.second;
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
         // set inputs
         std::bitset<Params::dnn::nb_inputs> in(s);
         for (size_t i = 0; i < in.size(); ++i)
            inputs[i] = in[i] ? 1 : -1;
             // step
         for (size_t i = 0; i < ind.gen().get_depth(); ++i)
            ind.nn().step(inputs);
            // behaviors
          std::vector<bool> xorand_res;
         for (size_t i = 0; i < Params::dnn::nb_inputs; i+=2)
          xorand_res.push_back(((bool)(inputs[i] > 0)) && ((bool)(inputs[i + 1] > 0)));
         for (size_t i = 0; i < Params::dnn::nb_outputs; ++i) 
            _behavior[s * Params::dnn::nb_outputs + i] = ind.nn().get_outf(i) > 0 ? 1 : 0;
             // errors
          xorand_res.push_back(xorand_res[0] ^ xorand_res[1]);
          xorand_res.push_back(xorand_res[2] ^ xorand_res[3]);
         if (xorand_res[4] && xorand_res[5])
          err1 += ind.nn().get_outf(0) > 0 ? 0 : 1;
        else
          err1 += ind.nn().get_outf(0) > 0 ? 1 : 0;
       }
      this->_objs.clear();

#ifdef MVG
      this->_objs.push_back(-42); // this will be set later
#endif
      float norm = k * Params::dnn::nb_outputs;
      this->_values.resize(2);
      this->_values[0] = 1 - err1 / norm;
      //this->_values[1] = 1 - err2 / norm;

#ifdef FG
      this->_objs.push_back(1 - err1 / norm);
#endif
      this->_value = 1 - err1 / norm;

      typename Indiv::nn_t::graph_t gsimp =ind.gen().simplified_nn().get_graph();

      if (boost::num_edges(gsimp) == 0 || boost::num_vertices(gsimp) == 0)
       {
        _mod = 0.0;
        _grc = 0.0;
       }
      else
       {
        _mod = mod::modularity(gsimp);
        _grc = hierar::Get_GRC(gsimp);

       }


#ifdef MOD
      this->_objs.push_back(_mod);
#endif
#ifdef HE
      this->_objs.push_back(_grc);
#endif
      // length
      _length = ind.gen().compute_length();
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


#if defined(LENGTH) && !defined(ONP)
      this->_objs.push_back(-_length);
#endif
      //optimal length
      bool write_svg = (this->mode() == fit::mode::view);
     std::string fname="/project/evolutionaryRoboticss/hmengist/newmega/3hxorand3hxor/PA/log/nn"+_postFix+".svg";
      _optlength = ind.gen().compute_optimal_length(0, write_svg,fname);
     //_optlength=_length;
#if defined(LENGTH) && defined(ONP)
      this->_objs.push_back(-_optlength);
#endif

#ifdef TLENGTH
      this->_objs.push_back(-ind.gen().compute_length(1.5)); //default
#endif


#if defined(DIV) || defined(NOV) || defined(NOVMAP)
      this->_objs.push_back(0);
#endif


#ifdef RANDOBJ
      this->_objs.push_back(rand() / (float)RAND_MAX);
#endif

      if (this->mode() == sferes::fit::mode::view)
      {
        //std::cout << "behavior:" << _behavior << std::endl;
        //std::ofstream ofs("/project/evolutionaryRobotics/hmengist/mega/3hxor/PA/log/nn"+_postFix+".dot");
      //if ((_grc >=0.75000)&&(1-err1/norm)==1)
         //recording(ind);
      //else
	//{
	//std::fstream fsol;
        //fsol.open("/project/evolutionaryRobotics/hmengist/7.25/xorands/PA/solvent.txt",std::fstream::out | std::fstream::app);
        //fsol <<"nn"+_postFix<<" "<<0<<"\n";
        //fsol.close();
//	}
        //recording(ind);
       
        //ind.nn().write(ofs);
        std::cout << "- FIT:" << 1 - err1 / norm << std::endl;
        std::cout << "- MOD:" << _mod << std::endl;
        std::cout << "- GRC:" << _grc << std::endl;
        std::cout << "length:" << _length << std::endl;
        std::cout << "nb neurons:" << ind.nn().get_nb_neurons() << std::endl;

      }
  
   //optimal length
      //bool write_svg = (this->mode() == fit::mode::view);
      //std::string fname="/project/evolutionaryRobotics/hmengist/7.25/andxorand/PH100/nn"+_postFix+".svg";
      //_optlength = ind.gen().compute_optimal_length(0, write_svg,fname,(1-err1/norm));
   //getlayer(ind);
   }
    void set_value(float v) { this->_value = v; }
    float values(size_t k) const { return _values[k]; }
    size_t nb_values() const { return _values.size(); }
    const behavior_t& behavior() const { return _behavior; }
    float mod() const { return _mod; }
    float length() const { return _length; }
    float optlength() const { return _optlength; }
    int nb_conns() const { return _nb_conns; }
    int nb_nodes() const { return _nb_nodes; }
    double get_grc() const { return _grc;}
    void setPostFix(std::string postFix){_postFix = postFix;}
  protected:
    std::vector<float> _values;
    behavior_t _behavior;
    float _mod;
    float _length, _optlength;
    int _nb_nodes, _nb_conns;
    double _grc;
    std::string _postFix;

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
  typedef boost::fusion::vector< stat::Mod<phen_t,Params> > stat_t; 
    
  //typedef boost::fusion::vector< stat::Archive<phen_t,Params> > stat_t;
 
  typedef modif::Novmap<phen_t, Params> modifier_t;
  typedef ea::Nsga2<phen_t, eval_t, stat_t, modifier_t, Params> ea_t;
  ea_t ea;

  if (argc > 1) // if a number is provided on the command line
  {
    int randomSeed = atoi(argv[1]);
    cout<<"randomSeed:"<<randomSeed<<endl;
    srand(randomSeed);  //set it as the random seed
    Params::seed = randomSeed;
    run_ea(argc, argv, ea, false);
  }
  else
  {
    cout<<"without random seed";
    run_ea(argc, argv, ea);
  }


  return 0;
}
