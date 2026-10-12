#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

#include "game.hpp"

namespace
{

void printHelp( const char* exe )
{
    std::cout << "Usage: " << exe << " [options]\n"
              << "  -h, --help          display this help message\n"
              << "  -l, --level <file>  start directly in the given .tmx level\n"
              << "  -d, --debug         draw collision boxes\n"
              << "      --autotest      run a headless scripted playthrough and exit\n";
}

} // namespace

int main( int argc, char *argv[] )
{
    nomi::Game::Options opts;

    // parse command line options
    for ( int i = 1; i < argc; ++i )
    {
        const std::string arg = argv[i];
        if ( arg == "-h" || arg == "--help" ) {
            printHelp( argv[0] );
            return EXIT_SUCCESS;
        } else if ( ( arg == "-l" || arg == "--level" ) && i + 1 < argc ) {
            opts.level = argv[++i];
        } else if ( arg == "-d" || arg == "--debug" ) {
            opts.debug = true;
        } else if ( arg == "--autotest" ) {
            opts.autotest = true;
        } else {
            std::cerr << "Unknown option: " << arg << "\n";
            printHelp( argv[0] );
            return EXIT_FAILURE;
        }
    }

    try {
        nomi::Game app( opts );
        return app.run();
    } catch ( const std::exception& e ) {
        std::cerr << "nomi: " << e.what() << "\n";
        return EXIT_FAILURE;
    }
}
