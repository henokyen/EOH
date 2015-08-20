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


#ifndef GEN_HNN_HPP_
#define GEN_HNN_HPP_

#include <modules/nn2/neuron.hpp>
#include <modules/nn2/pf.hpp>

#include <modules/nn2/gen_dnn_ff.hpp>
#include <modules/nn2/af_cppn.hpp>
#include <modules/nn2/trait.hpp>

namespace sferes
{
  namespace gen
  {
    template<typename W,
             typename Params,
             typename ParamsConns>
    class Hnn
    {
      public:
        typedef DnnFF<nn::Neuron<nn::PfWSum<W>,
                                 nn::AfCppn<nn::cppn::AfParams<typename Params::cppn> > >,
                      nn::Connection<W>,
                      Params> nn_conns_t;
        void init()
        {
          _develop(_cppn);
          _cppn.init();
        }
        void random()
        {
          _cppn.random();
          for (size_t i = 0; i < Params::hnn::nb_rand_mutations; ++i)
          {
            _cppn.mutate();
          }
          assert(_cppn.get_nb_inputs() == 7);
        }
        void mutate()
        {
          _cppn.mutate();
        }
        void cross(const Hnn& o, Hnn& c1, Hnn& c2)
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

        float conn_value(float x1, float y1, float z1,
                         float x2, float y2, float z2,
                         size_t output = 0)
        {
          assert(_cppn.get_nb_inputs() == 7);
          assert(output < _cppn.get_nb_outputs());

          std::vector<float> in(7);
          in[0] = x1;
          in[1] = y1;
          in[2] = z1;
          in[3] = x2;
          in[4] = y2;
          in[5] = z2;
          in[6] = 1.0;

          for (size_t k = 0; k < _cppn.get_depth(); ++k)
            _cppn.step(in);
          return _cppn.get_outf(output) * 2.0f - 1.0f;
        }

        template<class Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
          ar& BOOST_SERIALIZATION_NVP(_cppn);
        }
        const nn_conns_t& get_cppn() const
        {
          return _cppn;
        }
        nn_conns_t& get_cppn()
        {
          return _cppn;
        }
      protected:
        nn_conns_t _cppn;

        template<typename NN>
        void _develop(NN& nn)
        {
          BGL_FORALL_EDGES_T(e, nn.get_graph(),
                             typename NN::graph_t)
          nn.get_graph()[e].get_weight().develop();
          BGL_FORALL_VERTICES_T(v, nn.get_graph(),
                                typename NN::graph_t)
          {
            nn.get_graph()[v].get_pfparams().develop();
            nn.get_graph()[v].get_afparams().develop();
          }
        }
    };
  }
}

#endif
