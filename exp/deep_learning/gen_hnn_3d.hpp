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


#ifndef GEN_HNN_3D_HPP_
#define GEN_HNN_3D_HPP_

#include <boost/multi_array.hpp>
#include <boost/tr1/tuple.hpp>
#include <modules/nn2/neuron.hpp>
#include <modules/nn2/pf.hpp>
#include <modules/nn2/gen_dnn_ff.hpp>
#include <modules/nn2/trait.hpp>

#include "af_cppn.hpp"

namespace sferes
{
  namespace gen
  {
    template<typename W,
             typename Params,
             typename ParamsConns>
    class Hnn3d
    {
      public:
        typedef DnnFF<nn::Neuron<nn::PfWSum<W>,
                                 nn::AfCppn<nn::cppn::AfParams<typename Params::cppn> > >,
                      nn::Connection<W>,
                      ParamsConns> cppn_t;
        typedef boost::tuple<float, float, float> point_t;
        typedef std::vector<point_t> layer_t;
        typedef std::vector<layer_t> substrate_t;
         // we need this because we don't want to copy the substrate
        Hnn3d& operator = (const Hnn3d &o)
        {
          _cppn = o._cppn;
	  return *this;
        }
        void init()
        {
          std::cout << "Hnn3d::init" << std::endl;

	  // force linear outputs
	  typedef nn::cppn::AfParams<typename Params::cppn> afp_t;
          afp_t afparams; 
	  afparams.set(nn::cppn::linear, 0);
	  for(size_t i = 0; i < _cppn.get_outputs().size(); ++i)
	    _cppn.get_graph()[_cppn.get_outputs()[i]].set_afparams(afparams);
          _develop(_cppn); // develop the CPPN (not the HNN)
          _cppn.init();
          assert(_cppn.get_nb_inputs() == 9); // 3D + bias + distance
          _create_substrate();
        }
        void random()
        {
	  // Locality seed

	  typedef nn::cppn::AfParams<typename Params::cppn> afp_t;
          afp_t afparams; 
	  afparams.set(nn::cppn::gaussian, 0);

	  typename cppn_t::weight_t w; w.gen().data(0, 0.75f);//corresponds to 1
	  
	  assert(ParamsConns::dnn::nb_inputs == 9);
	  assert(ParamsConns::dnn::nb_outputs == 7);
	  _cppn.set_nb_inputs(ParamsConns::dnn::nb_inputs);
	  _cppn.set_nb_outputs(ParamsConns::dnn::nb_outputs);
	  typename cppn_t::vertex_desc_t g1 = _cppn.add_neuron("g1");
	  _cppn.get_graph()[g1].set_afparams(afparams);
	  typename cppn_t::vertex_desc_t g2 = _cppn.add_neuron("g2");
	  _cppn.get_graph()[g2].set_afparams(afparams);
	  // first layer
	  _cppn.add_connection(_cppn.get_input(6), g1, w);
	  _cppn.add_connection(_cppn.get_input(7), g2, w);
	  // layer to out
	  w.gen().data(0,.8750f);// correponds to 1.5
	  _cppn.add_connection(g1, _cppn.get_output(0), w);
	  _cppn.add_connection(g2, _cppn.get_output(0), w);
	  _cppn.add_connection(g1, _cppn.get_output(2), w);
	  _cppn.add_connection(g2, _cppn.get_output(2), w);
	  // bias
	  w.gen().data(0, 0.5f);// correponds to 0
	  _cppn.add_connection(_cppn.get_input(8), _cppn.get_output(0), w);
	  _cppn.add_connection(_cppn.get_input(8), _cppn.get_output(2), w);
	  
	  // connect the other I/O together
	  for (size_t i = 0; i < _cppn.get_nb_inputs(); ++i)
	    for (size_t j = 0; j < _cppn.get_nb_outputs(); ++j)
	      if (j != 0 && j != 2)
		{
		  w.random();		  
		  _cppn.add_connection(_cppn.get_input(i), 
				       _cppn.get_output(j), w);
		}
	  //_cppn.random();
          // for (size_t i = 0; i < Params::hnn::nb_rand_mutations; ++i)
          //   _cppn.mutate();
        }
        void mutate()
        {
          _cppn.mutate();
        }
        void cross(const Hnn3d& o, Hnn3d& c1, Hnn3d& c2)
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

        std::vector<float> query(const point_t& p1, const point_t& p2)
        {
          std::vector<float> in(9);
          in[0] = p1.get<0>();
          in[1] = p1.get<1>();
          in[2] = p1.get<2>();
          in[3] = p2.get<0>();
          in[4] = p2.get<1>();
          in[5] = p2.get<2>();
	  in[6] = p1.get<0>()  - p2.get<0>();
	  in[7] = p1.get<1>()  - p2.get<1>();
          in[8] = 1;

          for (size_t k = 0; k < _cppn.get_depth(); ++k)
            _cppn.step(in);

          return _cppn.get_outf();
        }

        template<class Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
          ar& BOOST_SERIALIZATION_NVP(_cppn);
        }
        const cppn_t& cppn() const
        {
          return _cppn;
        }
        cppn_t& cppn()
        {
          return _cppn;
        }
        const substrate_t& substrate() const { return _substrate; }
      protected:
        cppn_t _cppn;
        substrate_t _substrate;

        void _create_substrate()
        {
	  _substrate.clear();
           // we need the maximum size of layers to optimize the substrate space
          std::vector<float> max_dim(3);
          std::fill(max_dim.begin(), max_dim.end(), 0.0f);
          for (size_t i = 0; i < Params::hnn::nb_layers; ++i)
            for (size_t j = 0; j < 3; ++j)
              max_dim[j] = std::max(max_dim[j], Params::hnn::layer_dim(i * 3 + j));
          _substrate.resize(Params::hnn::nb_layers);
          for (size_t i = 0; i < Params::hnn::nb_layers; ++i)
          {
             // create shortcuts for useful variables
            std::vector<size_t> d(3);
            for (size_t j = 0; j < d.size(); ++j)
              d[j] = Params::hnn::layer_dim(i * 3 + j);
            std::vector<float> i_d(3), s_d(3);
            for (size_t j = 0; j < d.size(); ++j)
            {
              i_d[j] = 2.0f / (float)d[j];// / (max_dim[j] * max_dim[j]);
              s_d[j] = d[j] / 2.0 * i_d[j] - i_d[j] / 2.0;
            }
             // create the cube itself
            for (size_t n = 0; n < d[0]; ++n)
            {
              float x = n * i_d[0] - s_d[0];
              for (size_t k = 0; k < d[1]; ++k)
              {
                float y = k * i_d[1] - s_d[1];
                for (size_t p = 0; p < d[2]; ++p)
                {
                  float z = p * i_d[2] - s_d[2];
                  _substrate[i].push_back(boost::make_tuple(x,y,z));
                }
              }
            }
          }
        }

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
