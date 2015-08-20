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




#ifndef PHEN_HNN_HPP
#define PHEN_HNN_HPP

#include <map>
#include <sferes/phen/indiv.hpp>
#include <modules/nn2/nn.hpp>

#include <modules/nn2/params.hpp>
#include "gen_hnn.hpp"


namespace sferes
{
  namespace phen
  {
    namespace hnn
    {
      class Pos
      {
        public:
          Pos()
          {
          }
          Pos(float x, float y, float z) : _v(x, y, z)
          {
          }
          float x() const { return _v.x(); }
          float y() const { return _v.y(); }
          float z() const { return _v.z(); }
          float dist(const Pos& p) const { return (_v - p._v).norm(); }
        protected:
          Eigen::Vector3f _v;
      };
    }
     // hyperneat-inspired phenotype, based on a cppn
    SFERES_INDIV(Hnn, Indiv)
    {
      public:
        typedef Gen gen_t;
        typedef typename Params::hnn::neuron_t neuron_t;
        typedef typename Params::hnn::connection_t connection_t;
        typedef typename nn::NN<neuron_t, connection_t> nn_t;
        typedef typename nn_t::vertex_desc_t vertex_desc_t;
        typedef typename nn_t::vertex_desc_t v_d_t;
        typedef typename nn_t::graph_t graph_t;
        typedef typename std::vector<vertex_desc_t> layer_t;

        size_t nb_layers() const { return Params::hnn::layers_size(); }
        size_t get_depth() const { return nb_layers() + 1; }

        void develop()
        {
          this->gen().init();
          _all_neurons.clear();
          _layers.clear();
           // create the nn
          _nn = nn_t();
          size_t nb_layers = Params::hnn::layers_size();
          size_t nb_inputs = Params::hnn::layers(0);
          size_t nb_outputs = Params::hnn::layers(nb_layers - 1);
          _layers.resize(nb_layers);
          _nn.set_nb_inputs(nb_inputs);
          _nn.set_nb_outputs(nb_outputs);

           // add i/o
          BGL_FORALL_VERTICES_T(v, _nn.get_graph(), typename nn_t::graph_t)
          _all_neurons.push_back(v);
           // first & last layers
          _layers[0] = _nn.get_inputs();
          _layers[nb_layers - 1] = _nn.get_outputs();

           // create neurons
          for (size_t i = 1; i < nb_layers - 1; ++i)
          {
            _layers[i].clear();
            for (size_t j = 0; j < _layers[i].size(); ++j)
            {
              vertex_desc_t v = _nn.add_neuron("n" + boost::lexical_cast<std::string>(i));
              _layers[i].push_back(v);
              _all_neurons.push_back(v);
            }
          }
           // coords
          for (size_t i = 0; i < _all_neurons.size(); ++i)
            _coords_map[_all_neurons[i]] = hnn::Pos(Params::hnn::x(i),
                                                    Params::hnn::y(i),
                                                    Params::hnn::z(i));
           // connections
          for (size_t i = 0; i < _layers.size() - 1; ++i)
            for (size_t s = 0; s < _layers[i].size(); ++s)
              for (size_t t = 0; t < _layers[i + 1].size(); ++t)
              {
                const hnn::Pos& p_s = _coords_map[_layers[i][s]];
                const hnn::Pos& p_t = _coords_map[_layers[i + 1][t]];

                float w_exist = this->gen().conn_value(p_s.x(), p_s.y(), p_s.z(),
                                                       p_t.x(), p_t.y(), p_t.z(), 1);
                float w = this->gen().conn_value(p_s.x(), p_s.y(), p_s.z(),
                                                 p_t.x(), p_t.y(), p_t.z(), 0);
                typename connection_t::weight_t weight = typename connection_t::weight_t(w);
                if (w_exist > 0)
                  _nn.add_connection(_layers[i][s], _layers[i + 1][s], weight);
              }
        }
        void show_(std::ostream& ofs)
        {
          //           //_nn.write(os);
          //           //std::ofstream ofs("/tmp/nn.svg");
          //          ofs << "<svg width=\"200px\" height=\"200px\" viewbox=\"-500 -500 500 500\">";
          //          for (size_t i = 0; i < _all_neurons.size() * 2; i += 2)
          //          {
          //            float x = Params::hnn::substrate(i);
          //            float y = Params::hnn::substrate(i + 1);
          //            ofs << "<circle cx=\"" << x * 100 + 50
          //                << "\" cy=\"" << y * 100 + 50
          //                << "\" r=\"2\" fill=\"blue\" "
          //                << "opacity=\""
          //                << std::max(0.0f, _nn.get_graph()[_all_neurons[i / 2]].get_pfparams()[0]) << "\" "
          //                << " />" << std::endl;
          //          }
          //          typedef typename nn_t::graph_t graph_t;
          //          typedef typename nn_t::vertex_desc_t v_d_t;
          //          const graph_t& g = this->nn().get_graph();
          //          int nb_conns = 0;
          //          for (size_t i = 0; i < this->_all_neurons.size() * 2; i += 2)
          //            for (size_t j = 0; j < this->_all_neurons.size() * 2; j += 2)
          //            {
          //              v_d_t vi = this->_all_neurons[i / 2];
          //              v_d_t vj = this->_all_neurons[j / 2];
          //              BGL_FORALL_ADJ_T(vi, u, g, graph_t)
          //              if (vj == u)
          //              {
          //                float x1 = Params::hnn::substrate(3 * i) * 100 + 50;
          //                float y1 = Params::hnn::substrate(3 * i + 1) * 100 + 50;
          //                float x2 = Params::hnn::substrate(3 * j) * 100 + 50;
          //                float y2 = Params::hnn::substrate(3 * j + 1) * 100 + 50;
          //                float w = this->gen().conn_value(i, j, false);
          //                ofs << "<line x1=\"" << x1 << "\" y1=\"" << y1 << "\" "
          //                    << "x2=\"" << x2 << "\" y2=\"" << y2
          //                    << "\" stroke=\""
          //                    << (w > 0 ? "green" : "red")
          //                    << "\" stroke-width=\""
          //                    << fabs(w)
          //                    << "\" opacity=\"0.25\"  />"
          //                    << std::endl;
          //                ++nb_conns;
          //              }
          //            }
          //          ofs << "</svg>";
          //          assert(nb_conns == _nn.get_nb_connections());
        }
        float compute_length()
        {

          float length = 0;
          BGL_FORALL_EDGES_T(e, _nn.get_graph(), graph_t)
          {
            vertex_desc_t src = boost::source(e, _nn.get_graph());
            vertex_desc_t tgt = boost::target(e, _nn.get_graph());
            float d = _coords_map[src].dist(_coords_map[tgt]);
            length += d * d;
          }
          return length;
        }
        nn_t simplified_nn()
        {
          nn_t nn = _nn;
           // remove not-connected vertices
          std::vector<vertex_desc_t> to_remove;
          nn.simplify(); //WARNING: suppress some subgraphs !
          BGL_FORALL_VERTICES_T(v, nn.get_graph(), graph_t)
          if (boost::in_degree(v, nn.get_graph())
              + boost::out_degree(v, nn.get_graph()) == 0)
            to_remove.push_back(v);
          for (size_t i = 0; i < to_remove.size(); ++i)
            remove_vertex(to_remove[i], nn.get_graph());
          return nn;
        }

        nn_t& nn() { return _nn; }
        const nn_t& nn() const { return _nn; }
        const std::vector<typename nn_t::vertex_desc_t>&
        all_neurons() const { return _all_neurons; }

      protected:
        nn_t _nn;
        std::vector<layer_t> _layers;
        std::vector<typename nn_t::vertex_desc_t> _all_neurons;
        std::map<v_d_t, hnn::Pos> _coords_map;
    };
  }
}
#endif
