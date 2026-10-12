#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

#include "utils.hpp"

int main( int argc, char** argv )
{
    // the level tests read share/ : find it from the source tree
    nomi::initDataPath( NOMI_SOURCE_DIR "/nomi_tests" );

    doctest::Context context;
    context.applyCommandLine( argc, argv );
    return context.run();
}
