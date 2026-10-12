#include <doctest/doctest.h>

#include <cmath>
#include <filesystem>
#include <set>
#include <string>

#include "levelmap.hpp"
#include "utils.hpp"

using namespace nomi;
namespace fs = std::filesystem;

// Checks every level in share/levels : it loads, has a spawn and a goal, the spawn is not
// buried in the ground, enemies use existing sprites and the "next" chain is valid.

namespace
{
std::vector<fs::path> levelFiles()
{
    std::vector<fs::path> files;
    for ( const auto& e : fs::directory_iterator( dataPath( "levels" ) ) )
        if ( e.path().extension() == ".tmx" ) files.push_back( e.path() );
    return files;
}
}

TEST_CASE( "all levels are valid" )
{
    const auto files = levelFiles();
    REQUIRE( files.size() >= 2 );

    for ( const auto& file : files )
    {
        CAPTURE( file.filename().string() );
        LevelMap map;
        REQUIRE_NOTHROW( map.load( file.generic_string() ) );

        int spawns = 0, goals = 0;
        for ( const auto& obj : map.objects() )
        {
            CAPTURE( obj.cls );
            const std::set<std::string> known = { "spawn", "coin", "enemy", "goal" };
            CHECK( known.count( obj.cls ) == 1 );

            if ( obj.cls == "spawn" ) {
                ++spawns;
                // standing on something, and not inside a solid tile
                const auto& cm = map.collision();
                const sf::Vector2f feet = obj.bottomCenter();
                CHECK( cm.isGroundAt( { feet.x, feet.y + 4.f } ) );
                const int tx = int( std::floor( feet.x / cm.tileSize() ) );
                const int ty = int( std::floor( ( feet.y - 4.f ) / cm.tileSize() ) );
                CHECK( cm.tileAt( tx, ty ) != TileCollision::Solid );
            }
            if ( obj.cls == "goal" ) ++goals;
            if ( obj.cls == "enemy" ) {
                const std::string sprite = obj.str( "sprite", "gnu" );
                bool found = false;
                for ( const auto& e : fs::directory_iterator( dataPath( "pixmaps" ) ) )
                    found |= e.path().filename().string().rfind( sprite, 0 ) == 0;
                CHECK_MESSAGE( found, "no sprite sheet for enemy " << sprite );
            }
        }
        CHECK( spawns == 1 );
        CHECK( goals >= 1 );

        const std::string next = map.nextLevel();
        if ( !next.empty() ) CHECK_MESSAGE( fs::exists( next ), "next level " << next << " does not exist" );
    }
}

TEST_CASE( "the first level leads to all the others" )
{
    std::set<std::string> seen;
    std::string file = dataPath( "levels/level1.tmx" );
    while ( !file.empty() && seen.insert( fs::weakly_canonical( file ).string() ).second )
    {
        LevelMap map;
        REQUIRE_NOTHROW( map.load( file ) );
        file = map.nextLevel();
    }
    CHECK( file.empty() );   // the chain ends instead of looping
    CHECK( seen.size() >= 2 );
}
