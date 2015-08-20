
#include <boost/test/unit_test.hpp>
#include <modules/nn/nn.hpp>

#include "modularity.hpp"

int main(int argc, char **argv)
{
  assert(argc == 2);

  using namespace nn;
  typedef NN<Neuron<PfWSum, AfSigmoid>, Connection> nn_t;
  {
    nn_t nn;
    std::cout<<"created"<<std::endl;
    nn.load(argv[1]);
    std::cout<<"loaded"<<std::endl;
    std::vector<std::set<nn_t::vertex_desc_t> > mods;
    float mod1 = mod::h_modules(nn.get_graph(), mods);
    std::cout<<"modularity:"<<mod1<<std::endl;
    BOOST_FOREACH(std::set<nn_t::vertex_desc_t> s, mods)
      {
	std::cout<<"---"<<std::endl;
	BOOST_FOREACH(nn_t::vertex_desc_t v, s)
	  std::cout<<nn.get_graph()[v]._id<<std::endl;
      }
  }
}
