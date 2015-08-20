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




#ifndef DNN_FF_HPP_
#define DNN_FF_HPP_

#include <modules/nn2/gen_dnn.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/dag_shortest_paths.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/graph/reverse_graph.hpp>
#include <boost/property_map/vector_property_map.hpp>
#include <modules/modularity/modularity.hpp>
#include <modules/modularity/hierarchy.hpp>
#include "../exp/modularity/opt_placement.hpp"

namespace sferes
{
  namespace gen
  {
  namespace spatial
  {
    class LRCPos
    {
      public:
        LRCPos() {
        }
        LRCPos(float x, float y, float z) : _x(x), _y(y), _z(z) {
        }
        float dist(const LRCPos& p) const
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
        bool operator == (const LRCPos &p)
        { return _x == p._x && _y == p._y && _z == p._z; }
      protected:
        float _x, _y, _z;
    };
  }
    template<typename N, typename C, typename Params>
    class DnnFF : public Dnn<N, C, Params>
    {
    public:
      typedef nn::NN<N, C> nn_t;
      typedef N neuron_t;
      typedef C conn_t;
      typedef typename nn_t::io_t io_t;
      typedef typename nn_t::vertex_desc_t vertex_desc_t;
      typedef typename nn_t::edge_desc_t edge_desc_t;
      typedef typename nn_t::graph_t graph_t;
      //visualization purpose
      typedef std::vector<vertex_desc_t> layer_t;
      typedef std::vector<spatial::LRCPos> coords_t;
      typedef std::map<vertex_desc_t, std::pair<int, int> > layer_map_t;
      std::map<int, std::vector<std::string> > levels;
      std::map<double, int> y_s;

      DnnFF() {}
      DnnFF& operator=(const DnnFF& o)
      {
	static_cast<nn::NN<N, C>& >(*this)
	  = static_cast<const nn::NN<N, C>& >(o);
	return *this;
      }
      DnnFF(const DnnFF& o)
      { *this = o; }
      void init()
      {
	Dnn<N, C, Params>::init();
	_compute_depth();
      }
      void random()
      {
	assert(Params::dnn::init == dnn::ff);
        //std::cout<<"calling _random (6)  \n";
        this->_random(Params::dnn::nb_inputs, Params::dnn::nb_outputs,
		Params::dnn::min_nb_neurons, Params::dnn::max_nb_neurons,
		Params::dnn::min_nb_conns, Params::dnn::max_nb_conns);
      //this->_random_ff(Params::dnn::nb_inputs, Params::dnn::nb_outputs);
        //this->random_ff_noncom(Params::dnn::nb_inputs, Params::dnn::nb_outputs);
	_make_all_vertices();
      //std::cout<<"number of neurons is: "<<_all_vertices.size()<<std::endl;
      }
      void mutate()
      {
       //std::cout<<"is this the mutatio \n";
 	_change_conns();
	this->_change_neurons();

 	if (misc::rand<float>() < Params::dnn::m_rate_add_conn)
 	  _add_conn();

 	if (misc::rand<float>() < Params::dnn::m_rate_del_conn)
  	   this->_del_conn();

  	//std::cout<<"Params::dnn::m_rate_add_neuron: "<<Params::dnn::m_rate_add_neuron<<"\n";
        //if (misc::rand<float>() < Params::dnn::m_rate_add_neuron && this->get_nb_neurons() < Params::dnn::max_nb_neurons)
  	  //this->_add_neuron_on_conn();

 	//if (misc::rand<float>() < Params::dnn::m_rate_del_neuron)
   	  
      //    this->_del_neuron();
      }

      void cross(const DnnFF& o, DnnFF& c1, DnnFF& c2)
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
      size_t get_depth() const { return _depth; }
      // computes and returns the number of levles
      int nb_layers() const { return levels.size(); }
      //building layers for visualization
 void build_levels(std::map<std::string, std::pair<double,int> >& neuron_coords)
 {
	 std::vector<double> temp;
	 std::map<std::string, std::pair<double,int> >::iterator it;
	 for (it = neuron_coords.begin();it!=neuron_coords.end();it++)
	 {
		 //std::cout<<it->first<<"=>"<<it->second.first<<" "<<it->second.second<<"\n";
		 levels[it->second.second].push_back(it->first);
		// temp.push_back(it->second.second);
	 }
//	 std::map<int, std::vector<std::string> >:: iterator lv;
//
//	for (lv =levels.begin(); lv!= levels.end();++lv)
//	 {
//      std::cout<<lv->first<<" ";
//      for (int k =0;k<lv->second.size();k++)
//    	  std::cout<<lv->second[k];
//      std::cout<<"\n";
//	 }
 }

 void visualize(std::vector<double>& _x, std::vector<double>& _y,std::map<std::string, std::pair<double,int> > & neuron_coords, float fit)
{
	std::cout<<"this method will visualize \n";
	nn_t nn = *this;
	  std::vector<vertex_desc_t> to_remove;
	  nn.simplify(); //WARNING: suppress some subgraphs !
	 /* BGL_FORALL_VERTICES_T(v, nn.get_graph(), graph_t)
	  if (boost::in_degree(v, nn.get_graph())
		  + boost::out_degree(v, nn.get_graph()) == 0)
		  to_remove.push_back(v);

	  for (size_t i = 0; i < to_remove.size(); ++i)
		remove_vertex(to_remove[i], nn.get_graph());*/

 typename nn_t::graph_t gsimp =nn.get_graph();
 //double _grc = hierar::Get_GRC(gsimp); // was a test.
 //std::cout<<"GRC : "<<_grc<<std::endl; // was a test
 std::cout<<"number of nodes: "<<nn.get_nb_neurons()<<std::endl;
 //get coordinate for neurons in gsimp based on their LRC
 hierar::get_coords(gsimp,_x,_y, neuron_coords);
 //after the coordinates are determined form layers out of the simplified network
 build_levels(neuron_coords);
 build_layer(nn, neuron_coords);
 //then form the x,y,z coordinates for all vertices. This is being done to hack ONP
 form_coords(nn, neuron_coords);
 // now produce the optimal placement
 ONP (nn,neuron_coords, fit);
}
nn_t simplified_nn()
	{
//std::cout<<"before copying in simplified_nn() # of neurons is :"<<this->get_nb_neurons()<<std::endl;
	  nn_t nn = *this;
//std::cout<<"# of neurons in nn_t simplified_nn(): "<<nn.get_nb_neurons()<<std::endl;
//        remove not-connected vertices
	  std::vector<vertex_desc_t> to_remove;
	  nn.simplify(); //WARNING: suppress some subgraphs !
	  BGL_FORALL_VERTICES_T(v, nn.get_graph(), graph_t)
	  if (boost::in_degree(v, nn.get_graph())
		  + boost::out_degree(v, nn.get_graph()) == 0)
		  to_remove.push_back(v);

	  for (int i = 0; i < to_remove.size(); ++i)
		remove_vertex(to_remove[i], nn.get_graph());
//std::cout<<"size of to_remove in nn_t simplified_nn(): "<<to_remove.size()<<std::endl;
	  return nn;
	}
//places neurons in their optimal placements
void ONP (nn_t simp_nn,std::map<std::string, std::pair<double,int> >  neuron_coords,float fit,bool write_graph = true,std::string fname="nn_opt.svg")
{
  // rebuild i/o and copy coordinates
  std::vector<vertex_desc_t> inputs, outputs;
  std::vector<float> x_in, y_in, x_out, y_out;

if (simp_nn.get_nb_neurons() == 0 || simp_nn.get_nb_connections() == 0)
		    return;

BGL_FORALL_VERTICES_T(v, simp_nn.get_graph(), graph_t)
	if (simp_nn.get_graph()[v].get_in() != -1)
	  {
		inputs.push_back(v);
		x_in.push_back(_xpos[simp_nn.get_graph()[v].get_in()]);
		y_in.push_back(_ypos[simp_nn.get_graph()[v].get_in()]);
	  }
    else if (simp_nn.get_graph()[v].get_out() != -1)
     {
       outputs.push_back(v);
       int o = simp_nn.get_graph()[v].get_out();
       //int n = _xpos.size() - _layers[_layers.size() - 1].size();
       int n = _xpos.size() - levels[levels.size()- 1].size();
       assert(n + o < _xpos.size());
       assert(n + o < _ypos.size());
       x_out.push_back(_xpos[n + o]);
       y_out.push_back(_ypos[n + o]);
     }
//  // rebuild y coords (only if write_graph)
std::map<vertex_desc_t, float> yv_pos; // for the svg file
  if (write_graph)
  {
    int nb_layers = levels.size() - 1;
    std::map<vertex_desc_t, int> dmap;
    _compute_all_depths(simp_nn.get_graph(), outputs, dmap);
    std::cout<<"\n after the _compute_All_depth \n";

    for (typename std::map<vertex_desc_t, int>::iterator it = dmap.begin();  it != dmap.end(); ++it)
    {

    	vertex_desc_t v = simp_nn.get_neuron_label(levels[nb_layers - it->second][0]);
    			//yv_pos[it->first] = _coords_map[_layers[nb_layers - it->second][0]].y();
        std::cout<<simp_nn.get_graph()[it->first].get_id()<<" "<<it->second<<" "
        		<<_coords_map[v].y()<<" "<<levels[nb_layers - it->second][0]<<'\n';
    }
  }
  //compute the optimal placement
   Eigen::VectorXf x_pos= opt_placement::compute(simp_nn.get_graph(),
                                                  inputs, x_in,
                                                 outputs, x_out);

   Eigen::VectorXf y_pos = opt_placement::compute(simp_nn.get_graph(),
                                                  inputs, y_in,
                                                  outputs, y_out);

   std::cout<<"\n after opt placemet, neurons postion is \n";


 if (write_graph)
 {
 //std::string fname="/project/EvolvingAI/hmengist/non_ff_simp/PCC100/nn"+_postFix+".svg";
 std::ofstream ofs(fname.c_str());
   int i = 0;
   BGL_FORALL_VERTICES_T(v, simp_nn.get_graph(), graph_t) //layer y pos
   {
	   //y_pos[i++] = yv_pos[v];
	   y_pos[i] =  neuron_coords[simp_nn.get_graph()[v].get_id()].second;
	   x_pos[i] =  neuron_coords[simp_nn.get_graph()[v].get_id()].first;
	   i++;
   }
//   int p = 0;
//   BGL_FORALL_VERTICES_T(v, simp_nn.get_graph(), graph_t)
//   {
//      std::cout<<simp_nn.get_graph()[v].get_id()<<" x: "<<x_pos[p]<<" y: "<<y_pos[p]<<"\n";
//      p++;
//   }
opt_placement::to_svg(simp_nn.get_graph(), x_pos, y_pos, ofs,hierar::Get_GRC(simp_nn.get_graph()),fit);
 }
}
int custom_shortest_path(vertex_desc_t v, graph_t g){
 vertex_desc_t current;
 typename boost::graph_traits<graph_t>::out_edge_iterator e, e_end;
 boost::tie(e, e_end) = boost::out_edges(v, g);

  int result = 0;
  while(e != e_end){
  edge_desc_t vertexID = *e;
  current = boost::target(vertexID, g);
  boost::tie(e, e_end) = boost::out_edges(current, g);
 result++;
  }
return result;
}
    protected:
      std::set<vertex_desc_t> _all_vertices;
      size_t _depth;
      //visualizations purpose
    std::vector<layer_t> _layers;
	std::vector<float> _xpos, _ypos, _zpos;
	std::map<vertex_desc_t, spatial::LRCPos> _coords_map;
	coords_t _coords;
	layer_map_t _layer_map;

    void _reset()
    {
      _coords_map.clear();
       //_coords.clear();
      _layers.clear();
      _layer_map.clear();
      _layers.resize(nb_layers());
    }
 void build_layer(  nn_t simp_nn, std::map<std::string, std::pair<double,int> > & neuron_coords)
   {
     _reset();
     _layers.resize(nb_layers());
     //std::cout<<"before \n";
     std::cout<< "the number of levels :"<<nb_layers()<<'\n';

     //std::cout<<"the size is of layer :"<<_layers.size()<<"and "<<_layers[0].size()<<'\n';

   //  BOOST_FOREACH(vertex_desc_t v, simp_nn.get_inputs())
   //   //std::cout<<simp_nn.get_graph()[v].get_id();
   //    _layers[0].push_back(v);
     //std::cout<< "\n after \n";

    // _layers[0] = simp_nn.get_inputs();
    // _layers[nb_layers() - 1] = simp_nn.get_outputs();


     //std::cout<<"layer 0 neurons: \n";

    // for (int i =0;i<_layers[0].size();i++)
   	//  std::cout<<simp_nn.get_graph()[_layers[0][i]].get_id()<<" ";

    // std::cout<<" last layer "<< simp_nn.get_graph()[_layers[_layers.size() - 1][0]].get_id()<<'\n';

//     for (int i =0; i < nb_layers(); ++i)
//     {
//       std::vector<vertex_desc_t> layer;
//       std::cout<<"\n forming layer "<<i<<"\n";
//       for (int j = 0; j < levels[i].size(); ++j)
//       {
//         //vertex_desc_t v =   this->add_neuron("n" + boost::lexical_cast<std::string>(i));
//       	std::cout<<levels[i][j]<<" ";
//    	   vertex_desc_t v = simp_nn.get_neuron_label(levels[i][j]);
//           layer.push_back(v);
//           std::cout<<simp_nn.get_graph()[v].get_id()<<" ";
//          _layer_map[v] = std::make_pair(i, j);
//       }
//       std::cout<<"size of layer "<<i<<" is: "<<layer.size();
//       _layers.push_back(layer);
//     }
//   std::cout<<'\n';
//   std::cout<<"layer 0 neurons: \n";
//
//   for (int i =0;i<_layers[0].size();i++)
//   	  std::cout<<simp_nn.get_graph()[_layers[0][i]].get_id()<<" ";
//
//   std::cout<<" last layer "<< simp_nn.get_graph()[_layers[_layers.size() - 1][0]].get_id()<<'\n';


   }
    //form
 void form_coords( nn_t simp_nn,std::map<std::string, std::pair<double,int> >  neuron_coords)
 {
  _xpos.clear();
  _ypos.clear();
  _zpos.clear();
  _coords.clear();
  _coords_map.clear();
  std::map<int, std::vector<std::string> >:: iterator lv;

for (lv =levels.begin(); lv!= levels.end();++lv)
{
   std::cout<<lv->first<<" ";
   for (int k =0;k<lv->second.size();k++)
	{
	   float x = neuron_coords[lv->second[k]].first;
	   _xpos.push_back(x);
	   _ypos.push_back(lv->first);

	    vertex_desc_t v = simp_nn.get_neuron_label(lv->second[k]);
	   if (simp_nn.get_graph()[v].get_out() != -1)
	     _zpos.push_back(2);
	   else
	     _zpos.push_back(0);

	   _coords.push_back(spatial::LRCPos(_xpos[k], _ypos[k], _zpos[k]));
	   _coords_map[v]= _coords[k];
	}
}

 //   for (int i = 0, k = 0; i < _layers.size(); ++i)
//      {
 //   	  std::cout<<"layer :"<<i;
//       for (int j = 0; j < _layers[i].size(); ++j, ++k)
 //        {
 //           std::cout<<" "<<simp_nn.get_graph()[_layers[i][j]].get_id()<<" ";
//          float x = neuron_coords[simp_nn.get_graph()[_layers[i][j]].get_id()].first;
//          _xpos.push_back(x);
//          float y = neuron_coords[simp_nn.get_graph()[_layers[i][j]].get_id()].second;
//          _ypos.push_back(y);
//          std::cout<<simp_nn.get_graph()[_layers[i][j]].get_id()<<" X: "<<_xpos[k]<<" Y: "<<_ypos[k]<<'\n';
//          if (simp_nn.get_graph()[_layers[i][j]].get_out() != -1)
//            _zpos.push_back(2);
//          else
//        	  _zpos.push_back(0);
//
//          _coords.push_back(spatial::LRCPos(_xpos[k], _ypos[k], _zpos[k]));
//          _coords_map[_layers[i][j]]= _coords[k];
 //       }
    //  std::cout<<"A test: "<< simp_nn.get_graph()[_layers[_layers.size() - 1][0]].get_out()<<'\n';
 //     std::cout<<"\n";
 //   }
  }

template<typename G>
void _compute_all_depths(const G& g,const std::vector<vertex_desc_t>& outputs,std::map<vertex_desc_t, int>& final_dmap)
{
  using namespace boost;
  BGL_FORALL_VERTICES_T(v, g, graph_t)
    final_dmap[v] = 1e10;

  int d = 0;
    //size_t max = 0;
 BGL_FORALL_VERTICES_T(v, g, graph_t)
  {
      d = custom_shortest_path(v, g);
     //    if(d>max) max = d;
         final_dmap[v] = d;
  }
}
  void _make_all_vertices()
      {
	_all_vertices.clear();
	BGL_FORALL_VERTICES_T(v, this->_g, graph_t)
	  _all_vertices.insert(v);
      }
      void _change_conns()
      {
        float p = Params::dnn::m_avg_weight / (float) this->get_nb_connections();
        BGL_FORALL_EDGES_T(e, this->_g, graph_t)
          if (misc::rand<float>() < p)
             this->_g[e].get_weight().mutate();
      }


      // add only feed-forward connections
      void _add_conn()
      {
	using namespace boost;
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

      // useful to make the right number of steps
void _compute_depth()
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
	BGL_FORALL_EDGES_T(e, this->_g, graph_t)
	  em[e] = 1;
	e_map_t wmap(em);
	_depth = 0;
	// we compute the longest path between inputs and outputs
	BOOST_FOREACH(vertex_desc_t s, this->_inputs)
	  {
	    int_map_t im; a_map_t dmap(im);
 	    dag_shortest_paths
 	      (this->_g, s, dmap, wmap, cmap, pmap,
	       dijkstra_visitor<null_visitor>(),
 	       std::greater<int>(),
	       closed_plus<int>(),
	       std::numeric_limits<int>::min(), 0);

 	    BGL_FORALL_VERTICES_T(v, this->_g, graph_t)
	      {
		size_t d = get(dmap, v);
		if (this->_g[v].get_out() != -1 && d <= num_vertices(this->_g))
		  _depth = std::max(_depth, d);
	      }
	  }
	// add one to be sure
	_depth ++;
 }
    };

  }
}

#endif
