#include <iostream>
#include <boost/program_options.hpp>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <Eigen/Dense>

#include "game.hpp"

namespace po = boost::program_options;

int main( int argc, char *argv[] )
{
  // parse command line options
  po::options_description desc("Command line options");
  desc.add_options()
    ("help,h", "display help message");

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help") > 0) {
    std::cout << desc << std::endl;
    return EXIT_SUCCESS;
  }

  // start game
  nomi::game app;
  app.run();

  return EXIT_SUCCESS;
}