#ifndef _MODULARITY2_HPP_
#define _MODULARITY2_HPP_

#include <Eigen/Core>
#include <Eigen/Array>
#include <Eigen/QR>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/iteration_macros.hpp>
#include <boost/foreach.hpp>


// Community Structure in Directed Networks
// EA Leicht, MEJ Newman - Physical Review Letters, 2008 - APS
// http://arxiv.org/pdf/0709.4500
namespace mod2
{

  template<typename G>
  inline bool _linked(const G& g,
                      const typename G::vertex_descriptor& v1,
                      const typename G::vertex_descriptor& v2)
  {
    BGL_FORALL_OUTEDGES_T(v1, e, g, G)
    if (target(e, g) == v2)
      return true;
    return false;
  }

  template<typename G>
  static Eigen::MatrixXf _b_matrix(const G& g)
  {
    Eigen::MatrixXf b = Eigen::MatrixXf::Zero(num_vertices(g), num_vertices(g));
    size_t m = num_edges(g);
    size_t i = 0;
    BGL_FORALL_VERTICES_T(v1, g, G)
    {
      size_t j = 0;
      BGL_FORALL_VERTICES_T(v2, g, G)
      {
        size_t k_i_in = in_degree(v1, g);
        size_t k_j_out = out_degree(v2, g);
        b(i, j) = _linked(g, v1, v2) - k_i_in * k_j_out / (double) m;
        ++j;
      }
      ++i;
    }
    return b;
  }

  template<typename G>
  static Eigen::VectorXf _split_eigen(const G& g)
  {
    USING_PART_OF_NAMESPACE_EIGEN;

    MatrixXf b = _b_matrix(g);
    MatrixXf bbt = b + b.transpose();
    Eigen::EigenSolver<MatrixXf> eigen_solver(bbt);

    MatrixXf evec = eigen_solver.eigenvectors().real();
    MatrixXf eval = eigen_solver.eigenvalues().real();
    int k = -1; float pi = -1e10;
    for (size_t i = 0; i < eval.size(); ++i)
      if (eval(i) > 0 && eval(i) > pi)
      {
        k = i;
        pi = eval(i);
      }
    VectorXf v = evec.col(k);
    VectorXf s(boost::num_vertices(g));
    assert(v.size() == s.size());
    for (size_t i = 0; i < s.size(); ++i)
      s(i) = v(i) > 0 ? 1 : -1;
    return s;
  }

  template<typename G>
  float split(const G& g,
              std::vector<std::set<typename G::vertex_descriptor> >& mods)
  {
    Eigen::VectorXf s = _split_eigen(g);
    mods.resize(2);
    size_t i = 0;
    BGL_FORALL_VERTICES_T(v, g, G)
    {
      if (s(i) == -1)
        mods[0].insert(v);
      else
        mods[1].insert(v);
      ++i;
    }
    float q = 1.0f / (2.0f * boost::num_edges(g)) * (s.transpose() * _b_matrix(g)).dot(s);
    return q;
  }

}

#endif
