#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE hnn_3d

#include <iostream>
#include <boost/test/unit_test.hpp>
#include <boost/timer.hpp>
#include <sferes/fit/fitness.hpp>
#include <sferes/gen/evo_float.hpp>
#include <sferes/phen/parameters.hpp>

#include "gen_hnn_3d.hpp"
#include "phen_hnn_3d.hpp"

using namespace sferes;
using namespace sferes::gen::dnn;
using namespace sferes::gen::evo_float;

struct Params
{
  struct evo_float
  {
    static const float mutation_rate = 0.1f;
    static const float cross_rate = 0.1f;
    static const mutation_t mutation_type = polynomial;
    static const cross_over_t cross_over_type = sbx;
    static const float eta_m = 15.0f;
    static const float eta_c = 15.0f;
  };
  struct parameters
  {
     // maximum value of parameters
    static const float min = -2.0f;
     // minimum value
    static const float max = 2.0f;
  };

  struct cppn
  {
     // params of the CPPN
    struct sampled
    {
      SFERES_ARRAY(float, values, 0, 1, 2);
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
  struct hnn
  {
    static const size_t nb_rand_mutations = 0;
    static const size_t nb_layers = 3;
    SFERES_ARRAY(float, layer_dim,
                 20, 16, 11, // input layer
                 20, 16, 1, // hidden layer
                 2, 1, 1); // output layer
    typedef float weight_t;
    typedef float io_t;
    typedef nn::params::Vectorf<1> params_t;
    typedef nn::Neuron<nn::PfWSum<io_t>,
                       nn::AfTanh<params_t>, io_t > neuron_t;
    typedef nn::Connection<weight_t, io_t> connection_t;
  };
};


BOOST_AUTO_TEST_CASE(gen_hnn_3d)
{
  srand(time(0));
  typedef phen::Parameters<gen::EvoFloat<1, Params>, fit::FitDummy<>, Params> weight_t;
  typedef gen::Hnn3d<weight_t, Params, Params> gen_t;
  gen_t gen1, gen2, gen3, gen4;

  gen1.random();
  for (size_t i = 0; i < 20; ++i)
    gen1.mutate();
  gen1.init();
}

BOOST_AUTO_TEST_CASE(gen_hnn_3d_seed)
{
  srand(time(0));
  typedef phen::Parameters<gen::EvoFloat<1, Params>, fit::FitDummy<>, Params> weight_t;
  typedef gen::Hnn3d<weight_t, Params, Params> gen_t;
  gen_t gen1;

  gen1.random();
  gen1.init();
  std::ofstream ofs2("/tmp/seed.dot");
  gen1.cppn().write(ofs2);

}

//
BOOST_AUTO_TEST_CASE(phen_hnn_3d)
{
  srand(time(0));
  typedef phen::Parameters<gen::EvoFloat<1, Params>, fit::FitDummy<>, Params> weight_t;
  typedef gen::Hnn3d<weight_t, Params, Params> gen_t;
  typedef phen::Hnn3d<gen_t, fit::FitDummy<>, Params> phen_t;

  phen_t indiv;
  indiv.random();
  for (size_t i = 0; i < 50; ++i)
    indiv.mutate();
  boost::timer timer;
  indiv.develop();
  std::cout << "develop time:" << timer.elapsed() << std::endl;
  timer.restart();

  indiv.nn().init();

  std::vector<float> in(indiv.nn().get_nb_inputs());
  for (size_t i = 0; i < in.size(); ++i)
    in[i] = sferes::misc::rand<float>(-1, 1);
  for (size_t i = 0; i < 100; ++i)
    indiv.nn().step(in);
  std::cout << "running time:(100 it)" << timer.elapsed() << std::endl;

  std::ofstream ofs("/tmp/nn.dot");
  indiv.nn().write(ofs);
  std::ofstream ofs2("/tmp/nn_gen.dot");
  indiv.gen().cppn().write(ofs2);
}
