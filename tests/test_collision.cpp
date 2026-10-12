#include <doctest/doctest.h>

#include "collisionmap.hpp"

using namespace nomi;

namespace
{
// 10 x 6 map of 64 px tiles with a floor on row 5
CollisionMap makeFloorMap()
{
    CollisionMap map( 10, 6, 64.f );
    for ( unsigned x = 0; x < 10; ++x ) map.setTile( x, 5, TileCollision::Solid );
    return map;
}

sf::FloatRect box( float x, float y ) { return sf::FloatRect( { x, y }, { 30.f, 60.f } ); }
}

TEST_CASE( "falling onto the floor stops on top of it" )
{
    auto map = makeFloorMap();
    auto b = box( 100.f, 200.f );
    const auto res = map.move( b, { 0.f, 200.f } );   // floor top is at 320, box bottom starts at 260
    CHECK( res.onGround );
    CHECK( b.position.y + b.size.y == doctest::Approx( 320.f ) );
}

TEST_CASE( "a fast fall does not tunnel through the floor" )
{
    auto map = makeFloorMap();
    auto b = box( 100.f, 0.f );
    const auto res = map.move( b, { 0.f, 1000.f } );
    CHECK( res.onGround );
    CHECK( b.position.y + b.size.y == doctest::Approx( 320.f ) );
}

TEST_CASE( "walking into a wall stops at the wall" )
{
    auto map = makeFloorMap();
    map.setTile( 4, 4, TileCollision::Solid );            // wall from x=256
    auto b = box( 200.f, 260.f );                         // standing on the floor
    const auto res = map.move( b, { 100.f, 0.f } );
    CHECK( res.hitRight );
    CHECK( b.position.x + b.size.x == doctest::Approx( 256.f ) );
}

TEST_CASE( "jumping into a ceiling bumps the head" )
{
    auto map = makeFloorMap();
    map.setTile( 1, 2, TileCollision::Solid );            // block with bottom at y=192
    auto b = box( 70.f, 200.f );
    const auto res = map.move( b, { 0.f, -50.f } );
    CHECK( res.hitCeiling );
    CHECK( b.position.y == doctest::Approx( 192.f ) );
}

TEST_CASE( "one-way platforms can be jumped through from below and stood on from above" )
{
    CollisionMap map( 10, 6, 64.f );
    map.setTile( 2, 3, TileCollision::OneWay );           // platform top at y=192

    auto up = box( 140.f, 230.f );
    auto res = map.move( up, { 0.f, -100.f } );
    CHECK_FALSE( res.hitCeiling );
    CHECK( up.position.y == doctest::Approx( 130.f ) );

    auto down = box( 140.f, 100.f );                      // bottom at 160, above the platform
    res = map.move( down, { 0.f, 80.f } );
    CHECK( res.onGround );
    CHECK( down.position.y + down.size.y == doctest::Approx( 192.f ) );
}

TEST_CASE( "the map sides are walls but the bottom is open" )
{
    CollisionMap map( 4, 4, 64.f );
    auto b = box( 10.f, 10.f );
    auto res = map.move( b, { -50.f, 0.f } );
    CHECK( res.hitLeft );
    CHECK( b.position.x == doctest::Approx( 0.f ) );

    res = map.move( b, { 0.f, 500.f } );
    CHECK_FALSE( res.onGround );
    CHECK( b.position.y > map.pixelHeight() );
}

TEST_CASE( "solid rectangles from the object layer block like tiles" )
{
    CollisionMap map( 10, 6, 64.f );
    map.addSolidRect( sf::FloatRect( { 100.f, 300.f }, { 200.f, 20.f } ) );
    auto b = box( 150.f, 200.f );
    const auto res = map.move( b, { 0.f, 100.f } );
    CHECK( res.onGround );
    CHECK( b.position.y + b.size.y == doctest::Approx( 300.f ) );
    CHECK( map.isGroundAt( { 150.f, 305.f } ) );
}
