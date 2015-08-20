#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE opt_placement


#include <boost/test/unit_test.hpp>
#include <modules/nn2/nn.hpp>
#include <boost/assign/list_of.hpp>
#include <fstream>

#include "opt_placement.hpp"

BOOST_AUTO_TEST_CASE(basic)
{
  using namespace nn;
  typedef NN<Neuron<PfWSum<>, AfTanh<float> >, Connection<> > nn_t;
  typedef nn_t::vertex_desc_t v_t;
  nn_t nn;
  nn.set_nb_inputs(4);
  nn.set_nb_outputs(2);

  std::vector<v_t> vertices;
  for (size_t i = 0; i < 8; ++i)
    vertices.push_back(nn.add_neuron("n"));
  nn.add_connection(nn.get_input(0), vertices[0], 0);
  nn.add_connection(nn.get_input(1), vertices[3], 0);
  nn.add_connection(nn.get_input(2), vertices[1], 0);
  nn.add_connection(nn.get_input(3), vertices[2], 0);

  nn.add_connection(vertices[0], vertices[4], 0);
  nn.add_connection(vertices[1], vertices[5], 0);
  nn.add_connection(vertices[2], vertices[7], 0);
  nn.add_connection(vertices[3], vertices[6], 0);

  nn.add_connection(vertices[4], nn.get_output(0), 0);
  nn.add_connection(vertices[5], nn.get_output(1), 0);
  nn.add_connection(vertices[6], nn.get_output(0), 0);
  nn.add_connection(vertices[7], nn.get_output(1), 0);
  using namespace boost::assign;
  std::vector<float> x_inputs = list_of(0) (1) (2) (3);
  std::vector<float> x_outputs = list_of(0.5) (4);
  std::vector<float> y_inputs = list_of(0) (0) (0) (0);
  std::vector<float> y_outputs = list_of(2) (2);

  Eigen::MatrixXf res_x = opt_placement::compute(nn.get_graph(),
                                                 nn.get_inputs(), x_inputs,
                                                 nn.get_outputs(), x_outputs);
  Eigen::MatrixXf res_y = opt_placement::compute(nn.get_graph(),
                                                 nn.get_inputs(), y_inputs,
                                                 nn.get_outputs(), y_outputs);

  std::ofstream ofs("nn.svg");
  opt_placement::to_svg(nn.get_graph(), res_x, res_y, ofs);
}

