#define BOOST_TEST_DYN_LINK 
#define BOOST_TEST_MODULE modularity


#include <boost/test/unit_test.hpp>
#include <modules/nn/nn.hpp>

#include "modularity.hpp"

BOOST_AUTO_TEST_CASE(modularity)
{
  double mod1, mod2, mod3;
  typedef nn::NN<nn::PfWSum, nn::AfSigmoid> nn_t;
  {
    nn_t nn;
    nn.load("modules/modularity/test1.dot");
    std::vector<std::vector<nn_t::vertex_desc_t> > mods;
    mod1 = mod::modules(nn.get_graph(), mods);
    BOOST_CHECK_EQUAL(mods.size(), (size_t) 2);
  }
  {
    nn_t nn;
    nn.load("modules/modularity/test2.dot");
    std::vector<std::vector<nn_t::vertex_desc_t> > mods;
    mod2 = mod::modules(nn.get_graph(), mods);
    BOOST_CHECK_EQUAL(mods.size(), (size_t) 4);
  }

  {
    nn_t nn;
    nn.load("modules/modularity/test3.dot");
    std::vector<std::vector<nn_t::vertex_desc_t> > mods;
    mod3 = mod::modules(nn.get_graph(), mods);
    BOOST_CHECK_EQUAL(mods.size(), (size_t) 2);
  }

  BOOST_CHECK(mod2 > mod1);
  BOOST_CHECK(mod1 > mod3);
}

