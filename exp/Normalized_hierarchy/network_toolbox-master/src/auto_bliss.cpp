#include <iostream>
#include <fstream>
#include <limits>
#include <algorithm>

#include <boost/random.hpp>
#include <boost/math/special_functions/factorials.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/unordered_set.hpp>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/properties.hpp>
#include <boost/graph/random.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/foreach.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/reverse_graph.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/graph/graph_utility.hpp>


#include <bliss/defs.hh>
#include <bliss/graph.hh>
#include <bliss/timer.hh>
#include <bliss/utils.hh>

#include "igraph.hpp"

namespace gsym {

  typedef unsigned char byte_t;
  typedef std::vector< std::pair<int, int> > gen_t;

  static const byte_t ZERO = 0;
  static const byte_t NEG_LARGE = 1;
  static const byte_t NEG_SMALL = 2;
  static const byte_t NO_LINK = 3;
  static const byte_t POS_SMALL = 4;
  static const byte_t POS_LARGE = 5;

  static const byte_t TYPE_IO = 100;
  static const byte_t TYPE_N = 101;
  static const byte_t TYPE_M = 102;

  byte_t _color(float w) {
    if (w <= -1)
      return NEG_LARGE;
    else if (w < 0)
      return NEG_SMALL;
    else if (w < 1)
      return POS_SMALL;
    else
      return POS_LARGE;
  }


  byte_t _color_v(const std::string& type) {
    if (type == "io")
      return TYPE_IO;
    else if (type == "m")
      return TYPE_M;
    else return TYPE_N;
  }

  std::vector<std::pair<int, int> >
  _make_generator(const unsigned int N,
                  const unsigned int*perm,
                  const unsigned int offset) {
    std::vector<std::pair<int, int> > res;
    for (unsigned int i = 0; i < N; i++) {
      unsigned int j = perm[i];
      if (j == i)
        continue;
      bool is_first = true;
      while (j != i) {
        if (j < i) {
          is_first = false;
          break;
        }
        j = perm[j];
      }
      if (!is_first)
        continue;
      j = perm[i];
      while (j != i) {
        res.push_back(std::make_pair(i + offset, j + offset));
        j = perm[j];
      }
    }
    return res;
  }

  static void _report_aut(void *param, unsigned int n, const unsigned int *aut) {
    assert(param);
    std::vector<gen_t>*all = static_cast<std::vector<gen_t> *>(param);
    gen_t v = _make_generator(n, aut, 0);
    all->push_back(v);
    for (size_t i = 0; i < v.size(); ++i)
      std::cout << v[i].first << "->" << v[i].second << ";";
    std::cout << std::endl;
  }

  std::vector<int> _apply_gen(const std::vector<int>& state, const gen_t& gen) {
    std::vector<int> c = state;
    for (size_t i = 0; i < gen.size(); ++i) {
      c[gen[i].first] = gen[i].second;
      c[gen[i].second] = gen[i].first;
    }
    return c;
  }

  template<typename G>
  void write(const std::string& fname, G& g) {
    using namespace boost;
    std::ofstream ofs(fname.c_str());
    assert(ofs.good());

    ofs << "digraph g {" << std::endl;
    BGL_FORALL_VERTICES_T(v, g, G)
    ofs << "\"" << v << "\"" << "[label=" << g[v].get_label() << "]" << std::endl;
    BGL_FORALL_EDGES_T(e, g, G)
    ofs << "\"" << source(e, g) << "\" -> \""
        << target(e, g) << "\" [label=" << g[e].get_weight() << "]" << std::endl;
    ofs << "}";
  }



  template<typename G>
  bliss::Digraph _convert(const G& g, bool use_weights = true) {
    bliss::Digraph dg;
    std::map<typename G::vertex_descriptor, unsigned> mv;

    BGL_FORALL_VERTICES_T(v, g, G) {
      std::cout << g[v].get_id() << std::endl;
      mv[v] = dg.add_vertex(_color_v(g[v].get_type()));
    }
    BGL_FORALL_EDGES_T(e, g, G) {
      if (use_weights) {
        // "virtual" node to carry the weight
        unsigned v = dg.add_vertex(_color(g[e].get_weight()));
        dg.add_edge(mv[boost::source(e, g)], v);
        dg.add_edge(v, mv[boost::target(e, g)]);
      } else
        dg.add_edge(mv[boost::source(e, g)], mv[boost::target(e, g)]);
    }
    return dg;
  }

  template<typename G>
  double compute(const G& g) {
    // automorphisms and generators
    bliss::Digraph dg = _convert(g, false);
    dg.set_splitting_heuristic(bliss::Digraph::shs_flm);
    bliss::Stats stats;
    std::vector<gen_t> v;
    dg.find_automorphisms(stats, &_report_aut, &v);

    std::vector<int> labels(boost::counting_iterator<int>(0),
                            boost::counting_iterator<int>(boost::num_vertices(g)));

    boost::unordered_set<std::vector<int> > res;
    std::cout << "res size:" << res.size() << std::endl;

    //fixed points
    std::set<int> nfix;
    for (size_t i = 0; i < v.size(); ++i)
      for (size_t j = 0; j < v[i].size(); ++j) {
        nfix.insert(v[i][j].first);
        nfix.insert(v[i][j].second);
      }
    std::vector<int> fixed_points;
    for (size_t i = 0; i < labels.size(); ++i)
      if (nfix.find(labels[i]) == nfix.end())
        fixed_points.push_back(labels[i]);
    for (size_t i = 0; i < fixed_points.size(); ++i)
      std::cout << "fixed:" << fixed_points[i] << std::endl;

    double regularity =
      (boost::num_vertices(g) - fixed_points.size()) / (double)boost::num_vertices(g);
    double aut = stats.get_group_size_approx();
    std::cout << "stats.get_group_size_approx() :" << aut << std::endl;
    return aut; //log(aut / boost::math::factorial<double>(boost::num_vertices(g)));
  }
}

int main(int argc, char **argv) {
  if (argc == 2) {
    igraph::graph_t g = igraph::load(argv[1]);
    std::cout << gsym::compute(g) << std::endl;
  } else if (argc == 3) {
    int num_vertices = boost::lexical_cast<int>(argv[1]);
    int num_edges = boost::lexical_cast<int>(argv[2]);
    boost::mt19937 gen(time(0));
    igraph::graph_t gr;
    boost::generate_random_graph(gr, num_vertices, num_edges, gen, false);
    std::cout << "(random graph)=>" << gsym::compute(gr) << std::endl;
    gsym::write("random.dot", gr);
  } else {
    assert(0);
  }
  return 0;
}


