#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

#include "game.hpp"
#include "utils.hpp"
#include "version.hpp"

namespace
{

// The release build is a windowed application without a console. When it is started
// from a terminal (e.g. "nomi --help"), attach to that terminal so the output shows up.
// When the output is redirected to a file or pipe it already works, leave it alone.
void attachParentConsole()
{
#ifdef _WIN32
    const HANDLE out = GetStdHandle( STD_OUTPUT_HANDLE );
    if ( ( out == nullptr || out == INVALID_HANDLE_VALUE ) && AttachConsole( ATTACH_PARENT_PROCESS ) ) {
        FILE* f = nullptr;
        freopen_s( &f, "CONOUT$", "w", stdout );
        freopen_s( &f, "CONOUT$", "w", stderr );
        std::cout << "\n";
    }
#endif
}

void printHelp( const char* exe )
{
    std::cout << "nomi " NOMI_VERSION "\n"
              << "Usage: " << exe << " [options]\n"
              << "  -h, --help          display this help message\n"
              << "  -v, --version       display the version\n"
              << "  -l, --level <file>  start directly in the given .tmx level\n"
              << "  -d, --debug         draw collision boxes\n"
              << "      --autotest      run a headless scripted playthrough and exit\n";
}

} // namespace

int main( int argc, char *argv[] )
{
    attachParentConsole();
    nomi::initDataPath( argv[0] );
    nomi::Game::Options opts;

    // parse command line options
    for ( int i = 1; i < argc; ++i )
    {
        const std::string arg = argv[i];
        if ( arg == "-h" || arg == "--help" ) {
            printHelp( argv[0] );
            return EXIT_SUCCESS;
        } else if ( arg == "-v" || arg == "--version" ) {
            std::cout << "nomi " NOMI_VERSION "\n";
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
