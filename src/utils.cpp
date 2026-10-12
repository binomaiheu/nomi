#include <cmath>
#include <filesystem>
#include <system_error>

#include "utils.hpp"

namespace nomi{

namespace fs = std::filesystem;

bool gDebugDraw = false;

namespace
{
fs::path gShareDir;

fs::path findShareDir( const fs::path& exeDir )
{
    // look in the working directory first, then next to the executable and up from
    // there (build/Release/nomi.exe -> repo root)
    std::error_code ec;
    const fs::path candidates[] = {
        fs::current_path( ec ) / "share",
        exeDir / "share",
        exeDir / ".." / "share",
        exeDir / ".." / ".." / "share",
    };
    for ( const auto& c : candidates )
        if ( fs::is_directory( c / "pixmaps", ec ) ) return fs::weakly_canonical( c, ec );
    return fs::path( "share" );
}
}

float length(sf::Vector2f v)
{
    return std::sqrt( v.x*v.x + v.y*v.y );
}

void initDataPath( const char* argv0 )
{
    std::error_code ec;
    fs::path exe = argv0 ? fs::absolute( fs::path( argv0 ), ec ) : fs::path();
    gShareDir = findShareDir( exe.parent_path() );
}

std::string dataPath( const std::string& relative )
{
    if ( gShareDir.empty() ) initDataPath( nullptr );
    return ( gShareDir / fs::path( relative ) ).generic_string();
}

}
