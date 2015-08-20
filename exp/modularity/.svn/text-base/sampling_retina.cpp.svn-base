#include <iostream>
#include <bitset>
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
#include <modules/modularity/modularity.hpp>

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
  std::ofstream log_file("perfect_mod_length.dat");
}


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

      static const size_t nb_inputs = 8;
      static const size_t nb_outputs = 1;

       // slope of the sigmoid
      static const size_t lambda = 20;

      static const size_t min_nb_neurons = 10; //useless
      static const size_t max_nb_neurons = 30; //useless
      static const size_t min_nb_conns = 12; // ?? TODO
      static const size_t max_nb_conns = 26; // ?? TODO

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
        static const float min_coord = -4.0f;
        static const float max_coord = 4.0f;

        static const bool rand_coords = false;
         //wider separation (best with tlength of 3.5) // new default
        SFERES_ARRAY(int, layers, 8, 8, 4, 2, 1);
        SFERES_ARRAY(int, min_layers, 8, 4, 2, 1, 1);
        SFERES_ARRAY(float, x,
                     -3.5, -2.5, -1.5, -0.5, 0.5, 1.5, 2.5, 3.5,
                     -3.5, -2.5, -1.5, -0.5, 0.5, 1.5, 2.5, 3.5,
                     -2, -1, 1, 2,
                     -1, 1,
                     0);
        SFERES_ARRAY(float, y,
                     0, 0, 0, 0, 0, 0, 0, 0,
                     1, 1, 1, 1, 1, 1, 1, 1,
                     2, 2, 2, 2,
                     3, 3,
                     4);
        SFERES_ARRAY(float, z,
                     0, 0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0,
                     0, 0,
                     0);
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
SFERES_FITNESS(FitRetina, sferes::fit::Fitness)
{
  public:
    FitRetina() : _age(0)
    {
    }

    static const size_t b_size = 256;

    template<typename Indiv>
    void eval(Indiv& ind)
    {
      this->_objs.clear();
      this->_age++;
      _create_objects();
      float err1 = 0, err2 = 0;

      std::vector<float> inputs(8);
      size_t k = 0;
      bool perfect = true;
      int s = 0;
      for (s = 0; s < 256; ++s)
      {
        std::bitset<8> in(s);
        for (size_t i = 0; i < in.size(); ++i)
          inputs[i] = in[i] ? 1 : -1;
        bool left = std::find(_left_objects.begin(),
                              _left_objects.end(),
                              (s & 0xF))
                    != _left_objects.end();
        bool right = std::find(_right_objects.begin(),
                               _right_objects.end(),
                               (s & 0xF0))
                     != _right_objects.end();
         // step
        for (size_t i = 0; i < ind.gen().get_depth(); ++i)
          ind.nn().step(inputs);
        float out = ind.nn().get_outf(0);

         // left & right
        if (left && right)
          err1 += (out > 0 ? 0 : 1);
        else
          err1 += (out < 0 ? 0 : 1);

        if (err1 != 0)
        {
          perfect = false;
          break;
        }
      }

      typename Indiv::nn_t::graph_t gsimp = ind.gen().simplified_nn().get_graph();
      if (boost::num_edges(gsimp) == 0 || boost::num_vertices(gsimp) == 0)
        _mod = 0;
      else
        _mod = mod::modularity(gsimp);
       //compute_mod_simplified(ind.gen());
      _optlength = ind.gen().compute_optimal_length(0, false);
      //      global::log_file << _optlength << "  " << _mod << std::endl;
      global::log_file << ind.gen().get_nb_connections() << "  " << _mod << std::endl;
      std::cout << " " << _optlength
                << " " << _mod
                << " " << ind.gen().get_nb_connections()
                << std::endl;
      // typedef boost::archive::xml_oarchive oa_t;
      // std::string fname =
      //   boost::lexical_cast<std::string>(global::id) + ".xml";
      // std::ofstream ofs_xml(fname.c_str());
      // oa_t oa(ofs_xml);
      // oa << boost::serialization::make_nvp("phen", ind);

      global::id += 1;

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
  protected:
    std::vector<float> _values;
    std::vector<size_t> _left_objects;
    std::vector<size_t> _right_objects;
    std::bitset<b_size> _behavior;
    int _age;

    float _mod, _mod2;
    float _length, _optlength;
    template<typename I>
    void analysis_left_right(I& indiv)
    {
      float best_right = 0, best_left = 0;
      for (size_t i = 0; i < 4; ++i)
        _analysis_left_right_cases(i, indiv, &best_left, &best_right);
      std::cout << "- BLN: " << best_left / 256.0f << std::endl
                << "- BRN: " << best_right / 256.0f << std::endl;
      std::cout << "- A: "
                << (best_left + best_right) / 512.0f
                << std::endl;
    }
    template<typename I>
    void _analysis_left_right_cases(int c, I& indiv,
                                    float*best_left, float *best_right)
    {
      assert(c < 4);
      std::vector<int> left_score(indiv.nn().get_nb_neurons());
      std::vector<int> right_score(indiv.nn().get_nb_neurons());
      std::fill(left_score.begin(), left_score.end(), 0);
      std::fill(right_score.begin(), right_score.end(), 0);

      std::vector<float> inputs(8);
      for (size_t s = 0; s < 256; ++s)
      {
        std::bitset<8> in(s);
        for (size_t i = 0; i < in.size(); ++i)
          inputs[i] = in[i] ? 1 : -1;
        bool left = std::find(_left_objects.begin(),
                              _left_objects.end(),
                              (s & 0xF))
                    != _left_objects.end();
        bool right = std::find(_right_objects.begin(),
                               _right_objects.end(),
                               (s & 0xF0))
                     != _right_objects.end();
         // step
        for (size_t i = 0; i < indiv.get_depth(); ++i)
          indiv.nn().step(inputs);
        size_t k = 0;
        BGL_FORALL_VERTICES_T(v, indiv.nn().get_graph(), typename I::nn_t::graph_t)
        {
          bool x = false;
          if (c == 0)
            x = indiv.nn().get_graph()[v].get_current_output() > 0;
          else
          if (c == 1)
            x = indiv.nn().get_graph()[v].get_current_output() >= 0;
          else
          if (c == 2)
            x = indiv.nn().get_graph()[v].get_current_output() < 0;
          else
          if (c == 3)
            x = indiv.nn().get_graph()[v].get_current_output() <= 0;

          if (left && x)
            left_score[k]++;
          if (!left && !x)
            left_score[k]++;
          if (right && x)
            right_score[k]++;
          if (!right && !x)
            right_score[k]++;

          ++k;
        }
      }
      size_t k = 0;
      float best_left_score = -1e10, best_right_score = -1e10;
      BGL_FORALL_VERTICES_T(v, indiv.nn().get_graph(), typename I::nn_t::graph_t)
      {
        if (left_score[k] > best_left_score)
          best_left_score = left_score[k];
        if (right_score[k] > best_right_score)
          best_right_score = right_score[k];
        ++k;
      }
      *best_left = std::max(best_left_score, *best_left);
      *best_right = std::max(best_right_score, *best_right);
    }
    template<typename I>
    void analysis_io_mod(I& indiv)
    {
      std::vector<std::set<typename I::nn_t::graph_t::vertex_descriptor> > mods;
      typename I::nn_t nn = indiv.simplified_nn();
      float q = mod2::split(nn.get_graph(), mods);
      assert(mods.size() == 2);
      using namespace boost::assign;
      std::set<int> left = list_of(0) (1) (2) (3);
      std::set<int> right = list_of(4) (5) (6) (7);
      float s = 0;
      for (size_t i = 0; i < 2; ++i)
      {
        int m_left = 0, m_right = 0;
        BOOST_FOREACH(typename I::nn_t::graph_t::vertex_descriptor v, mods[i])
        {
           //          std::cout << nn.get_graph()[v].get_in() << std::endl;
          if (left.find(nn.get_graph()[v].get_in()) != left.end())
            m_left++;
          if (right.find(nn.get_graph()[v].get_in()) != right.end())
            m_right++;
        }
        s += abs(m_left - m_right);
      }
      float qs = compute_mod_simplified(indiv.nn());
      std::cout << "- IOM: " << s << std::endl;
      std::cout << "- Q2: " << q << std::endl;
      std::cout << "- QS: " << qs << std::endl;

    }

    template<typename I>
    void analysis_dyn_mod(I& indiv)
    {
      std::vector<float> inputs(8);
      std::vector<std::vector<bool> > changed(2);
      changed[0].resize(indiv.nn().get_nb_neurons());
      changed[1].resize(indiv.nn().get_nb_neurons());
      for (size_t c = 0; c < changed.size(); ++c)
      {
        std::fill(changed[c].begin(), changed[c].end(), false);
        for (size_t i = 0; i < 16; ++i)
        {
          std::bitset<8> obj(i);
          std::vector<bool> prev_state(indiv.nn().get_nb_neurons());
          for (size_t s = 0; s < 16; ++s)
          {
            std::bitset<8> in(s);
            if (c == 0)
              in = (in << 4) | obj;
            else
              in = (obj << 4) | in;
            for (size_t j = 0; j < in.size(); ++j)
              inputs[j] = in[j] ? 1 : -1;
            for (size_t j = 0; j < indiv.get_depth(); ++j)
              indiv.nn().step(inputs);
            size_t k = 0;
            std::vector<bool> state(indiv.nn().get_nb_neurons());
            BGL_FORALL_VERTICES_T(v, indiv.nn().get_graph(),
                                  typename I::nn_t::graph_t)
            state[k++] = indiv.nn().get_graph()[v].get_current_output() > 0;
            if (s != 0)
              for (size_t j = 0; j < state.size(); ++j)
                if (state[j] != prev_state[j])
                  changed[c][j] = true;
            prev_state = state;
          }
        }
      }

      float diffs = 0.0f;
      for (size_t j = 0; j < changed[0].size(); ++j)
        if (changed[0][j] != changed[1][j])
          diffs += 1;
      float score = diffs / changed[0].size();
      std::cout << "- DM: " << score << std::endl;
    }


    void _create_objects()
    {
      _left_objects.push_back(15);
      _left_objects.push_back(7);
      _left_objects.push_back(5);
      _left_objects.push_back(13);
      _left_objects.push_back(1);
      _left_objects.push_back(11);
      _left_objects.push_back(4);
      _left_objects.push_back(14);
       //
      _right_objects.push_back(112);
      _right_objects.push_back(240);
      _right_objects.push_back(208);
      _right_objects.push_back(160);
      _right_objects.push_back(176);
      _right_objects.push_back(32);
      _right_objects.push_back(224);
      _right_objects.push_back(128);

    }
    void _seasons(bool left, bool right, float out, std::vector<float>& errs)
    {
      assert(errs.size() == 4);
      if (left && right)
        errs[0] += (out > 0 ? 0 : 1);
      else
        errs[0] += (out < 0 ? 0 : 1);

      if (left || right)
        errs[1] += (out > 0 ? 0 : 1);
      else
        errs[1] += (out < 0 ? 0 : 1);

      if (left)
        errs[2] += (out > 0 ? 0 : 1);
      else
        errs[2] += (out < 0 ? 0 : 1);

      if (right)
        errs[3] += (out > 0 ? 0 : 1);
      else
        errs[3] += (out < 0 ? 0 : 1);

    }
};
;
namespace sferes
{
  namespace stat
  {
    SFERES_CLASS(RetinaStop)
    {
      public:
        template<typename E>
        void refresh(const E& ea) const
        {
          if (stop_criterion(ea))
          {
            std::cout << "Stop criterion reached" << std::endl;
            ea.write();
            exit(0);
          }
        }
        void show(std::ostream& os, size_t k)
        {}
        template<class Archive>
        void serialize(Archive& ar, const unsigned int version)
        {}

        template<typename EA>
        bool stop_criterion(const EA& ea) const
        {
          for (size_t i = 0; i < ea.pop().size(); ++i)
            if (ea.pop()[i]->fit().value() == 1.0)
              return true;
          return false;

        }
    };
  }
}


long Params::seed = -1;

int main(int argc, char **argv)
{
  typedef FitRetina<Params> fit_t;
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

  assert(argc == 2);
  Params::seed = boost::lexical_cast<int>(argv[1]);
  srand(Params::seed);


  for (long i = 0; i < 40000; ++i)
    for (size_t k = 0; k < 100; ++k)
    {
      phen_t p;
      p.gen().random(k);
      p.develop();
      p.fit().eval(p);
    }
  return 0;
}
