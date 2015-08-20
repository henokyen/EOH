#include <iostream>
#include <sstream>
#include <bitset>
#include <boost/dynamic_bitset.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/random.hpp>
#include <boost/lexical_cast.hpp>
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
#include "behavior_div.hpp"
#include "gen_spatial.hpp"
#include "mvg.hpp"
#include "modularity.hpp"
#include "mod2.hpp"

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

namespace global
{
  int id = 0;
  std::ofstream log_file("/project/evolutionaryRobotics/hmengist/7.25/sampling/hier_mod_sampling_noweight.dat");                                                    
}

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


SFERES_PARAMS(
  struct Params
  {
 struct visualisation
    {
      static const size_t  max_modularity_split = 3;
    };

    static long seed;
    struct rand_obj
    {
      static const float std_dev = 0.1;
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
          SFERES_ARRAY(float, values, -2,-1,1,2); //???-2??
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
          static const bool ordered = true;// it was fasle before
        };
      };

      static const size_t nb_inputs = 8;
      static const size_t nb_outputs = 1;

       // slope of the sigmoid
      static const size_t lambda = 20;

      static const size_t min_nb_neurons = 10; //useless
      static const size_t max_nb_neurons = 30; //useless
      static const size_t min_nb_conns = 12; // ?? TODO
      static const size_t max_nb_conns = 100; // ?? TODO

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
      struct spatial
      {
        static const float min_coord = -6.0f;
        static const float max_coord = 6.0f;

        static const bool rand_coords = false;
         //wider separation (best with tlength of 3.5) // new default
        SFERES_ARRAY(int, layers, 8, 4, 4, 2, 1);
        //SFERES_ARRAY(int, min_layers, 8, 4, 2, 1, 1);
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
      };
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
int maxv = -1;
SFERES_FITNESS(FitXorXor, sferes::fit::Fitness)
{
  public:
    FitXorXor() : _age(0){}
template  <typename T>
  string NumberToString ( T Number )
  {
     ostringstream ss;
     ss << Number;
     return ss.str();
  }

template<typename Indiv>
float dist(const Indiv& o) const
{
  return (o.fit()._behavior ^ _behavior).count() / (float) _behavior.size();
}
template<typename Indiv>
int behavoiral_dist(const Indiv* o) const
{
	for (int i = 0;i < o->fit()._behavior.size(); ++i)
				std::cout << o->fit()._behavior[i];
	std::cout<<" \n ############ \n";
	for (int i = 0;i < _behavior.size(); ++i)
		        std::cout << _behavior[i];
	std::cout<<" \n ############ \n";
	//std::cout << "foo.size() is " << _behavior.size() << '\n';
	//std::cout << "foo.size() is " << o->fit()._behavior.size() << '\n';
	return (o->fit()._behavior ^ _behavior).count() ;/// (float) _behavior.size();
}
template <typename Indiv, size_t nb_bits>
void off_behavoir(Indiv* ind, std::bitset<nb_bits>& behavior)
{
   //vector<Indiv> offspring;
	//std::cout<<"you here ? \n";
	//typename Indiv::nn_t copied_nn = ind.gen().unsimplified_nn();
   std::vector<float> inputs(Params::dnn::nb_inputs);
   int k = (int)pow(2, Params::dnn::nb_inputs);

   for (size_t s = 0; s < k; ++s)
    {
       std::bitset<Params::dnn::nb_inputs> in(s);
       for (size_t i = 0; i < in.size(); ++i)
           inputs[i] = in[i] ? 1 : -1;
          //step
          for (size_t i = 0; i < ind->gen().get_depth(); ++i)
  	      ind->nn().step(inputs);
          //behaviors
          for (size_t i = 0; i < Params::dnn::nb_outputs; ++i){
            behavior[s * Params::dnn::nb_outputs + i] = ind->nn().get_outf(i) > 0 ? 1 : 0;
             //std::cout<<_behavior[i];
          }
    }
   //std::cout<<"\n %%%%%%% \n";
}
    static const size_t b_size = 256;
    template<typename Indiv>
    void eval(Indiv& ind)
    {
    	//std::cout<<"you here ? \n";
    	//typename Indiv::nn_t copied_nn = ind.gen().unsimplified_nn();
      std::vector<float> inputs(Params::dnn::nb_inputs);
      int k = (int)pow(2, Params::dnn::nb_inputs);
      float err1 = 0, err2 = 0, nba = 0;
      for (size_t s = 0; s < k; ++s)
      {
         std::bitset<Params::dnn::nb_inputs> in(s);
         for (size_t i = 0; i < in.size(); ++i)
            inputs[i] = in[i] ? 1 : -1;
             //step
         for (size_t i = 0; i < ind.gen().get_depth(); ++i)
        	 ind.nn().step(inputs);
            //behaviors
          std::vector<bool> xorand_res;
         for (size_t i = 0; i < Params::dnn::nb_inputs; i+=2)
          xorand_res.push_back(((bool)(inputs[i] > 0)) && ((bool)(inputs[i + 1] > 0)));
         for (size_t i = 0; i < Params::dnn::nb_outputs; ++i)
            _behavior[s * Params::dnn::nb_outputs + i] = ind.nn().get_outf(i) > 0 ? 1 : 0;
             //errors
          xorand_res.push_back(xorand_res[0] ^ xorand_res[1]);
          xorand_res.push_back(xorand_res[2] ^ xorand_res[3]);
         if (xorand_res[4] && xorand_res[5])
          err1 += ind.nn().get_outf(0) > 0 ? 0 : 1;
        else
          err1 += ind.nn().get_outf(0) > 0 ? 1 : 0;
      }
      typename Indiv::nn_t::graph_t gsimp = ind.gen().simplified_nn().get_graph();
      if (boost::num_edges(gsimp) == 0 || boost::num_vertices(gsimp) == 0){
        _mod = 0.0;
        _grc  = 0.0;
       }
      else
       {
        _mod = mod::modularity(gsimp);
        _grc = hierar::Get_GRC(gsimp);
       }
       this->set_mode(sferes::fit::mode::view);
       bool write_svg = (this->mode() == sferes::fit::mode::view);
       //cout<<"value of write_svg"<<write_svg<<endl;
       std::string fname="/project/evolutionaryRobotics/hmengist/7.25/sampling/nn.svg";
      _optlength = ind.gen().compute_optimal_length(0,false,fname,0);// just find optlength only
      global::log_file << ind.gen().get_nb_connections() <<" "<<_optlength<<" "<< _mod<<" "<< _grc<<std::endl;
     // std::cout << " " << _optlength
       //         << " " << _mod
         //       << " " << _mod
           //     << " " << ind.gen().get_nb_connections()
             //   << std::endl;
      global::id += 1;
      //std::cout<<":  "<<global::id<<std::endl;

	    
 if ((_grc > 0.75 && _mod < 0.2)||
     (_grc > 0.75 && _mod > 0.4)||
     (_grc < 0.7 && _mod < 0.2)||
     (_grc < 0.7 && _mod > 0.3))
  //if ((_grc <= 0.35 && _mod >= 0.4) || (_grc <= 0.3 && _mod <= 0.1)) 
    {
//std::cout<<"Begining if statment"<<std::endl;
string fname ="/project/evolutionaryRobotics/hmengist/7.25/sampling/"+NumberToString(_grc)+"_"+NumberToString(_mod)+"_nn.svg";
  // bool write_svg =true;
  bool write_svg = (this->mode() == sferes::fit::mode::view);
  //std::cout<<write_svg<<std::endl;
  float _optlength = ind.gen().compute_optimal_length(0,write_svg,fname,0);
  //std::ofstream ofs(fname.c_str());
  //ind.gen().write_svg(gsimp,ofs);
  //std::cout<<"if statement svg"<<std::endl;
 
string fdot="/project/evolutionaryRobotics/hmengist/7.25/sampling/"+NumberToString(_grc)+"_"+NumberToString(_mod)+"_nn.dot";   
std::ofstream ofs(fdot.c_str());
 ind.nn().write(ofs);
//std::cout<<"after dot"<<std::endl;
  }
    }
    float values(size_t k) const {
      return _values[k];
    }
    size_t nb_values() const { return _values.size(); }
    void set_value(float v) { this->_value = v; }
    const std::bitset<b_size>& behavior() const { return _behavior; }
    float mod() const { return _mod; }
    float length() const { return _length; }
    float optlength() const { return _optlength; }
    double get_grc() const { return _grc;}

  protected:
    std::vector<float> _values;
    std::vector<size_t> _left_objects;
    std::vector<size_t> _right_objects;
    std::bitset<b_size> _behavior;
    int _age;
    double _grc;



    float _mod, _mod2;
    float _length, _optlength;

};
;

long Params::seed = 1000;

int main(int argc, char **argv)
{
	int count=0;
	int count_valid=0;
	//std::cout<<"valid:  "<<count_valid<<" not valid:  "<<count<<std::endl;
  typedef FitXorXor<Params> fit_t;
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
  typedef AfTanhNxor<bias_t> af_t;
  typedef sferes::gen::DnnSpatial<Neuron<pf_t, af_t>, Connection<weight_t>, Params> gen_t;
  typedef phen::Dnn<gen_t, fit_t, Params> phen_t;

  //assert(argc == 2);
 Params::seed = boost::lexical_cast<int>(argv[1]);
 srand(Params::seed);

 for (size_t i = 0; i < 100 ; ++i)
    for (size_t k = 11; k <=11 ; ++k)
   {
     phen_t p;
     p.gen().random(k);
     if(p.gen().is_validnetwork())
       {
        count++;
       // std::cout<<"not valid:  "<<count<<std::endl;
        continue;
       }
   else
       {
         count_valid++;
         //std::cout<<"valid:  "<<count_valid<<std::endl;
       }
     //phen_t copp=p;
     //p.develop();

     p.evolvbility(p);
     //p.fit().eval(p);
     //p.fit().evolvability(p);
   }
//std::cout<<"valid:  "<<count_valid<<" not valid:  "<<count<<std::endl;
  return 0;
}
