#ifndef IGRAPH_HPP_
#define IGRAPH_HPP_

#include <boost/graph/reverse_graph.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/dag_shortest_paths.hpp>
#include <boost/graph/visitors.hpp>
#include <boost/property_map/vector_property_map.hpp>



namespace igraph {

  struct Node {
    Node() :
      _id(_rand_id()) {
    }
    const std::string& get_id() const {
      return _id;
    }
    int get_in() const {
      if (_id[0] != 'i')
        return -1;
      return get_io(_id);
    }
    int get_out() const {
      if (_label[0] != 'o')
        return -1;
      return get_io(_id);
    }
    const std::string& get_type() const {
      return _id;
    }
    const std::string& get_label() const {
      return _label;
    }

    std::string _id;
    std::string _label;
    std::string _type;

    int _color; //for algos
   protected:
    int get_io(const std::string& s) const {
      std::string k = _label.substr(1, _label.size() - 1);
      return boost::lexical_cast<int>(k);
    }
    std::string _rand_id() const {
      unsigned r = rand();
      return boost::lexical_cast<std::string>(r);
    }
  };

  struct Connection {
    Connection() : _weight(std::rand() / ((float) RAND_MAX + 1)) {}
    float _weight;
    float get_weight() const {
      return _weight;
    }
  };

  typedef std::vector< std::pair<int, int> > gen_t;
  typedef boost::adjacency_list<boost::listS, boost::listS,
          boost::bidirectionalS,
          Node, Connection> graph_t;
  typedef boost::graph_traits<graph_t>::vertex_iterator vertex_it_t;
  typedef boost::graph_traits<graph_t>::edge_iterator edge_it_t;
  typedef boost::graph_traits<graph_t>::out_edge_iterator out_edge_it_t;
  typedef boost::graph_traits<graph_t>::in_edge_iterator in_edge_it_t;
  typedef boost::graph_traits<graph_t>::edge_descriptor edge_desc_t;
  typedef boost::graph_traits<graph_t>::vertex_descriptor vertex_desc_t;
  typedef boost::graph_traits<graph_t>::adjacency_iterator adj_it_t;
  typedef std::vector<vertex_desc_t> vertex_list_t;
  typedef unsigned char byte_t;

  static graph_t load(const std::string& fname) {
    std::ifstream ifs(fname.c_str());
    assert(ifs.good());

    graph_t g;
    boost::dynamic_properties dp;
    dp.property("node_id", get(&Node::_id, g));
    dp.property("label", get(&Node::_label, g));
    dp.property("type", get(&Node::_type, g));
    dp.property("label", get(&Connection::_weight, g));

    boost::read_graphviz(ifs, g, dp, "node_id");
    return g;
  }

  template<typename G>
  static void write(const G& g, std::ostream& ofs) {
    ofs << "digraph G {" << std::endl;
    BGL_FORALL_VERTICES_T(v, g, G) {
      ofs << g[v].get_id();
      ofs << " [label=\"" << g[v].get_id() << "\"";
      ofs << "]" << std::endl;
    }
    BGL_FORALL_EDGES_T(e, g, G) {
      ofs << g[source(e, g)].get_id()
          << " -> "
          << g[target(e, g)].get_id()
          << "[label=\"" << g[e].get_weight() << "\"]"
          << std::endl;
    }
    ofs << "}" << std::endl;
  }

// NOT Thread-safe !
  namespace misc {
    template<typename T>
    inline T rand(T max = 1.0) {
      assert(max > 0);
      T v;
      do
        v = T(((double)max * ::rand()) / (RAND_MAX + 1.0));
      while (v >= max); // this strange case happened... precision problem?
      assert(v < max);
      return v;
    }


    template<typename T>
    inline T rand(T min, T max) {
      assert(max != min);
      assert(max > min);
      T res = T(rand<double>() * ((long int) max - (long int) min) + min);
      assert(res >= min);
      assert(res < max);
      return res;
    }

  }
  template <typename G>
  typename boost::graph_traits<G>::vertex_descriptor
  random_vertex(G& g) {
    using namespace boost;
    if (num_vertices(g) > 1) {
      std::size_t n = misc::rand(num_vertices(g));
      typename graph_traits<G>::vertex_iterator i = vertices(g).first;
      while (n-- > 0) ++i;
      return *i;
    } else
      return *vertices(g).first;
  }

  template <class G>
  typename boost::graph_traits<G>::edge_descriptor
  random_edge(G& g) {
    using namespace boost;
    if (num_edges(g) > 1) {
      std::size_t n = misc::rand(num_edges(g));
      typename graph_traits<G>::edge_iterator i = edges(g).first;
      while (n-- > 0) ++i;
      return *i;
    } else
      return *edges(g).first;
  }



  // the graph HAS TO be layered
  // you need to call simplify before !
  template<typename G>
  static std::map<typename G::vertex_descriptor, int> compute_layers(const G& g) {
    using namespace boost;
    std::vector<vertex_desc_t> outputs;
    BGL_FORALL_VERTICES(v, g, graph_t)
    if (g[v].get_out() != -1)
      outputs.push_back(v);

    typedef reverse_graph<G> r_graph_t;
    typedef std::map<vertex_desc_t, size_t> int_map_t;
    typedef std::map<vertex_desc_t, vertex_desc_t> vertex_map_t;
    typedef std::map<vertex_desc_t, default_color_type> color_map_t;
    typedef std::map<edge_desc_t, int> edge_map_t;
    typedef associative_property_map<int_map_t> a_map_t;
    typedef associative_property_map<color_map_t> c_map_t;
    typedef associative_property_map<vertex_map_t> v_map_t;
    typedef associative_property_map<edge_map_t> e_map_t;

    std::map<vertex_desc_t, int> res;

    BGL_FORALL_VERTICES(s, g, graph_t) {
      color_map_t cm;
      c_map_t cmap(cm);
      vertex_map_t vm;
      v_map_t pmap(vm);
      edge_map_t em;
      BGL_FORALL_EDGES_T(e, g, graph_t) {
        em[e] = 1;
      }
      e_map_t wmap(em);
      int_map_t im;
      a_map_t dmap(im);
      dag_shortest_paths
      (g, s, dmap, wmap, cmap, pmap,
       dijkstra_visitor<null_visitor>(),
       std::greater<int>(),
       closed_plus<int>(),
       std::numeric_limits<int>::min(), 0);
      BOOST_FOREACH(vertex_desc_t o, outputs) {
        if (get(dmap, o) != std::numeric_limits<int>::min())
          res[s] = get(dmap, o);
      }
    }
    return res;
  }

  template<typename V>
  class bfs_pred_visitor : public boost::default_bfs_visitor {
   public:
    bfs_pred_visitor(std::set<V>&pred) : _pred(pred) {
    }
    template<typename Vertex, typename Graph >
    void discover_vertex(Vertex u, const Graph& g) {
      _pred.insert(u);
    }
   protected:
    std::set<V>& _pred;
  };

// remove neurons that are not connected to both one input an
// one output (this is NOT callled automatically in NN
//
// WARNING: if simplify_in is true, this can change the behavior
// of neurons since neurons not connected to inputs but connected
// to outputs can output a constant value
//
// principle : keep the neurons that are successors of inputs
// and predecessors of outputs
  static void simplify(graph_t& g, bool simplify_in = false) {
    using namespace boost;
    // we need sets and not lists withouh io
    std::set<vertex_desc_t> all_neurons;
    std::vector<vertex_desc_t> inputs, outputs;
    BGL_FORALL_VERTICES(v, g, graph_t)
    if (g[v].get_out() != -1)
      outputs.push_back(v);
    else if (g[v].get_in() != -1)
      inputs.push_back(v);
    else
      all_neurons.insert(v);
    std::set<vertex_desc_t> out_preds, in_succs;

    // out
    BOOST_FOREACH(vertex_desc_t v, outputs) {
      std::set<vertex_desc_t> preds;
      bfs_pred_visitor<vertex_desc_t> vis(preds);
      breadth_first_search(boost::make_reverse_graph(g), v,
                           color_map(get(&Node::_color, g)).visitor(vis));
      out_preds.insert(preds.begin(), preds.end());
    }
    // in
    if (simplify_in)
      BOOST_FOREACH(vertex_desc_t v, inputs) {
      std::set<vertex_desc_t> succs;
      bfs_pred_visitor<vertex_desc_t> vis(succs);
      breadth_first_search(g, v, color_map(get(&Node::_color, g)).visitor(vis));
      in_succs.insert(succs.begin(), succs.end());
    } else
      in_succs = all_neurons;
    // make the intersection of in_succ and out_preds
    std::set<vertex_desc_t> valid_neurons;
    std::set_intersection(in_succs.begin(), in_succs.end(),
                          out_preds.begin(), out_preds.end(),
                          std::insert_iterator<std::set<vertex_desc_t> >(valid_neurons,
                              valid_neurons.begin()));
    // get the list of neurons that are NOT in valid_neurons
    std::set<vertex_desc_t> to_remove;
    std::set_difference(all_neurons.begin(), all_neurons.end(),
                        valid_neurons.begin(), valid_neurons.end(),
                        std::insert_iterator<std::set<vertex_desc_t> >(to_remove,
                            to_remove.begin()));

    // remove these neurons
    BOOST_FOREACH(vertex_desc_t v, to_remove) {
      clear_vertex(v, g);
      remove_vertex(v, g);
    }
    to_remove.clear();

    // also remove disconnected i/o
    BGL_FORALL_VERTICES(v, g, graph_t)
    if (in_degree(v, g) == 0 && out_degree(v, g) == 0)
      to_remove.insert(v);
    BOOST_FOREACH(vertex_desc_t v, to_remove) {
      clear_vertex(v, g);
      remove_vertex(v, g);
    }
  }
}


#endif
