// code (c) J.-B. Mouret - mouret@isir.upmc.fr
// GPL licence
#include <boost/program_options.hpp>
#include <fstream>
#include <cstdlib>

#include "opt_placement.hpp"
#include "igraph.hpp"



std::pair<std::string, std::string> parse_options(int argc, char**argv) {
  namespace po = boost::program_options;
  po::options_description desc("Allowed options");
  desc.add_options()
  ("help,h", "print this help message")
  ("input,i", po::value<std::string>(), "input file (dot)")
  ("output,o", po::value<std::string>(), "output file (svg)");

  po::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
  } catch (po::error & e) {
    std::cerr << "error: " << e.what() << std::endl;
    std::cerr << desc << std::endl;
    exit(1);
  }
  if (!vm.count("output")) {
    std::cerr << "please specify an output file [-o]" << std::endl;
    exit(1);
  }
  if (!vm.count("input")) {
    std::cerr << "please specify an input file [-i]" << std::endl;
    exit(1);
  }
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    exit(0);
  }
  return std::make_pair(vm["input"].as<std::string>(),
                        vm["output"].as<std::string>());
}

int main(int argc, char **argv) {
  auto r = parse_options(argc, argv);
  auto g = igraph::load(r.first);
  std::cout << "load ok" << std::endl;
  igraph::simplify(g, true);
  std::cout << "simplify ok" << std::endl;
  std::ofstream ofs(r.second);
  opt_placement::opt_svg(g, ofs);
  std::cout << r.second << " successfully written" << std::endl;
  return 0;
}
