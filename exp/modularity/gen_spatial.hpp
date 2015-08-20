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


#ifndef DNN_SPATIAL_HPP_
#define DNN_SPATIAL_HPP_

#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/dag_shortest_paths.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/graph/reverse_graph.hpp>
#include <boost/property_map/vector_property_map.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>
#include <string>
#include<map>
#include<algorithm>

//#include <modules/nn2/gen_dnn_ff.hpp>

#include "opt_placement.hpp"
//#include <modules/modularity/hierarchy.hpp>
using namespace std;
namespace sferes
{
  namespace gen
  {
    namespace spatial
    {
      class Pos
      {
        public:
          Pos() {
          }
          Pos(float x, float y, float z) : _x(x), _y(y), _z(z) {
          }
          float dist(const Pos& p) const
          {
            float x = _x - p._x;
            float y = _y - p._y;
            float z = _z - p._z;
            return sqrt(x * x + y * y + z * z);
          }
          float x() const { return _x; }
          float y() const { return _y; }
          float z() const { return _z; }

          template<class Archive>
          void serialize(Archive& ar, const unsigned int version)
          {
            ar& BOOST_SERIALIZATION_NVP(_x);
            ar& BOOST_SERIALIZATION_NVP(_y);
            ar& BOOST_SERIALIZATION_NVP(_z);
          }
          bool operator == (const Pos &p)
          { return _x == p._x && _y == p._y && _z == p._z; }
        protected:
          float _x, _y, _z;
      };
    }

    template<typename N, typename C, typename Params>
    class DnnSpatial : public Dnn<N, C, Params>
    {
      public:
        typedef nn::NN<N, C> nn_t;
        typedef N neuron_t;
        typedef C conn_t;
        typedef typename nn_t::io_t io_t;
        typedef typename nn_t::vertex_desc_t vertex_desc_t;
        typedef typename nn_t::edge_desc_t edge_desc_t;
        typedef typename nn_t::graph_t graph_t;
        typedef std::vector<spatial::Pos> coords_t;
        typedef std::vector<vertex_desc_t> layer_t;
        typedef std::map<vertex_desc_t, std::pair<int, int> > layer_map_t;
        typedef typename std::vector<neuron_t> neuron_list;



        void init()
        {
        // std::cout<<"initiatting the network??"<<std::endl;
          Dnn<N, C, Params>::init();
        // _reinit_layers();// was commented out
        }
        size_t nb_layers() const { return Params::dnn::spatial::layers_size(); }
        size_t get_depth() const { return nb_layers() + 1; }

        void random(int nb_conns)
        {
          //std::cout<<"caling random "<<std::endl;
			_reset();
          _layers.resize(nb_layers());
          this->set_nb_inputs(Params::dnn::spatial::layers(0));
          this->set_nb_outputs(Params::dnn::spatial::layers(nb_layers() - 1));
          _layers[0] = this->get_inputs();
          _layers[nb_layers() - 1] = this->get_outputs();

          assert(_layers[0].size() == Params::dnn::nb_inputs);
          for (size_t i = 1; i < nb_layers() - 1; ++i)
          {
            std::vector<vertex_desc_t> layer;
            for (size_t j = 0; j < Params::dnn::spatial::layers(i); ++j)
            {
              vertex_desc_t v =
                this->add_neuron("n" + boost::lexical_cast<std::string>(i));
              layer.push_back(v);
              _layer_map[v] = std::make_pair(i, j);
            }
            _layers[i] = layer;
          }

          assert(_layers[nb_layers() - 1].size() == Params::dnn::nb_outputs);

          this->_random_neuron_params();
          _copy_coords();

          for (size_t i = 0; i < nb_conns; ++i)
            this->_add_conn();
          _rand_obj = misc::rand<float>();
        }
void random()
        {
          _reset();
          _layers.resize(nb_layers());
          this->set_nb_inputs(Params::dnn::spatial::layers(0));
          this->set_nb_outputs(Params::dnn::spatial::layers(nb_layers() - 1));
          _layers[0] = this->get_inputs();
          _layers[nb_layers() - 1] = this->get_outputs();

          assert(_layers[0].size() == Params::dnn::nb_inputs);
          for (size_t i = 1; i < nb_layers() - 1; ++i)
          {
            std::vector<vertex_desc_t> layer;
            for (size_t j = 0; j < Params::dnn::spatial::layers(i); ++j)
            {
              vertex_desc_t v =
                this->add_neuron("n" + boost::lexical_cast<std::string>(i));
              layer.push_back(v);
              _layer_map[v] = std::make_pair(i, j);
            }
            _layers[i] = layer;
          }

          assert(_layers[nb_layers() - 1].size() == Params::dnn::nb_outputs);

          this->_random_neuron_params();
          _copy_coords();
          if (Params::dnn::spatial::rand_coords)
          {
            typedef boost::mt19937 base_generator_t;
            typedef boost::variate_generator<base_generator_t, boost::uniform_real<> > rand_t;
            assert(Params::seed >= 0);
            rand_t rand_gen(base_generator_t(Params::seed),
                            boost::uniform_real<>(Params::dnn::spatial::min_coord,
                                                  Params::dnn::spatial::max_coord));
            for (size_t i = 0; i < _layers[0].size(); ++i)
            {
              _xpos[i] = rand_gen();
              _coords[i] = spatial::Pos(_xpos[i], _ypos[i], _zpos[i]);
            }
          }

          size_t nb_conns = misc::rand(Params::dnn::min_nb_conns,
                                       Params::dnn::max_nb_conns);
          for (size_t i = 0; i < nb_conns; ++i)
            this->_add_conn();
           // this->_add_skip_conn();
          _rand_obj = misc::rand<float>();
        }


        template<typename Archive>
        void save(Archive& a, const unsigned v) const
        {
          Dnn<N, C, Params>::save(a, v);
          a& BOOST_SERIALIZATION_NVP(_coords);
          a& BOOST_SERIALIZATION_NVP(_xpos);
          a& BOOST_SERIALIZATION_NVP(_ypos);
          a& BOOST_SERIALIZATION_NVP(_zpos);
        }
        template<typename Archive>
        void load(Archive& a, const unsigned v)
        {
         // std::cout<<"called ende ?"<<std::endl;
          Dnn<N, C, Params>::load(a, v);
          a& BOOST_SERIALIZATION_NVP(_coords);
          a& BOOST_SERIALIZATION_NVP(_xpos);
          a& BOOST_SERIALIZATION_NVP(_ypos);
          a& BOOST_SERIALIZATION_NVP(_zpos);

          _reinit_layers();
          if (!Params::dnn::spatial::rand_coords)
            _copy_coords();
        }
        BOOST_SERIALIZATION_SPLIT_MEMBER();

        void mutate()
        {
          //cout<<"Is this where gen_spatail mutate ? \n";
          _reinit_layers();

          this->_change_conns();
          this->_change_neurons();

          if (misc::rand<float>() < Params::dnn::m_rate_add_conn)
          this->_add_conn();
          //this -> _add_skip_conn();
          //_add_non_ff_conn();

          if (misc::rand<float>() < Params::dnn::m_rate_del_conn
              && this->get_nb_connections() > Params::dnn::min_nb_conns)
            this->_del_conn();

          _rand_obj += misc::gaussian_rand<float>(0.0f, Params::rand_obj::std_dev);
#ifdef BOUNDR
           // add some bounds
          if (_rand_obj > 1) _rand_obj = 1;
          if (_rand_obj < 0) _rand_obj = 0;
#endif
        }
        float rand_obj() const { return _rand_obj; }
        void cross(const DnnSpatial& o, DnnSpatial& c1, DnnSpatial& c2)
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

        nn_t simplified_nn()
        {
//std::cout<<"before copying in simplified_nn() # of neurons is :"<<this->get_nb_neurons()<<std::endl;
          nn_t nn = *this;
//std::cout<<"# of neurons in nn_t simplified_nn(): "<<nn.get_nb_neurons()<<std::endl;
          //remove not-connected vertices
          std::vector<vertex_desc_t> to_remove;
          nn.simplify(); //WARNING: suppress some subgraphs !
          BGL_FORALL_VERTICES_T(v, nn.get_graph(), graph_t)
          if (boost::in_degree(v, nn.get_graph())
              + boost::out_degree(v, nn.get_graph()) == 0)
              to_remove.push_back(v);
               
          for (size_t i = 0; i < to_remove.size(); ++i)
            remove_vertex(to_remove[i], nn.get_graph());
//std::cout<<"size of to_remove in nn_t simplified_nn(): "<<to_remove.size()<<std::endl;
          return nn;
        }
 
    nn_t unsimplified_nn()
        {
          nn_t nn = *this;

          /*remove not-connected vertices
          std::vector<vertex_desc_t> to_remove;
         WARNING: suppress some subgraphs !
          BGL_FORALL_VERTICES_T(v, nn.get_graph(), graph_t)
          if (boost::in_degree(v, nn.get_graph())
              + boost::out_degree(v, nn.get_graph()) == 0)
              to_remove.push_back(v);

          for (size_t i = 0; i < to_remove.size(); ++i)
            remove_vertex(to_remove[i], nn.get_graph());*/
          nn.simplify_noremoval();
          return nn;
        }
	void simplified_nn(nn_t & nn)
	{
	  //remove not-connected vertices
	  std::vector<vertex_desc_t> to_remove;
	  nn.simplify_noremoval(); //WARNING: suppress some subgraphs !

	  BGL_FORALL_VERTICES_T(v, nn.get_graph(), graph_t)
	  if (boost::in_degree(v, nn.get_graph())+ boost::out_degree(v, nn.get_graph()) == 0)
		  nn.get_graph()[v].set_removed(1);

	  //to_remove.push_back(v);
//std::cout<<"size of to_remove in..."<<to_remove.size()<<std::endl;

	  //for (size_t i = 0; i < to_remove.size(); ++i)
		//remove_vertex(to_remove[i], nn.get_graph());

	 }
   size_t nb_neurons()
        {
        return this->get_nb_neurons();
        }

     bool is_validnetwork()
      {
    	 //std::cout<<"is where? \n";
    	 nn_t nn = *this;
       return nn.is_valid_network();
      }  
    vector<layer_t> get_layer()
        {
          return _layers;
        }
    template <typename T>
    const bool Contains( std::vector<T>& Vec, const T& Element )
    {
       //std::cout<<"in contains \n";
    	if (std::find(Vec.begin(), Vec.end(), Element) != Vec.end())
            return true;

        return false;
    }
float compute_optimal_length(float threshold = 0, bool write_graph = false,std::string fname="nn_opt.svg",float fit = 0)
        {
	  assert(_layers.size());
          // copy the current nn

//	  nn_t non = unsimplified_nn();
//	  graph_t gg=non.get_graph();
//	  BGL_FORALL_VERTICES_T(v, gg, graph_t)
//	  std::cout<<gg[v].get_id()<<" "<<gg[v].get_removed()<<"\n";
	  nn_t nn = simplified_nn();
          //if (is_validnetwork())
          //std::cout<<"is still valid and number of neurons is :"<<nn.get_nb_neurons()<<std::endl;
          //else
          //std::cout<<"after simplify not valid "<<std::endl;


	  if (nn.get_nb_neurons() == 0 || nn.get_nb_connections() == 0)
	    return 0;
        //std::cout<<"inside compute"<<std::endl;

	  // rebuild i/o and copy coordinates
	  std::vector<vertex_desc_t> inputs, outputs;
	  std::vector<float> x_in, y_in, x_out, y_out;
	  assert(_xpos.size() >= nn.get_nb_neurons());
	  assert(_ypos.size() >= nn.get_nb_neurons());
	  BGL_FORALL_VERTICES_T(v, nn.get_graph(), graph_t)
	    if (nn.get_graph()[v].get_in() != -1)
	      {
		inputs.push_back(v);
		x_in.push_back(_xpos[nn.get_graph()[v].get_in()]);
		y_in.push_back(_ypos[nn.get_graph()[v].get_in()]);
	      }
	   else if (nn.get_graph()[v].get_out() != -1)
	     {
	       outputs.push_back(v);
	       int o = nn.get_graph()[v].get_out();
	       int n = Params::dnn::spatial::x_size() - _layers[_layers.size() - 1].size();
	       assert(n + o < _xpos.size());
	       assert(n + o < _ypos.size());
	       x_out.push_back(_xpos[n + o]);
	       y_out.push_back(_ypos[n + o]);
	     }
	
          //  // rebuild y coords (only if write_graph)
	  std::map<vertex_desc_t, float> yv_pos; // for the svg file
           if (write_graph)
           {
             //std::cout<<"size of _coords_map:"<<_coords_map.size()<<std::endl;
            // std::cout<<"size of _layers:"<<_layers.size()<<std::endl;

//       std::vector<vertex_desc_t> vertex_list_t = nn.get_neuron_list();
//       std::vector<neuron_t> ns = nn.get_neurons();
       //std::cout<<"size of neurons is :"<<ns.size()<<"\n";
       //std::cout<<"size of n_layer_map is:"<<_layer_map.size()<<"\n";

//       for (int c=0;c<vertex_list_t.size();c++)
//       {
//    	   std::cout<<"checking  "<<nn.get_neuron_by_vertex(vertex_list_t[c]).get_id()<<"\n";
//    	   for (int k=0;k<_layers.size();k++)
//    	   {
//    		   if (Contains(_layers[k],nn.get_neuron_by_vertex(vertex_list_t[c])))
//    			   std::cout<<nn.get_neuron_by_vertex(vertex_list_t[c]).get_id()<<" is at  "<< k <<" layer \n";
//    	   }
//       }
       //std::map<vertex_desc_t, std::pair<int, int> >::iterator it;
       //BGL_FORALL_VERTICES_T(v, this->_g, graph_t)
       //std::cout<<this->_g[v].get_id()<<" ";
       //cout<<"\n";

       //BGL_FORALL_VERTICES_T(v, nn.get_graph(), graph_t)
         //std::cout<<nn.get_graph()[v].get_id()<<" ";
         //cout<<"\n";


//      BGL_FORALL_VERTICES_T(v, nn.get_graph(), graph_t)
//      {
//       for (typename std::map<vertex_desc_t, std::pair<int, int> >::iterator it=_layer_map.begin();it!=_layer_map.end();++it)
//       {
//    	   //std::cout<<"comparing "<<nn.get_graph()[v].get_id();
//           //std::cout <<" with "<<this->_g[it->first].get_id();
//           //std::cout<<" and "<<nn.get_graph()[it->first].get_id()<<"\n";
//    	   if(nn.get_graph()[v].get_id() == this->_g[it->first].get_id())
//        	  {
//        	  cout<<nn.get_graph()[it->first].get_id()<<" "<<it->second.first<<" "<<it->second.second<<"\n";//for (int k=0;k<_layers.size();k++)
//        	  yv_pos[v] = it->second.first;
//        	  break;
////    	      if (Contains(_layers[k],v))
//        	  }
//          }
//      }
//       BOOST_FOREACH(vertex_desc_t v, this->_g) {
//    	   std::cout<<nn.get_graph()[v].get_id()<<" "<<_layer_map[v].first<<" "<<_layer_map[v].second<<"\n";
//       }
//       for (int k=0;k<_layers.size()-1;k++)
//		{
//		  bool found = false;
//		  std::cout<<"size of layer "<<k<<" is "<<_layers[k].size()<<"\n";
//		  for (int p=0;p<_layers[k].size();p++)
//			 std::cout<<this->_g[_layers[k][p]].get_id()<<"\n";
//    	}
//            std::cout<<nn.get_nb_neurons()<<std::endl;
             size_t nb_layers = _layers.size() - 1;
             std::map<vertex_desc_t, size_t> dmap;
             _compute_all_depths(nn.get_graph(), outputs, dmap);
          //std::cout<<"size of dmap:"<<dmap.size()<<std::endl;
      for (typename std::map<vertex_desc_t, size_t>::iterator it = dmap.begin(); it != dmap.end(); ++it)
  //           BGL_FORALL_VERTICES_T(v, nn.get_graph(), graph_t)
 //            {
//std::cout<<"size of layer  "<<nb_layers - it->second<<"is : "<<_layers[nb_layers - it->second].size()<<std::endl;

//std::cout << nn.get_graph()[it->first].get_id() << " => " << it->second << '\n';
                 //std::cout << nn.get_graph()[_layers[nb_layers - it->second][0]].get_id() << '\n';
                 //if (_layers[nb_layers - it->second].size() > 0)//I did this for map-elite exprmt vis
	         yv_pos[it->first] = _coords_map[_layers[nb_layers - it->second][0]].y();
 //   	   if (nn.get_graph()[v].get_in() != -1)
   //          yv_pos[v] = 0.0;
 //   	   else if(nn.get_graph()[v].get_out() != -1)
   // 		   yv_pos[v] = 4.0;
//    	   else
//    	   {
//    		  //std::cout<<"checking for hidden \n";
//    		   for (int k=1;k<_layers.size()-1;k++)
//			   {
//    			  bool found = false;
//				std::cout<<"size of layer "<<k<<" is "<<_layers[k].size()<<"\n";
//    		 for (int p=0;p<_layers[k].size();p++)
//    	     {
//    	     if ( nn.get_graph()[v].get_id()== this->_g[_layers[k][p]].get_id())
//				{
//					//std::cout << nn.get_graph()[v].get_id() << " => " << k<< '\n';
//					yv_pos[v]=k;
//					found = true;
//					break;
//				}
//			   }
//    		 if (found)
//    		   break;
//			   }
//    	   }
	         //std::cout<<"this is for x()"<< _coords_map[_layers[nb_layers - it->second][0]].x();
	       //  }
           }
    //     typename std::map<vertex_desc_t, float>::iterator it;
    //     for (it=yv_pos.begin(); it!=yv_pos.end(); ++it)
			 //std::cout << nn.get_graph()[it->first].get_id() << " => " << it->second << '\n';


          //  // compute the optimal placement
          Eigen::VectorXf x_pos = opt_placement::compute(nn.get_graph(),
                                                         inputs, x_in,
                                                         outputs, x_out);

          Eigen::VectorXf y_pos = opt_placement::compute(nn.get_graph(),
                                                         inputs, y_in,
                                                         outputs, y_out);

          // TEST
          //   size_t i = 0;
          //   BGL_FORALL_VERTICES_T(v, nn.get_graph(), graph_t) //layer y pos
          //   y_pos[i++] = yv_pos[v];

           // compute length
          float length = 0;
          std::map<vertex_desc_t, spatial::Pos> coords;
          size_t k = 0;
          BGL_FORALL_VERTICES_T(v, nn.get_graph(), graph_t)
          {
            coords[v] = spatial::Pos(x_pos[k], y_pos[k], 0);
			//std::cout<<y_pos[k];
            ++k;
          }
          BGL_FORALL_EDGES_T(e, nn.get_graph(), graph_t)
          {
            vertex_desc_t src = boost::source(e, nn.get_graph());
            vertex_desc_t tgt = boost::target(e, nn.get_graph());
      //      double weight = this->_g[e].get_weight().data(0);
              double weight = nn.get_graph()[e].get_weight().data(0);
            float d = coords[src].dist(coords[tgt]);
            if (d > threshold)
              length += d * d;
          }
          if (write_graph)
          {
            std::ofstream ofs(fname.c_str());
            size_t i = 0;
	     //   size_t k = 0;
  BGL_FORALL_VERTICES_T(v, nn.get_graph(), graph_t)
//	{
        y_pos[i++] = yv_pos[v];
	   // std::cout<<nn.get_graph()[v].get_id()<<"=>"<<" "<<x_pos[k]<<" "<<y_pos[k]<<"\n";
	    //k++;
	//}
	opt_placement::to_svg(nn.get_graph(), x_pos, y_pos, ofs,hierar::Get_GRC(nn.get_graph()),
	fit,Params::visualisation::max_modularity_split);
            //std::cout<<"after svg"<<std::endl;
          }

	  return length;
	  return 0;
        }

        float compute_length()
        {
          _reinit_layers(); //to remove ?
          //std::cout<<"then you call compute_length() right ?"<<std::endl;
          float length = 0;
          BGL_FORALL_EDGES_T(e, this->_g, graph_t)
          {
            vertex_desc_t src = boost::source(e, this->_g);
            vertex_desc_t tgt = boost::target(e, this->_g);
            double weight = this->_g[e].get_weight().data(0);
            float d = _coords_map[src].dist(_coords_map[tgt]);
            length += d * d;
          }
          return length;
        }

        float compute_length(float min_length)
        {
          _reinit_layers(); //to remove ?
          float length = 0;
          BGL_FORALL_EDGES_T(e, this->_g, graph_t)
          {
            vertex_desc_t src = boost::source(e, this->_g);
            vertex_desc_t tgt = boost::target(e, this->_g);
            double weight = this->_g[e].get_weight().data(0);
            float l = _coords_map[src].dist(_coords_map[tgt]);
            length += l > min_length ? l : 0;
          }
          return length;
        }
        float max_length()
        {
          _reinit_layers(); //to remove ?
          float length = 0;
          BGL_FORALL_EDGES_T(e, this->_g, graph_t)
          {
            vertex_desc_t src = boost::source(e, this->_g);
            vertex_desc_t tgt = boost::target(e, this->_g);
            double weight = this->_g[e].get_weight().data(0);
            length = std::max(_coords_map[src].dist(_coords_map[tgt]), length);
            //std::cout<<_coords_map[src].dist(_coords_map[tgt])<<std::endl;
          }
          return length;
        }
template<typename G>
 void write_svg(const G& g, std::ostream& ofs)
        {
          _reinit_layers();
          std::vector<std::set<typename G::vertex_descriptor> > mods;
          float x =mod2::split(g, mods);

          ofs << "<?xml version=\"1.0\" standalone=\"no\"?>" << std::endl;
//          ofs << "<svg width=\"100%\" height=\"100%\" version=\"1.1\""
          ofs << "<svg width=\"400\" height=\"400\" version=\"1.1\""
              << " xmlns=\"http://www.w3.org/2000/svg\">"<<std::endl;
//          ofs << " <xmlns=\"http://www.w3.org/2000/svg\">" << std::endl;
          ofs << "<rect style=\"fill:#b2c4cf;fill-opacity:1;stroke:none\" "<<std::endl;
          ofs << " width=\"400\" height=\"300\" x=\"0\" y=\"1.059322\"/>"<<std::endl;
          ofs << "<g transform=\"translate(-300,-80)\">"<<std::endl;
            


          BGL_FORALL_EDGES_T(e, this->_g, graph_t)
          {
            vertex_desc_t src = boost::source(e, this->_g);
            vertex_desc_t tgt = boost::target(e, this->_g);
            float x1 = _coords_map[src].x() * 50 + 500;
            float y1 = _coords_map[src].y() * 50 + 100;
            float x2 = _coords_map[tgt].x() * 50 + 500;
            float y2 = _coords_map[tgt].y() * 50 + 100;
            double weight = this->_g[e].get_weight().data(0);
            ofs << "<line x1=\"" << x1 << "\" y1=\"" << y1 << "\" "
                << "x2=\"" << x2 << "\" y2=\"" << y2 << "\""
                << " style=\"stroke:rgb("
                //<< (weight > 0 ? "0,255,0" : "255,0,0")
                << (weight > 0 ? "0,0,0" : "0,0,0")
                << ");stroke-width:" << fabs(weight)
                << "\"/>"
                << std::endl;
          }

          BGL_FORALL_VERTICES_T(v, this->_g, graph_t)
          {
            float x = _coords_map[v].x() * 50 + 500;
            float y = _coords_map[v].y() * 50 + 100;
            ofs << "<circle cx=\"" << x << "\" cy=\"" << y
           //     << "\" r=\"10\" stroke=\"black\" stroke-width=\"1\" fill=\"#808080\"/>"<<std::endl;
                << "\" r=\"10\" stroke=\"black\" stroke-width=\"1\" fill=\"";
          ofs   <<(mods[0].find(v)!=mods[0].end() ? "#00008F" : "#FF3000");
          ofs   << "\"/>" << std::endl;

           // ofs << "<text id=\"TextElement\" x=\"" << x - 5 << "\" y=\"" << y + 5 << "\""
             //   << " style=\"font-family:Verdana;font-size:10\">"
               // << this->_g[v]._id
               // << "</text>" << std::endl;
          }
        ofs << "</g>" << std::endl;
        ofs << "</svg>" << std::endl;
        }
        const layer_t& layer(size_t n) { assert(n < _layers.size()); return _layers[n]; }
      protected:
        std::set<vertex_desc_t> _all_vertices;
        std::vector<layer_t> _layers;
        std::vector<float> _xpos, _ypos, _zpos;
        std::map<vertex_desc_t, spatial::Pos> _coords_map;
        coords_t _coords;
        layer_map_t _layer_map;
        float _rand_obj;
       
        void _make_all_vertices()
            {
      	_all_vertices.clear();
      	BGL_FORALL_VERTICES_T(v, this->_g, graph_t)
      	  _all_vertices.insert(v);
            }
        void _reset()
        {
          _coords_map.clear();
           //      _coords.clear();
          _layers.clear();
          _layer_map.clear();
          _layers.resize(nb_layers());
        }
        void _reinit_layers()
        {
         //std::cout<<"do we come here ?"<<std::endl;
         _reset();
        // std::cout<<"# of neurons: "<<this->get_nb_neurons()<<std::endl;
           // build layers
          _layers[0] = this->get_inputs();
          _layers[nb_layers() - 1] = this->get_outputs();
          for (size_t k = 0; k < this->_inputs.size(); ++k)
            _layer_map[this->_inputs[k]] = std::make_pair(0, k);
          for (size_t k = 0; k < this->_outputs.size(); ++k)
            _layer_map[this->_outputs[k]] = std::make_pair(nb_layers() - 1, k);
          int l = 1, k = 0;
          BGL_FORALL_VERTICES_T(v, this->_g, graph_t)
          if (!this->is_input(v) && !this->is_output(v))
          {
           //std::cout<<"the neurons are :"<<std::endl;
           //std::cout<<this->_g[v].get_id()<<" ";
            _layers[l].push_back(v);
            _layer_map[v] = std::make_pair(l, k);
            ++k;
            if (k == Params::dnn::spatial::layers(l))
            {
              k = 0;
              ++l;
            }
          }
           // build coords
          for (size_t i = 0, k = 0; i < _layers.size(); ++i)
            for (size_t j = 0; j < _layers[i].size(); ++j, ++k)
            {
               // if (_coords.size() != Params::dnn::spatial::x_size())
               //        {
               //          spatial::Pos p(Params::dnn::spatial::x(k),
               //                             Params::dnn::spatial::y(k),
               //                         Params::dnn::spatial::z(k));
               //           _coords.push_back(p);
               //        }
              assert(_coords.size());
              assert(k < _coords.size());
              _coords_map[_layers[i][j]] = _coords[k];
			
            }
        }
         // void _swap_neurons()
         // {
         //        size_t l = nb_layers() <= 3 ? 1 : misc::rand<int>en(1, nb_layers() - 1);
         //        int r_src = misc::rand<int>(0, _layers[l].size());
         //        int r_tgt = misc::rand<int>(0, _layers[l].size());
         //        vertex_desc_t src = _layers[l][r_src];
         //        vertex_desc_t tgt = _layers[l][r_tgt];
         //        coords_t::iterator it_src =
         //          std::find(_coords.begin(), _coords.end(), _coords_map[src]);
         //        coords_t::iterator it_tgt =
         //          std::find(_coords.begin(), _coords.end(), _coords_map[tgt]);
         //        std::swap(*it_src, *it_tgt);
         // }
        void _change_conns()
        {
          float p = Params::dnn::m_avg_weight / (float) this->get_nb_connections();

          BGL_FORALL_EDGES_T(e, this->_g, graph_t)
          if (misc::rand<float>() < p)
            this->_g[e].get_weight().mutate();
          BGL_FORALL_EDGES_T(e, this->_g, graph_t)
          if (misc::rand<float>() < Params::dnn::m_rate_change_conn)
          {
            vertex_desc_t src = source(e, this->_g);
            vertex_desc_t tgt = target(e, this->_g);
            size_t src_layer = _layer_map[src].first;
            size_t tgt_layer = _layer_map[tgt].first;

            int max_tries = num_vertices(this->_g) * num_vertices(this->_g),
                nb_tries = 0;
            if (misc::flip_coin())
              do
                src =
                  _layers[src_layer][misc::rand < int > (0, _layers[src_layer].size())];
              while (++nb_tries < max_tries && is_adjacent(this->_g, src, tgt));
            else
              do
                tgt =
                  _layers[tgt_layer][misc::rand < int > (0, _layers[tgt_layer].size())];
              while (++nb_tries < max_tries && is_adjacent(this->_g, src, tgt));
            if (nb_tries < max_tries)
            {
              typename nn_t::weight_t w = this->_g[e].get_weight();
              remove_edge(e, this->_g);
              this->add_connection(src, tgt, w);
            }
            return;
          }
        }
        void _copy_coords()
        {
	  //std::cout<<"copying coordinates"<<std::endl;
          _xpos.clear();
          _ypos.clear();
          _zpos.clear();
          _coords.clear();
          for (size_t i = 0, k = 0; i < Params::dnn::spatial::layers_size(); ++i)
            for (size_t j = 0; j < Params::dnn::spatial::layers(i); ++j, ++k)
            {
              _xpos.push_back(Params::dnn::spatial::x(k));
              _ypos.push_back(Params::dnn::spatial::y(k));
              _zpos.push_back(Params::dnn::spatial::z(k));
              _coords.push_back(spatial::Pos(_xpos[k], _ypos[k], _zpos[k]));
			  //std::cout<<_ypos[k];
            }
        }
// add only feed-forward connections
void _add_non_ff_conn()
        {
  	using namespace boost;
  	//cout<<"adding connections that skips layers \n";
  	vertex_desc_t v = this->_random_src();
  	std::set<vertex_desc_t> preds;
  	nn::bfs_pred_visitor<vertex_desc_t> vis(preds);
  	breadth_first_search(make_reverse_graph(this->_g),
  			     v, color_map(get(&N::_color, this->_g)).visitor(vis));
  	_make_all_vertices();
  	std::set<vertex_desc_t> tmp, avail, in;
  	// avoid to connect to predecessors
  	std::set_difference(_all_vertices.begin(), _all_vertices.end(),
  			    preds.begin(), preds.end(),
  			    std::insert_iterator<std::set<vertex_desc_t> >(tmp, tmp.begin()));
  	// avoid to connect to inputs
  	BOOST_FOREACH(vertex_desc_t v, this->_inputs) // inputs need
  						      // to be sorted
  	    in.insert(v);
  	std::set_difference(tmp.begin(), tmp.end(),
  			    in.begin(), in.end(),
  			    std::insert_iterator<std::set<vertex_desc_t> >(avail, avail.begin()));

  	if (avail.empty())
  	  return;
  	vertex_desc_t tgt = *misc::rand_l(avail);
  	typename nn_t::weight_t w;
  	w.random();
  	this->add_connection(v, tgt, w);
        }
void _add_skip_conn()
	{
	//std::cout<<"adding connections that can skip \n";
	vertex_desc_t src, tgt;
	  size_t max_tries = num_vertices(this->_g) * num_vertices(this->_g),
			 nb_tries = 0;
	  do
	  {
		size_t l = misc::rand<int>(0, nb_layers() - 1);
		src = _layers[l][misc::rand < int > (0, _layers[l].size())];
		size_t k = misc::rand<int>(l+1, nb_layers());
		assert (k<=nb_layers() - 1);
		tgt = _layers[k][misc::rand < int > (0, _layers[k].size())];
	  }
	  while (is_adjacent(this->_g, src, tgt) && ++nb_tries < max_tries);
	  if (nb_tries < max_tries)
	  {
		typename nn_t::weight_t w; // = nn::trait<typename
								   // nn_t::weight_t>::zero();
		 //      w.gen().data(0, 0);
		w.random();
		this->add_connection(src, tgt, w);
	  }
	}

void _add_conn()
        {
          vertex_desc_t src, tgt;
          size_t max_tries = num_vertices(this->_g) * num_vertices(this->_g),
                 nb_tries = 0;
          do
          {
            size_t l = misc::rand<int>(0, nb_layers() - 1);
            src = _layers[l][misc::rand < int > (0, _layers[l].size())];
            tgt = _layers[l + 1][misc::rand < int > (0, _layers[l + 1].size())];
          }
          while (is_adjacent(this->_g, src, tgt) && ++nb_tries < max_tries);
          if (nb_tries < max_tries)
          {
            typename nn_t::weight_t w; // = nn::trait<typename
                                       // nn_t::weight_t>::zero();
             //      w.gen().data(0, 0);
            w.random();
            this->add_connection(src, tgt, w);
          }
        }

        template<typename G>
        void _compute_all_depths(const G& g,
                                 const std::vector<vertex_desc_t>& outputs,
                                 std::map<vertex_desc_t, size_t>& final_dmap)
        {
          using namespace boost;
          typedef std::map<vertex_desc_t, size_t> int_map_t;
          typedef std::map<vertex_desc_t, vertex_desc_t> vertex_map_t;
          typedef std::map<vertex_desc_t, default_color_type> color_map_t;
          typedef std::map<edge_desc_t, int> edge_map_t;

          typedef associative_property_map<int_map_t> a_map_t;
          typedef associative_property_map<color_map_t> c_map_t;
          typedef associative_property_map<vertex_map_t> v_map_t;
          typedef associative_property_map<edge_map_t> e_map_t;

          color_map_t cm; c_map_t cmap(cm);
          vertex_map_t vm; v_map_t pmap(vm);
          edge_map_t em;
          BGL_FORALL_EDGES_T(e, g, graph_t)
          em[e] = 1;
          e_map_t wmap(em);

	  BGL_FORALL_VERTICES_T(v, g, graph_t){
	    final_dmap[v] = 1e10;
            //std::cout<<g[v].get_id()<<std::endl;
            }

          //  // we compute the shortest path between outputs and v
           /*BOOST_FOREACH(vertex_desc_t s, outputs)
           {
             int_map_t im; a_map_t dmap(im);
	     //             dag_shortest_paths(make_reverse_graph(g), s, dmap, wmap, cmap, pmap,
                                // dijkstra_visitor<null_visitor>(),
                                // std::greater<int>(),
                                // closed_plus<int>(),
                                // (std::numeric_limits<int>::min)(), 0);
	     
             BGL_FORALL_VERTICES_T(v, g, graph_t)
	       {
		 size_t d = get(dmap, v);
		 if (d <= num_vertices(g))
		   final_dmap[v] = std::min(final_dmap[v], d);
	       }
	   }*/
           size_t d = 0;
        //size_t max = 0;
          BGL_FORALL_VERTICES_T(v, g, graph_t)
         {
           d = custom_shortest_path(v, g);
           //if(d>max) max = d;
          // std::cout<<g[v].get_id()<<"=>"<<d<<"\n";
           final_dmap[v] = d;
        }
}
//Shortest path algorithm that only works on simplified feed-forward networks, created to account for a bug in the boost library.
       size_t custom_shortest_path(vertex_desc_t v, graph_t g){
        vertex_desc_t current;
        typename boost::graph_traits<graph_t>::out_edge_iterator e, e_end;
        boost::tie(e, e_end) = boost::out_edges(v, g);

         size_t result = 0;
         while(e != e_end){
         edge_desc_t vertexID = *e;
         current = boost::target(vertexID, g);
         boost::tie(e, e_end) = boost::out_edges(current, g);
        result++;
         }     
   return result;
   }
    };

  }
}

#endif
