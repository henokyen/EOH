#ifndef _MODULARITY_HPP_
#define _MODULARITY_HPP_


#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/foreach.hpp>
#include <gsl/gsl_math.h>
#include <gsl/gsl_eigen.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>

#include "igraph.hpp"

// Community Structure in Directed Networks
// EA Leicht, MEJ Newman - Physical Review Letters, 2008 - APS
// http://arxiv.org/pdf/0709.4500

namespace mod {
  namespace null_model {
    // the null model used by Leicht
    struct Directed {
      template<typename G>
      double operator()(const G& g,
                        const typename G::vertex_descriptor& v1,
                        const typename G::vertex_descriptor& v2) const {
        size_t m = num_edges(g);
        size_t k_i_in = in_degree(v1, g);
        size_t k_j_out = out_degree(v2, g);
        return k_i_in * k_j_out / (double) m;
      }
    };

    // the null model used by Newman
    struct Undirected {
      template<typename G>
      double operator()(const G& g,
                        const typename G::vertex_descriptor& v1,
                        const typename G::vertex_descriptor& v2) const {
        size_t m = num_edges(g);
        size_t k_i_in = degree(v1, g);
        size_t k_j_out = degree(v2, g);
        return k_i_in * k_j_out / (double) m;
      }
    };

    // for layered networks (e.g. multi-layer perceptron)
    // the map contains the id of the layer for each node
    // WARNING: the last layer is 0!
    template<typename G>
    struct Layered {
      typedef std::map<typename G::vertex_descriptor, int> map_t;
      Layered(const G& g, const map_t & m) : _m(m) {
        // find the max layer
        int cm = -1;
        for (typename map_t::const_iterator it = m.begin(); it != m.end(); ++it)
          if (it->second > cm)
            cm = it->second;
        _layers.resize(cm + 1);
        for (typename map_t::const_iterator it = m.begin(); it != m.end(); ++it)
          _layers[it->second].push_back(it->first);
        _num_edges.resize(_layers.size());
        for (size_t i = 0; i < _layers.size(); ++i) {
          int k = 0;
          for (size_t j = 0; j < _layers[i].size(); ++j)
            k += out_degree(_layers[i][j], g);
          _num_edges[i] = k;
        }
      }

      double operator()(const G& g,
                        const typename G::vertex_descriptor& v1,
                        const typename G::vertex_descriptor& v2) const {
        int k1 = _m.find(v1)->second;
        int k2 = _m.find(v2)->second;
        if (k1 <= k2)
          return 0;
        size_t m = _num_edges[k1];
        size_t k_j_out = out_degree(v1, g);
        size_t k_i_in = in_degree(v2, g);
        return k_i_in * k_j_out / (double) m;
      }
      map_t _m;
      std::vector<std::vector<typename G::vertex_descriptor> > _layers;
      std::vector<int> _num_edges;
    };
  }

  template<typename G1, typename G2>
  void convert_graph(const G1& src, G2& g,
                     std::map <
                     typename boost::graph_traits<G2>::vertex_descriptor,
                     typename boost::graph_traits<G1>::vertex_descriptor > & rmap) {
    using namespace boost;
    typedef typename graph_traits<G2>::vertex_descriptor vertex_desc_t;
    std::map<typename graph_traits<G1>::vertex_descriptor, vertex_desc_t> neuron_map;
    BGL_FORALL_VERTICES_T(v, src, G1) {
      vertex_desc_t d = add_vertex(g);
      neuron_map[v] = d;
      rmap[d] = v;
    }

    BGL_FORALL_EDGES_T(v, src, G1)
    add_edge(neuron_map[source(v, src)],
             neuron_map[target(v, src)],
             g);
  }

  template<typename G>
  inline bool _linked(const G& g,
                      const typename G::vertex_descriptor& v1,
                      const typename G::vertex_descriptor& v2) {
    BGL_FORALL_OUTEDGES_T(v1, e, g, G)
    if (target(e, g) == v2)
      return true;
    return false;
  }



  template<typename G, typename NullModel>
  inline void _b_matrix(const G& g, gsl_matrix *& b, const NullModel& null_model) {

    b = gsl_matrix_alloc(num_vertices(g), num_vertices(g));

    size_t m = num_edges(g);
    size_t i = 0;
    BGL_FORALL_VERTICES_T(v1, g, G) {
      size_t j = 0;
      BGL_FORALL_VERTICES_T(v2, g, G) {
        size_t k_i_in = in_degree(v1, g);
        size_t k_j_out = out_degree(v2, g);
        double b_ij = _linked(g, v1, v2) - null_model(g, v1, v2);
        gsl_matrix_set(b, i, j, b_ij);
        ++j;
      }
      ++i;
    }
  }

  // res = b^T + b, allocate res
  static void _bbt(gsl_matrix*b, gsl_matrix *& res) {
    gsl_matrix*bt = gsl_matrix_alloc(b->size1, b->size2);
    res = gsl_matrix_alloc(b->size1, b->size2);
    gsl_matrix_memcpy(res, b);

    gsl_matrix_transpose_memcpy(bt, b);
    gsl_matrix_add(res, bt);
    gsl_matrix_free(bt);
  }

  // split in two according to b
  static void _split_eigen(gsl_matrix*b, size_t n, std::vector<int>& s) {
    assert(n);
    s.resize(n);
    gsl_eigen_symmv_workspace *w = gsl_eigen_symmv_alloc(n);
    gsl_vector *eval = gsl_vector_alloc(n);
    gsl_matrix *evec = gsl_matrix_alloc(n, n);
    gsl_matrix *b_copy = gsl_matrix_alloc(n, n);
    gsl_matrix_memcpy(b_copy, b);
    gsl_eigen_symmv(b_copy, eval, evec, w);

    gsl_eigen_symmv_sort(eval, evec, GSL_EIGEN_SORT_VAL_DESC);
    for (size_t i = 0; i < n; ++i)
      s[i] = (gsl_matrix_get(evec, i, 0) > 0 ? 1 : -1);

    gsl_eigen_symmv_free(w);
    gsl_vector_free(eval);
    gsl_matrix_free(evec);
    gsl_matrix_free(b_copy);
    assert(s.size() == n);
  }

  static void _bg(gsl_matrix*b, const std::vector<size_t>& sg1,
                  gsl_matrix *& bg) {
    size_t n1 = sg1.size();
    assert(n1);
    bg = gsl_matrix_alloc(n1, n1);
    for (size_t i = 0; i < n1; ++i)
      for (size_t j = 0; j < n1; ++j) {
        double b_ij = gsl_matrix_get(b, sg1[i], sg1[j]);
        if (i == j)
          for (size_t k = 0; k < n1; ++k)
            b_ij -= gsl_matrix_get(b, sg1[i], sg1[k]);
        gsl_matrix_set(bg, i, j, b_ij);
      }
  }


  double _delta_q(gsl_matrix*bbt, const std::vector<int>& sg, size_t m) {
    // 1/(4 * m) * s^T * bbt * s
    assert(sg.size());
    gsl_matrix*sv = gsl_matrix_alloc(sg.size(), 1);
    gsl_matrix*svt = gsl_matrix_alloc(1, sg.size());
    gsl_matrix*tmp1 = gsl_matrix_alloc(sg.size(), 1);
    gsl_matrix*tmp2 = gsl_matrix_alloc(1, 1);

    for (size_t i = 0; i < sg.size(); ++i) {
      gsl_matrix_set(sv, i, 0, sg[i]);
      gsl_matrix_set(svt, 0, i, sg[i]);
    }
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans,
                   1.0, bbt, sv,
                   0.0, tmp1);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans,
                   1.0, svt, tmp1,
                   0.0, tmp2);
    double delta_q = 1.0 / (4.0 * m) * gsl_matrix_get(tmp2, 0, 0);

    gsl_matrix_free(sv);
    gsl_matrix_free(svt);
    gsl_matrix_free(tmp1);
    gsl_matrix_free(tmp2);

    return delta_q;
  }

  void _split(gsl_matrix*b, const std::vector<size_t>& sg, size_t m,
              std::vector<std::string>& modules) {
    assert(sg.size());

    gsl_matrix*bbt, *bg;
    _bg(b, sg, bg);
    _bbt(bg, bbt);
    std::vector<int> s;
    _split_eigen(bbt, sg.size(), s);

    double delta_q = _delta_q(bbt, s, m);
    if (delta_q <= 1e-5) {
      gsl_matrix_free(bbt);
      gsl_matrix_free(bg);
      return;
    }

    // new sg
    std::vector<size_t> sg1, sg2;
    assert(sg.size() == s.size());
    for (size_t i = 0; i < s.size(); ++i)
      if (s[i] == -1) {
        modules[sg[i]] += '0';
        sg1.push_back(sg[i]);
      } else {
        modules[sg[i]] += '1';
        sg2.push_back(sg[i]);
      }

    gsl_matrix_free(bbt);
    gsl_matrix_free(bg);

    if (sg1.size() > 0 && sg2.size() > 0) {
      _split(b, sg1, m, modules);
      _split(b, sg2, m, modules);
    }

  }


  template<typename G, typename NullModel>
  float compute_modularity(const G& g,
                           const std::vector<std::string>& modules,
                           const NullModel& null_model) {
    double m = boost::num_edges(g);
    double q = 0.0;
    size_t v1_i = 0;
    BGL_FORALL_VERTICES_T(v1, g, G) {
      size_t v2_i = 0;
      BGL_FORALL_VERTICES_T(v2, g, G) {
        if (modules[v1_i] == modules[v2_i]) {
          double b_ij = _linked(g, v1, v2) - null_model(g, v1, v2);
          q += b_ij;
        }
        ++v2_i;
      }
      ++v1_i;
    }
    return q / m;
  }

  template<typename G, typename NullModel>
  inline void split(const G& g,
                    std::vector<std::string>& modules,
                    const NullModel& null_model) {
    size_t m = num_edges(g);
    gsl_matrix*b, *bbt;

    _b_matrix(g, b, null_model);
    _bbt(b, bbt);

    size_t n = num_vertices(g);
    std::vector<int> s;
    _split_eigen(bbt, n, s);

    size_t n1 = std::count(s.begin(), s.end(), -1);
    size_t n2 = n - n1;

    modules.resize(num_vertices(g));
    std::vector<size_t> sg1(n1), sg2(n2);
    size_t j1 = 0, j2 = 0;
    for (size_t i = 0; i < n; ++i)
      if (s[i] == -1) {
        modules[i] += '0';
        assert(j1 < sg1.size());
        sg1[j1] = i;
        ++j1;
      } else {
        modules[i] += '1';
        assert(j2 < sg2.size());
        sg2[j2] = i;
        ++j2;
      }

    if (sg1.size() > 0 && sg2.size() > 0) {
      _split(b, sg1, m, modules);
      _split(b, sg2, m, modules);
    }
    gsl_matrix_free(b);
    gsl_matrix_free(bbt);
  }

  template<typename G>
  void _h_modules(const std::set<typename boost::graph_traits<G>::vertex_descriptor>& nodes,
                  const std::vector<std::string>& ids,
                  std::vector<std::set<typename boost::graph_traits<G>::vertex_descriptor> >& mods,
                  size_t n) {
    assert(nodes.size());
    typedef typename boost::graph_traits<G>::vertex_descriptor vertex_desc_t;
    std::set<vertex_desc_t> s1, s2;
    BOOST_FOREACH(vertex_desc_t v, nodes) {
      if (n >= ids[v].size()) {
        assert(s1.empty());
        assert(s2.empty());
        return;
      }

      if (ids[v][n] == '0')
        s1.insert(v);
      else
        s2.insert(v);
    }
    mods.push_back(s1);
    mods.push_back(s2);
    assert(s1.size());
    assert(s2.size());
    if (s1.size())
      _h_modules<G>(s1, ids, mods, n + 1);
    if (s2.size())
      _h_modules<G>(s2, ids, mods, n + 1);
  }

  // ALL the modules (high and low levels)
  template<typename G, typename NullModel>
  double h_modules(const G& g_origin,
                   std::vector<std::set<typename boost::graph_traits<G>::vertex_descriptor> >& mods_r,
                   const NullModel& null_model) {
    using namespace boost;
    typedef adjacency_list<vecS, vecS, bidirectionalS> graph_t;
    typedef graph_traits<graph_t>::vertex_descriptor vertex_desc_t;

    graph_t g;
    std::vector<std::set<vertex_desc_t> > mods;
    std::map<vertex_desc_t, typename boost::graph_traits<G>::vertex_descriptor> rmap;
    convert_graph(g_origin, g, rmap);

    assert(num_vertices(g) != 0);
    std::vector<std::string> ids;
    split(g, ids, null_model);
    typedef std::set<vertex_desc_t> vset_t;
    vset_t nodes;

    BGL_FORALL_VERTICES_T(v, g, graph_t)
    nodes.insert(v);

    assert(nodes.size());
    _h_modules<graph_t>(nodes, ids, mods, 0);

    float m = compute_modularity(g, ids, null_model);
    // create mod_r
    mods_r.resize(mods.size());
    for (size_t i = 0; i < mods.size(); ++i)
      BOOST_FOREACH(vertex_desc_t v, mods[i])
      mods_r[i].insert(rmap[v]);
    return m;
  }

  // true if set is included in Vm
  template<typename S>
  bool _includes(const S& s, const std::vector<S>& vm) {
    for (size_t i = 0; i < vm.size(); ++i)
      if (s != vm[i] &&
          std::includes(vm[i].begin(), vm[i].end(), s.begin(), s.end()))
        return true;
    return false;
  }

  template<typename G, typename Vm>
  void _write_modules(const G& g, const Vm& vm, std::ostream& os) {
    Vm vv;
    // find the sets that are not included in another set
    for (size_t i = 0; i < vm.size(); ++i)
      if (!_includes(vm[i], vm))
        vv.push_back(vm[i]);
    // split the vm list
    std::vector<Vm> n_vm(vv.size());
    for (size_t k = 0; k < vv.size(); ++k)
      for (size_t i = 0; i < vm.size(); ++i)
        if (vm[i] != vv[k] &&
            std::includes(vv[k].begin(), vv[k].end(),
                          vm[i].begin(), vm[i].end()))
          n_vm[k].push_back(vm[i]);

    // recurse
    for (size_t i = 0; i < n_vm.size(); ++i) {
      os << "subgraph cluster_" << rand() << " {" << std::endl;
      if (!n_vm[i].empty())
        _write_modules(g, n_vm[i], os);
      else
        for (typename Vm::value_type::const_iterator it = vv[i].begin();
             it != vv[i].end(); ++it)
          os << g[*it]._id << std::endl;
      os << "}" << std::endl;
    }
  }


  // hierachical dot file (slow version)
  template<typename G, typename NullModel>
  void write_modules(const G& g, std::ostream& os, const NullModel& null_model) {
    typedef std::vector<std::set<typename boost::graph_traits<G>::vertex_descriptor> >
    v_mod_t;
    v_mod_t mods_r;
    h_modules(g, mods_r, null_model);
    os << "digraph G {" << std::endl;
    _write_modules(g, mods_r, os);
    BGL_FORALL_EDGES_T(e, g, G) {
      os << g[source(e, g)]._id
         << " -> " << g[target(e, g)]._id
         << "[label=\"" << g[e].get_weight() << "\"]" << std::endl;
    }

    os << "}" << std::endl;
  }


  // post-processing : modules in a more convenient form (list of sets)
  // ONLY leaf-modules (not the 'higher-level' modules'
  // TODO : convert graph !
  template<typename G, typename NullModel>
  inline double modules(
    const G& g,
    std::vector<std::vector<typename boost::graph_traits<G>::vertex_descriptor> >& mods,
    const NullModel& null_model) {
    std::vector<std::string> modules;
    split(g, modules, null_model);

    typedef typename boost::graph_traits<G>::vertex_descriptor vertex_desc_t;
    typedef std::map<std::string, std::vector<vertex_desc_t> > m_map_t;
    m_map_t m_map;
    size_t i = 0;
    BGL_FORALL_VERTICES_T(v, g, G)
    m_map[modules[i++]].push_back(v);

    // copy to res
    mods.resize(m_map.size());
    size_t k = 0;
    for (typename m_map_t::const_iterator it = m_map.begin();
         it != m_map.end();
         ++it) {
      BOOST_FOREACH(vertex_desc_t v, it->second)
      mods[k].push_back(v);
      ++k;
    }

    float q = compute_modularity(g, modules, null_model);
    return q;
  }

  template<typename G, typename NullModel>
  inline double modularity(const G& g, const NullModel& null_model) {
    std::vector<std::vector<typename boost::graph_traits<G>::vertex_descriptor> > mods;
    return modules(g, mods, null_model);
  }



  template<typename G, typename NullModel>
  std::pair<double, double> random(const G& g, const NullModel& null_model,
                                   size_t k = 1000, size_t k2 = 50) {
    using namespace boost;
    typedef typename boost::graph_traits<G>::edge_descriptor edge_desc_t;
    typedef typename boost::graph_traits<G>::vertex_descriptor vertex_desc_t;

    double m = 0;
    double mm = 0;
    for (size_t i = 0; i < k; ++i) {
      if (i % 100 == 0) {
        std::cout << "[" << i << "] ";
        std::cout.flush();
      }
      G rg = g;
      for (size_t j = 0; j < num_vertices(rg) * k2; ++j) {
        edge_desc_t e1 = igraph::random_edge(rg);
        edge_desc_t e2 = igraph::random_edge(rg);
        if (e1 == e2)
          continue;
        vertex_desc_t src_1 = source(e1, rg);
        vertex_desc_t src_2 = source(e2, rg);
        vertex_desc_t tgt_1 = target(e1, rg);
        vertex_desc_t tgt_2 = target(e2, rg);
        remove_edge(e1, rg);
        remove_edge(e2, rg);
        add_edge(src_1, tgt_2, rg);
        add_edge(src_2, tgt_1, rg);
      }
      double q = modularity(rg, null_model);
      m += q;
      mm = std::max(mm, q);
    }
    return std::make_pair(m / k, mm);
  }


  // the source network (g) has to be layered!
  template<typename G>
  std::pair<double, double> random_feedforward(const G& g, size_t k = 1000, size_t k2 = 50) {
    using namespace boost;
    typedef typename boost::graph_traits<G>::edge_descriptor edge_desc_t;
    typedef typename boost::graph_traits<G>::vertex_descriptor vertex_desc_t;
    typedef std::map<typename G::vertex_descriptor, int> map_t;
    // reconstruct layers
    map_t layer_map_g = compute_layers(g);
    int cm = -1;
    for (typename map_t::const_iterator it = layer_map_g.begin(); it != layer_map_g.end(); ++it)
      if (it->second > cm)
        cm = it->second;
    std::vector<std::vector<edge_desc_t> > edge_list(cm);
    double m = 0;
    double mm = 0;

    for (size_t i = 0; i < k; ++i) {
      if (i % 100 == 0) {
        std::cout << "[" << i << "] ";
        std::cout.flush();
      }
      G rg = g;
      map_t layer_map = compute_layers(rg);

      for (size_t j = 0; j < num_vertices(rg) * k; ++j) {
        for (size_t t = 0; t < edge_list.size(); ++t)
          edge_list[t].clear();
        BGL_FORALL_EDGES_T(e, rg, G)
        edge_list[cm - layer_map[source(e, rg)]].push_back(e);
        int n = igraph::misc::rand<int>(0, edge_list.size());
        int r1 = igraph::misc::rand<int>(0, edge_list[n].size());
        int r2 = igraph::misc::rand<int>(0, edge_list[n].size());
        edge_desc_t e1 = edge_list[n][r1];
        edge_desc_t e2 = edge_list[n][r2];
        if (e1 == e2)
          continue;
        vertex_desc_t src_1 = source(e1, rg);
        vertex_desc_t src_2 = source(e2, rg);
        vertex_desc_t tgt_1 = target(e1, rg);
        vertex_desc_t tgt_2 = target(e2, rg);
        remove_edge(e1, rg);
        remove_edge(e2, rg);
        add_edge(src_1, tgt_2, rg);
        add_edge(src_2, tgt_1, rg);
      }
      double q = modularity(rg, null_model::Layered<igraph::graph_t>(rg, layer_map));
      m += q;
      mm = std::max(mm, q);
    }
    return std::make_pair(m / k, mm);
  }

}

#endif
