#ifndef C_MODULARITY_HPP_
#define C_MODULARITY_HPP_

template<typename NN>
float compute_mod_simplified(const NN& nn_)
{
  // copy
  NN nn = nn_;
  // remove useless subgraphs
  //nn.simplify();
  // remove low weights
  std::list<typename NN::edge_desc_t> to_remove;
  BGL_FORALL_EDGES_T(e, nn.get_graph(), typename NN::graph_t)
    {
      typename NN::weight_t weight = nn.get_graph()[e].get_weight();
      float w = nn::trait<typename NN::weight_t>::single_value(weight);
      if (fabs(w) < 0.05)// here is the threshold
	to_remove.push_back(e);
    }
  // we need to do this to avoid any edge invalidation
  while (!to_remove.empty())
    {
      BGL_FORALL_EDGES_T(e, nn.get_graph(), typename NN::graph_t)
	if (to_remove.front() == e)
	  {
	    remove_edge(e, nn.get_graph());
	    to_remove.pop_front();
	    break;
	  }
    }
  // compute the Q score
  float q = mod::modularity(nn.get_graph());
  return q;  
}

#endif
