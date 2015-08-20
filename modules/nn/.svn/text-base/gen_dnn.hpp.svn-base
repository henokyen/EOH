//| This file is a part of the sferes2 framework.
//| Copyright 2009, ISIR / Universite Pierre et Marie Curie (UPMC)
//| Main contributor(s): Jean-Baptiste Mouret, mouret@isir.fr
//|
//| This software is a computer program whose purpose is to facilitate
//| experiments in evolutionary computation and evolutionary robotics.
//| 
//| This software is governed by the CeCILL license under French law
//| and abiding by the rules of distribution of free software.  You
//| can use, modify and/ or redistribute the software under the terms
//| of the CeCILL license as circulated by CEA, CNRS and INRIA at the
//| following URL "http://www.cecill.info".
//| 
//| As a counterpart to the access to the source code and rights to
//| copy, modify and redistribute granted by the license, users are
//| provided only with a limited warranty and the software's author,
//| the holder of the economic rights, and the successive licensors
//| have only limited liability.
//|
//| In this respect, the user's attention is drawn to the risks
//| associated with loading, using, modifying and/or developing or
//| reproducing the software by the user in light of its specific
//| status of free software, that may mean that it is complicated to
//| manipulate, and that also therefore means that it is reserved for
//| developers and experienced professionals having in-depth computer
//| knowledge. Users are therefore encouraged to load and test the
//| software's suitability as regards their requirements in conditions
//| enabling the security of their systems and/or data to be ensured
//| and, more generally, to use and operate it in the same conditions
//| as regards security.
//|
//| The fact that you are presently reading this means that you have
//| had knowledge of the CeCILL license and that you accept its terms.




#ifndef DNN_HPP_
#define DNN_HPP_
#include <bitset>
#include <boost/serialization/serialization.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/utility.hpp>
#include <sferes/dbg/dbg.hpp>
#include <sferes/misc.hpp>
#include <modules/nn/nn.hpp>
namespace sferes
{
  namespace gen
  {

    template <class Graph>
      typename boost::graph_traits<Graph>::vertex_descriptor
      random_vertex(Graph& g)
      {
        assert(num_vertices(g));      
        using namespace boost;
        if (num_vertices(g) > 1)
        {
          std::size_t n = misc::rand(num_vertices(g));
          typename graph_traits<Graph>::vertex_iterator i = vertices(g).first;
          while (n-- > 0) ++i;
          return *i;
        } 
        else
          return *vertices(g).first;
      }

    template <class Graph>
      typename boost::graph_traits<Graph>::edge_descriptor
      random_edge(Graph& g)
      {
        assert(num_edges(g));
        using namespace boost;
        if (num_edges(g) > 1)
        {
          std::size_t n = misc::rand(num_edges(g));
          typename graph_traits<Graph>::edge_iterator i = edges(g).first;
          while (n-- > 0) ++i;
          return *i;
        } 
        else
          return *edges(g).first;
      }


    namespace dnn
    {
      enum weight_t {continuous = 0, modulation, sampled, bitstring };
      enum init_t { ff = 0, random_topology };
      template<int T, typename Params>
        struct MutationWeight_f
        {
          float operator()(float w) { assert(0); return 0.0f; }
        };
      template<int T, typename io_t, typename Params>
        struct RandomWeight_f
        {
          float operator()() { assert(0); return 0.0f; }
        };
      template<int T, typename Params>
        struct RandomVect_f
        {
          void operator()(std::vector<float>& v) { assert(0); }
        };
      template<int T, typename Params>
        struct MutationVect_f
        {
          void operator()(std::vector<float>& v) { assert(0);}
        };
    }

    template<typename N, typename C, typename Params>
      class DnnT : public nn::NN<N, C>
    {
      public:
        typedef nn::NN<N, C> nn_t;
        typedef N neuron_t;
        typedef C conn_t;
        typedef typename nn_t::io_t io_t;
        typedef typename nn_t::vertex_desc_t vertex_desc_t;
        typedef typename nn_t::edge_desc_t edge_desc_t;
        typedef typename nn_t::adj_it_t adj_it_t;

        typedef typename nn_t::graph_t graph_t;

        void random()
        {
          if (Params::dnn::init == dnn::ff)
            _random_ff(Params::dnn::nb_inputs, Params::dnn::nb_outputs);
          else
            _random(Params::dnn::nb_inputs, Params::dnn::nb_outputs,
                Params::dnn::min_nb_neurons, Params::dnn::max_nb_neurons,
                Params::dnn::min_nb_conns, Params::dnn::max_nb_conns);
        }

        void mutate() 
        {
          _change_conns();

          _change_neurons();

          if (misc::rand<float>() < Params::dnn::m_rate_add_conn)
            _add_conn_nodup();

          if (misc::rand<float>() < Params::dnn::m_rate_del_conn)
            _del_conn();

          if (misc::rand<float>() < Params::dnn::m_rate_add_neuron)
            _add_neuron_on_conn();

          if (misc::rand<float>() < Params::dnn::m_rate_del_neuron)
            _del_neuron();

        }
        void cross(const DnnT& o, DnnT& c1, DnnT& c2) 
        {
          if (misc::flip_coin())
          {
            c1 = *this;
            c2 = o;
          }
          else
          {
            c2 = *this;
            c1 = o;
          }
        }
        // serialize the graph "by hand"...
        template<typename Archive>
          void save(Archive& a, const unsigned v) const
          {
            dbg::trace("nn", DBG_HERE);
            std::vector<int> inputs;
            std::vector<int> outputs;
            std::vector<std::vector<float> > afparams;
            std::vector<std::vector<float> > pfparams;
            std::map<vertex_desc_t, int> nmap;
            std::vector<std::pair<int, int> > conns;
            std::vector<io_t> weights;

            BGL_FORALL_VERTICES_T(v, this->_g, graph_t)
            {
              if (this->is_input(v))
                inputs.push_back(afparams.size());
              if (this->is_output(v))
                outputs.push_back(afparams.size());
              nmap[v] = afparams.size();
              afparams.push_back(this->_g[v].get_afparams());
              pfparams.push_back(this->_g[v].get_pfparams());
            }
            BGL_FORALL_EDGES_T(e, this->_g, graph_t)
            {
              conns.push_back(std::make_pair(nmap[source(e, this->_g)],
                    nmap[target(e, this->_g)]));
              weights.push_back(this->_g[e].get_weight());
            }
            assert(pfparams.size() == afparams.size());
            assert(weights.size() == conns.size());

            a & BOOST_SERIALIZATION_NVP(afparams);	
            a & BOOST_SERIALIZATION_NVP(pfparams);	
            a & BOOST_SERIALIZATION_NVP(weights);
            a & BOOST_SERIALIZATION_NVP(conns);	
            a & BOOST_SERIALIZATION_NVP(inputs);	
            a & BOOST_SERIALIZATION_NVP(outputs);	
          }
        template<typename Archive>
          void load(Archive& a, const unsigned v)
          {
            dbg::trace("nn", DBG_HERE);
            std::vector<int> inputs;
            std::vector<int> outputs;
            std::vector<std::vector<float> > afparams;
            std::vector<std::vector<float> > pfparams;
            std::map<size_t, vertex_desc_t> nmap;
            std::vector<std::pair<int, int> > conns;
            std::vector<io_t> weights;

            a & BOOST_SERIALIZATION_NVP(afparams);
            a & BOOST_SERIALIZATION_NVP(pfparams);
            a & BOOST_SERIALIZATION_NVP(weights);
            a & BOOST_SERIALIZATION_NVP(conns);	
            a & BOOST_SERIALIZATION_NVP(inputs);	
            a & BOOST_SERIALIZATION_NVP(outputs);	

            assert(pfparams.size() == afparams.size());

            assert(weights.size() == conns.size());
            this->set_nb_inputs(inputs.size());
            this->set_nb_outputs(outputs.size());
            for (size_t i = 0; i < this->get_nb_inputs(); ++i)
              nmap[inputs[i]] = this->get_input(i);
            for (size_t i = 0; i < this->get_nb_outputs(); ++i)
              nmap[outputs[i]] = this->get_output(i);

            for (size_t i = 0; i < afparams.size(); ++i)
              if (std::find(inputs.begin(), inputs.end(), i) == inputs.end() 
                  && std::find(outputs.begin(), outputs.end(), i) == outputs.end())
                nmap[i] = this->add_neuron("n", pfparams[i], afparams[i]);
              else
              {
                this->_g[nmap[i]].set_pfparams(pfparams[i]);
                this->_g[nmap[i]].set_afparams(afparams[i]);
              }


            //assert(nmap.size() == num_vertices(this->_g));
            for (size_t i = 0; i < conns.size(); ++i)
              this->add_connection(nmap[conns[i].first], nmap[conns[i].second], weights[i]);
          }
        BOOST_SERIALIZATION_SPLIT_MEMBER();

      protected:
        dnn::RandomWeight_f<Params::dnn::weight_type, io_t, Params> _random_weight_f;
        dnn::RandomVect_f<Params::dnn::weight_type, Params> _random_vect_f;
        dnn::MutationWeight_f<Params::dnn::weight_type, Params> _mutation_weight_f;
        dnn::MutationVect_f<Params::dnn::weight_type, Params> _mutation_vect_f;

        // we start with a fully connected 0-layer perceptron with
        // random weights
        void _random_ff(size_t nb_inputs, size_t nb_outputs) 
        {
          this->set_nb_inputs(nb_inputs);
          this->set_nb_outputs(nb_outputs);
          if (Params::dnn::io_param_evolving) 
          {
            BGL_FORALL_VERTICES_T(v, this->_g, graph_t)
            {
              std::vector<float> afparams(neuron_t::af_t::nb_params);	
              std::vector<float> pfparams(neuron_t::pf_t::nb_params);
              this->_random_vect_f(afparams);
              this->_random_vect_f(pfparams);
              this->_g[v].set_afparams(afparams);
              this->_g[v].set_pfparams(pfparams);
            }
          }
          BOOST_FOREACH(vertex_desc_t& i, this->_inputs)
            BOOST_FOREACH(vertex_desc_t& o, this->_outputs)
            this->add_connection(i, o, this->_random_weight_f());	
        }

        void _random(size_t nb_inputs, size_t nb_outputs,
            size_t min_nb_neurons, size_t max_nb_neurons,
            size_t min_nb_conns, size_t max_nb_conns) 
        {
          // io
          this->set_nb_inputs(nb_inputs);
          this->set_nb_outputs(nb_outputs);

          if (Params::dnn::io_param_evolving) 
          { // Comment: if we don't go through this code, afparams and
            // pfparams will remain empty and thus won't be mutated...
            // SD: to be checked...
            BGL_FORALL_VERTICES_T(v, this->_g, graph_t)
            {
              std::vector<float> afparams(neuron_t::af_t::nb_params);	
              std::vector<float> pfparams(neuron_t::pf_t::nb_params);
              _random_vect_f(afparams);
              _random_vect_f(pfparams);
              this->_g[v].set_afparams(afparams);
              this->_g[v].set_pfparams(pfparams);
            }
          }

          // neurons
          size_t nb_neurons = misc::rand(min_nb_neurons, max_nb_neurons);
          for (size_t i = 0; i < nb_neurons; ++i)
            _add_neuron();

          // conns
          size_t nb_conns = misc::rand(min_nb_conns, max_nb_conns);
          for (size_t i = 0; i < nb_conns; ++i)
            _add_conn_nodup();

          this->simplify();
        }

        vertex_desc_t _random_tgt()
        {
          vertex_desc_t v;
          do
            v = random_vertex(this->_g);
          while (is_input(v));
          return v;
        }
        vertex_desc_t _random_src()
        {
          vertex_desc_t v;
          do
            v = random_vertex(this->_g);
          while (is_output(v));
          return v;
        }

        vertex_desc_t _add_neuron() 
        { 
          std::vector<float> afparams(neuron_t::af_t::nb_params);	
          std::vector<float> pfparams(neuron_t::pf_t::nb_params);
          _random_vect_f(afparams);
          _random_vect_f(pfparams);
          return this->add_neuron("n", pfparams, afparams);
        }
        vertex_desc_t _add_neuron_on_conn()
        {
          if (!num_edges(this->_g))
            return (vertex_desc_t)0x0;	
          edge_desc_t e = random_edge(this->_g);
          vertex_desc_t src = source(e, this->_g);
          vertex_desc_t tgt = target(e, this->_g);
          typename nn_t::io_t w = this->_g[e].get_weight();
          // 
          std::vector<float> afparams(neuron_t::af_t::nb_params);	
          std::vector<float> pfparams(neuron_t::pf_t::nb_params);
          this->_random_vect_f(afparams);
          this->_random_vect_f(pfparams);
          vertex_desc_t n = this->add_neuron("n", pfparams, afparams);
          // 
          remove_edge(e, this->_g);
          this->add_connection(src, n, w);
          this->add_connection(n, tgt, w);
          return n;	
        }

        void _del_neuron()
        {	
          assert(num_vertices(this->_g));

          if (this->get_nb_neurons() <= this->get_nb_inputs() + this->get_nb_outputs())
            return;
          vertex_desc_t v;
          do
            v = random_vertex(this->_g);
          while (is_output(v) || is_input(v));

          clear_vertex(v, this->_g);	
          remove_vertex(v, this->_g);
        }
        void _add_conn()
        {
          this->add_connection(_random_src(), 
              _random_tgt(), 
              _random_weight_f());
        }
        // add a random connection by avoiding to duplicate an existent connection
        void _add_conn_nodup()
        {
          vertex_desc_t src, tgt;
          // this is only an upper bound; a connection might of course
          // be possible even after max_tries tries.
          size_t max_tries = num_vertices(this->_g) * num_vertices(this->_g), 
                 nb_tries = 0;
          do 
          {
            src = _random_src();
            tgt = _random_tgt();
          }
          while (is_adjacent(this->_g, src, tgt) && ++nb_tries < max_tries);
          if (nb_tries < max_tries)
            this->add_connection(src, tgt, _random_weight_f());
        }
        void _del_conn()
        {
          if (!this->get_nb_connections())
            return;
          remove_edge(random_edge(this->_g), this->_g);
        }
        void _change_neurons()
        {
          // Warning: mutation rates are used directly within _mutation_vect_f
          BGL_FORALL_VERTICES_T(v, this->_g, graph_t) {
            if (this->_g[v].get_afparams().size() > 0 )
            {
              std::vector<float> afparam=this->_g[v].get_afparams();
              _mutation_vect_f(afparam);
              this->_g[v].set_afparams(afparam);
            }
            if (this->_g[v].get_pfparams().size() > 0)
            {
              std::vector<float> pfparam=this->_g[v].get_pfparams();
              _mutation_vect_f(pfparam);
              this->_g[v].set_pfparams(pfparam);
            }
          }
        }
        // No dub version
        void _change_conns()
        {
          BGL_FORALL_EDGES_T(e, this->_g, graph_t)
            if (misc::rand<float>() < Params::dnn::m_rate_weight)
              this->_g[e]._weight = _mutation_weight_f(this->_g[e].get_weight());

          BGL_FORALL_EDGES_T(e, this->_g, graph_t)
            if (misc::rand<float>() < Params::dnn::m_rate_change_conn)
            {
              vertex_desc_t src = source(e, this->_g);
              vertex_desc_t tgt = target(e, this->_g);
              typename nn_t::io_t w = this->_g[e].get_weight();
              remove_edge(e, this->_g);
              int max_tries = num_vertices(this->_g) * num_vertices(this->_g),
                  nb_tries = 0;
              if (misc::flip_coin())
                do
                  src = _random_src();
                while(++nb_tries < max_tries && is_adjacent(this->_g, src, tgt));
              else
                do
                  tgt = _random_tgt();
                while(++nb_tries < max_tries && is_adjacent(this->_g, src, tgt));
              if (nb_tries < max_tries)
                this->add_connection(src, tgt, w);
              return;
            }
        }
    };

    // version for neurons (legacy code)
    template<typename Params>
      class Dnn : public DnnT<nn::Neuron<typename Params::nn::potential_t, 
      typename Params::nn::activation_t>,
      nn::Connection,
      Params>
    {
      public:
        typedef typename nn::NN<nn::Neuron<typename Params::nn::potential_t, 
                typename Params::nn::activation_t>,
                nn::Connection> nn_t;
    };


    // partial specialization for weight types
    namespace dnn
    {
      //---  continous type ----
      template<typename Params>
        struct MutationWeight_f<continuous, Params>
        {
          float operator()(float w) 
          {
            w += misc::gaussian_rand(0.0f, Params::dnn::weight_sigma);	  
            return misc::put_in_range(w, -Params::dnn::max_weight, Params::dnn::max_weight);
          }
          std::pair<float, float> operator()(const std::pair<float, float>& w)
          {
            float w1 =  misc::rand<float>() < 0.5 ? operator()(w.first) : w.first;
            float w2 =  misc::rand<float>() < 0.5 ? operator()(w.second) : w.second;
            return std::make_pair(w1, w2);
          }
          std::vector<float> operator()(const std::vector<float>& v)
          {
            std::vector<float> res;
            BOOST_FOREACH(float x, v)
              if (misc::rand<float>() < 1.0f / v.size())
                res.push_back(operator()(x));
              else
                res.push_back(x);
            assert(res.size() == v.size());
            return res;
          }
        };

      template<typename Params>
        struct RandomWeight_f<continuous, float, Params>
        {
          float operator()() 
          {
            return misc::rand<float>(Params::dnn::max_weight * 2.0f)
              - Params::dnn::max_weight;   
          }
        };
      template<typename Params>
        struct RandomWeight_f<continuous, std::pair<float, float>, Params>
        {
          std::pair<float, float> operator()() 
          {
            float v1 = misc::rand<float>(Params::dnn::max_weight * 2.0f)
              - Params::dnn::max_weight;   
            float v2 = misc::rand<float>(Params::dnn::max_weight * 2.0f)
              - Params::dnn::max_weight;
            return std::make_pair(v1, v2);
          }
        };
      template<typename Params>
        struct RandomWeight_f<continuous, std::vector<float>, Params>
        {
          std::vector<float> operator()() 
          {
            std::vector<float> v;
            for (size_t i = 0; i < Params::dnn::weight_size; ++i)
              v.push_back(misc::rand<float>(Params::dnn::max_weight * 2.0f)
                  - Params::dnn::max_weight);   
            return v;
          }
        };


      template<typename Params>
        struct RandomVect_f<continuous, Params>
        {
          void operator()(std::vector<float>& v) 
          {
            for (size_t i = 0; i < v.size(); ++i)
            {
              v[i] = misc::rand<float>();
              assert(v[i] < 1);
            }
          }
        };


      template<typename Params>
        struct MutationVect_f<continuous, Params>
        {
          void operator()(std::vector<float>& v)
          {
            for (size_t i = 0; i < v.size(); ++i) 
            {
              if (misc::rand<float>() < Params::dnn::m_rate_fparams) 
              {
                v[i] += misc::gaussian_rand(0.0f, Params::dnn::vect_sigma);	  
                v[i] = misc::put_in_range(v[i], 0, 1);
              }
            }
          }
        };


      // ---  sampled type ---
      template<typename Params>
        struct MutationWeight_f<sampled, Params>
        {
          float operator()(float w) 
          {
            // find index
            int index = -1;
            for (size_t i = 0; i < Params::dnn::weights_size(); ++i)
              if (fabs(Params::dnn::weights(i) - w) < 1e-3)
              { 
                index = i; 
                break;
              }
            assert(index != -1);
            // move -1/+1
            int m = misc::flip_coin() ? -1 : 1;
            return Params::dnn::weights(misc::put_in_range(m + index, 0, 
                  (int) Params::dnn::weights_size() - 1));
          }
        };

      template<typename Params>
        struct RandomWeight_f<sampled, float, Params>
        {
          float operator()() 
          {
            return Params::dnn::weights(misc::rand(Params::dnn::weights_size()));
          }
        };

      template<typename Params>
        struct RandomVect_f<sampled, Params>
        {
          void operator()(std::vector<float>& v)
          {
            for (size_t i = 0; i < v.size(); ++i)
              v[i] = Params::dnn::weights(misc::rand(Params::dnn::weights_size()));
          }
        };


      template<typename Params>
        struct MutationVect_f<sampled, Params>
        {
          void operator()(std::vector<float>& v)
          {
            for (size_t vi = 0; vi < v.size(); ++vi) 
            {
              if (misc::rand<float>() < Params::dnn::m_rate_fparams) 
              {
                // find index
                int index = -1;
                for (size_t i = 0; i < Params::dnn::weights_size(); ++i)
                  if (fabs(Params::dnn::weights(i) - v[vi]) < 1e-3)
                  { 
                    index = i; 
                    break;
                  }
                assert(index != -1);
                // move -1/+1
                int m = misc::flip_coin() ? -1 : 1;
                v[vi] = Params::dnn::weights(misc::put_in_range(m + index, 0, 
                      (int) Params::dnn::weights_size() - 1));

              }
            }
          }
        };


      template<int S>
        float bs_to_float(const std::bitset<S>& bs, float max_v)
        {
          static const size_t bitstring_max = 1 << S;
          return ((bs.to_ulong() / (float) bitstring_max * 2.0f) - 1.0f) * max_v;
        }

      //---  bitstring type ----
      template<typename Params>
        struct MutationWeight_f<bitstring, Params>
        {
          float operator()(float w) 
          {
            static const size_t bitstring_max = 1 << Params::dnn::nb_bits;
            size_t v2 = (size_t)roundf(((w / Params::dnn::max_weight) + 1.0f) 
                / 2.0f * bitstring_max );
            std::bitset<Params::dnn::nb_bits> bs(v2);
            for (size_t i = 0; i < Params::dnn::nb_bits; ++i)
              if (misc::rand<float>() < Params::dnn::m_rate_bit_flip)
                bs[i] = bs[i].flip();
            return bs_to_float<Params::dnn::nb_bits>(bs, Params::dnn::max_weight);
          }
        };

      template<typename Params>
        struct RandomWeight_f<bitstring, float, Params>
        {
          float operator()() 
          {
            std::bitset<Params::dnn::nb_bits> bs;
            for (size_t i = 0; i < Params::dnn::nb_bits; ++i)
              bs[i] = (int) misc::flip_coin();
            return bs_to_float<Params::dnn::nb_bits>(bs, Params::dnn::max_weight);
          }
        };

      template<typename Params>
        struct RandomVect_f<bitstring, Params>
        {

          void operator()(std::vector<float>& v) 
          {
            std::bitset<Params::dnn::nb_bits> bs;
            for (size_t j = 0; j < v.size(); ++j)
            {
              for (size_t i = 0; i < Params::dnn::nb_bits; ++i)
                bs[i] = (int) misc::flip_coin();
              v[j] = bs_to_float<Params::dnn::nb_bits>(bs, Params::dnn::max_bias);
            }
          }
        };

      template<typename Params>
        struct MutationVect_f<bitstring, Params>
        {
          void operator()(std::vector<float>& v)
          {
            static const size_t bitstring_max = 1 << Params::dnn::nb_bits;
            for (size_t vi = 0; vi < v.size(); ++vi) 
            {
              if (misc::rand<float>() < Params::dnn::m_rate_fparams) 
              {		  
                size_t v2 = (size_t)roundf(((v[vi] / Params::dnn::max_weight) + 1.0f) 
                    / 2.0f * bitstring_max );
                std::bitset<Params::dnn::nb_bits> bs(v2);
                for (size_t i = 0; i < Params::dnn::nb_bits; ++i)
                  if (misc::rand<float>() < Params::dnn::m_rate_bit_flip)
                    bs[i] = bs[i].flip();
                v[vi]=bs_to_float<Params::dnn::nb_bits>(bs, Params::dnn::max_weight);
              }

            }
          }
        };


    }  
  }
}

#endif
