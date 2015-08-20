#ifndef _OPT_PLACEMENT_HPP_
#define _OPT_PLACEMENT_HPP_

#include <iostream>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/foreach.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/reverse_graph.hpp>
#include <Eigen/Core>
#include <Eigen/LU>

namespace opt_placement {
  template<typename G>
  inline bool _linked(const G& g,
                      const typename G::vertex_descriptor& v1,
                      const typename G::vertex_descriptor& v2) {
    BGL_FORALL_OUTEDGES_T(v1, e, g, G)
    if (target(e, g) == v2)
      return true;
    return false;
  }

  // consider _only_ verts in the verts set
  template<typename G>
  inline Eigen::MatrixXf _laplacian(const G& g,
                                    const std::set<typename G::vertex_descriptor>& verts) {
    using namespace Eigen;
    using namespace boost;

    Eigen::MatrixXf l = MatrixXf::Zero(num_vertices(g), num_vertices(g));
    size_t i = 0;
    BGL_FORALL_VERTICES_T(v1, g, G) {
      size_t j = 0;
      BGL_FORALL_VERTICES_T(v2, g, G) {
        if (verts.find(v1) == verts.end()
            || verts.find(v2) == verts.end())
          l(i, j) = 0;
        else if (i == j)
          l(i, j) = in_degree(v1, g) + out_degree(v1, g);
        else if (_linked(g, v1, v2) || _linked(g, v2, v1))
          l(i, j) = -1;
        else
          l(i, j) = 0;
        ++j;
      }
      ++i;
    }
    return l;
  }
  // include in the adjacency iff source _or_ target is in verts
  template<typename G>
  inline Eigen::MatrixXf _adjacency(const G& g,
                                    const std::set<typename G::vertex_descriptor>& verts) {
    using namespace Eigen;
    using namespace boost;

    Eigen::MatrixXf a = MatrixXf::Zero(num_vertices(g), num_vertices(g));
    size_t i = 0;
    BGL_FORALL_VERTICES_T(v1, g, G) {
      size_t j = 0;
      BGL_FORALL_VERTICES_T(v2, g, G) {
        if ((verts.find(v1) != verts.end()
             || verts.find(v2) != verts.end())
            && (_linked(g, v1, v2) || _linked(g, v2, v1)))
          a(i, j) = 1;
        else
          a(i, j) = 0;
        ++j;
      }
      ++i;
    }
    return a;
  }

  // include in the degree matrix iff vertex is in verts
  template<typename G>
  inline Eigen::MatrixXf _degree(const G& g,
                                 const std::set<typename G::vertex_descriptor>& verts) {
    using namespace Eigen;
    using namespace boost;

    Eigen::MatrixXf d = MatrixXf::Zero(num_vertices(g), num_vertices(g));
    size_t i = 0;
    BGL_FORALL_VERTICES_T(v, g, G) {
      if (verts.find(v) != verts.end())
        d(i, i) = in_degree(v, g) + out_degree(v, g);
      ++i;
    }
    return d;
  }


  template<typename V, typename T>
  inline int _find_index(const V& vect, const T& e) {
    for (size_t i = 0; i < vect.size(); ++i)
      if (e == vect[i])
        return i;
    return -1;
  }
  template<typename G>
  inline Eigen::VectorXf _f(const G& g,
                            const std::vector<typename G::vertex_descriptor>& inputs,
                            const std::vector<float>& coords_inputs,
                            const std::vector<typename G::vertex_descriptor>& outputs,
                            const std::vector<float>& coords_outputs) {
    using namespace boost;
    Eigen::VectorXf f = Eigen::VectorXf::Zero(num_vertices(g));
    size_t i = 0, i_in = 0, i_out = 0;
    assert(inputs.size() == coords_inputs.size());
    assert(outputs.size() == coords_outputs.size());

    BGL_FORALL_VERTICES_T(v, g, G) {
      int in = _find_index(inputs, v);
      assert(in < (int)coords_inputs.size());
      if (in != -1)
        f(i) = coords_inputs[in];
      int out = _find_index(outputs, v);
      assert(out < (int)coords_outputs.size());
      if (out != -1)
        f(i) = coords_outputs[out];
      ++i;

    }
    return f;
  }

  template<typename G>
  inline Eigen::VectorXf compute(const G& g,
                                 const std::vector<typename G::vertex_descriptor>& inputs,
                                 const std::vector<float>& coords_inputs,
                                 const std::vector<typename G::vertex_descriptor>& outputs,
                                 const std::vector<float>& coords_outputs) {
    using namespace Eigen;
    typedef typename G::vertex_descriptor v_d_t;
    std::set<v_d_t> all_set, io_set, no_io_set;
    BGL_FORALL_VERTICES_T(v, g, G) all_set.insert(v);
    BOOST_FOREACH(v_d_t v, inputs) io_set.insert(v);
    BOOST_FOREACH(v_d_t v, outputs) io_set.insert(v);

    if (inputs.size() == 0)
      std::cout << "no input";
    if (outputs.size() == 0)
      std::cout << "no output";


    std::set_difference(all_set.begin(), all_set.end(),
                        io_set.begin(), io_set.end(),
                        std::insert_iterator<std::set<v_d_t> >(no_io_set,
                            no_io_set.begin()));

    MatrixXf l = _laplacian(g, no_io_set);
    MatrixXf b = _adjacency(g, io_set);
    MatrixXf d = _degree(g, io_set);
    VectorXf f = _f(g, inputs, coords_inputs, outputs, coords_outputs);

    VectorXf r = (l + d).inverse() * b * f;

    // clean up r ??
    int i = 0;
    BGL_FORALL_VERTICES_T(v, g, G) {
      if (_find_index(inputs, v) != -1 || _find_index(outputs, v) != -1)
        r(i) = f(i);
      ++i;
    }

    // std::cout << "l\n" << l << std::endl
    //           << "b\n" << b << std::endl
    //           << "d\n" << d << std::endl
    //           << "f\n" << f.transpose() << std::endl
    //           << "r\n" << r.transpose() << std::endl
    //           << "num edge" << num_edges(g) << std::endl;
    return r;
  }

  template<typename G>
  inline void write_svg(const G& g,
                        const Eigen::VectorXf& x_coords,
                        const Eigen::VectorXf& y_coords,
                        std::ofstream& ofs) {
    typedef typename G::vertex_descriptor v_t;

    ofs << "<?xml version=\"1.0\" standalone=\"no\"?>" << std::endl;
    ofs << "<svg width=\"500\" height=\"300\" version=\"1.1\""
        << " xmlns=\"http://www.w3.org/2000/svg\">"
        << "<g transform=\"translate(-225,-80)\">" // WARNING translation
        << std::endl;


    std::map<v_t, float> x_map, y_map;
    size_t i = 0;
    Eigen::VectorXf x_c = x_coords;
    bool done = false;
    int k = 20000;
    while (!done && k > 0) {
      done = true;
      for (size_t i = 0; i < x_c.size(); ++i)
        for (size_t j = 0; j < x_c.size(); ++j)
          if (i != j && fabs(x_c(i) - x_c(j)) < 0.7
              && fabs(y_coords(i) - y_coords(j)) < 0.5) {
            float m = (rand() / ((float)RAND_MAX + 1)) * 0.5 - 0.25;
            x_c(i) = x_c(i) + m;
            done = false;
          }
      --k;
    }
    BGL_FORALL_VERTICES_T(v, g, G) {
      x_map[v] = x_c(i);
      y_map[v] = y_coords(i);
      ++i;
    }
    BGL_FORALL_EDGES_T(e, g, G) {
      v_t src = boost::source(e, g);
      v_t tgt = boost::target(e, g);
      float x1 = x_map[src] * 50 + 500;
      float y1 = y_map[src] * 50 + 100;
      float x2 = x_map[tgt] * 50 + 500;
      float y2 = y_map[tgt] * 50 + 100;
      double weight = g[e]._weight;

      ofs << "<line x1=\"" << x1 << "\" y1=\"" << y1 << "\" "
          << "x2=\"" << x2 << "\" y2=\"" << y2 << "\""
          << " style=\"stroke:"
          << (weight > 0 ? "#9cbd86" : "#c83737")
          << ";stroke-width:" << fabs(weight * 4)
          << "\"/>"
          << std::endl;
    }
    BGL_FORALL_VERTICES_T(v, g, G) {
      float x = x_map[v] * 50 + 500;
      float y = y_map[v] * 50 + 100;
      ofs << "<circle cx=\"" << x << "\" cy=\"" << y
          << "\" r=\"20\" stroke=\"black\" stroke-width=\"1\" fill=\"";
      if (g[v].get_in() != -1)
        ofs << "#9cbd86";
      else if (g[v].get_out() != -1)
        ofs << "#c83737";
      else if (g[v]._type == "m")
        ofs << "#aaaaaa";
      else
        ofs << "#7882b3";
      ofs << "\"/>" << std::endl;
      ofs << "<text id=\"TextElement\" x=\"" << x << "\" y=\"" << y + 5 << "\""
          <<
          " style=\"font-family:Verdana;font-weight:bold;font-size:20;fill:black;text-anchor:middle\">"
          << g[v]._label
          << "</text>" << std::endl;
    }
    ofs << "</g>" << std::endl;
    ofs << "</svg>" << std::endl;
  }

  // the function you probably want to call
  template<typename G>
  inline void opt_svg(const G& g, std::ofstream& ofs) {
    typedef typename G::vertex_descriptor v_t;

    float q = 0;
    // coordinates of i/o
    std::vector<v_t> inputs, outputs;
    std::vector<float> x_in, y_in, x_out, y_out;
    int k = 1, l = 1;
    BGL_FORALL_VERTICES_T(v, g, G)
    if (g[v].get_in() != -1)
      inputs.push_back(v);
    else if (g[v].get_out() != -1)
      outputs.push_back(v);

    for (int i = 0; i < inputs.size(); ++i) {
      x_in.push_back(i - inputs.size() / 2.0);
      y_in.push_back(0);
    }

    for (int i = 0; i < outputs.size(); ++i) {
      x_out.push_back(i - outputs.size() / 2.0);
      y_out.push_back(5);
    }

    // compute the optimal placement
    Eigen::VectorXf x_coords = opt_placement::compute(g, inputs, x_in, outputs, x_out);
    Eigen::VectorXf y_coords = opt_placement::compute(g, inputs, y_in, outputs, y_out);

    write_svg(g, x_coords, y_coords, ofs);
  }

}
#endif
