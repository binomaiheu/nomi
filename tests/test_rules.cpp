#include <doctest/doctest.h>

#include "physics.hpp"
#include "session.hpp"

using namespace nomi;

TEST_CASE( "landing on an enemy from above is a stomp" )
{
    // falling, and the feet were above the enemy's head in the previous tick
    CHECK( phys::isStomp( 400.f, 300.f, 405.f ) );
    // a little overlap from the previous tick is tolerated
    CHECK( phys::isStomp( 410.f, 300.f, 405.f ) );
}

TEST_CASE( "running into an enemy or jumping up into it hurts" )
{
    CHECK_FALSE( phys::isStomp( 460.f, 0.f, 405.f ) );     // side contact on the ground
    CHECK_FALSE( phys::isStomp( 400.f, -500.f, 405.f ) );  // moving up
    CHECK_FALSE( phys::isStomp( 450.f, 300.f, 405.f ) );   // falling, but was already level with it
}

TEST_CASE( "the jump reaches the configured height" )
{
    const float v = phys::jumpVelocity();
    // h = v^2 / 2g
    CHECK( v * v / ( 2.f * phys::Gravity ) == doctest::Approx( phys::JumpHeight ) );
    CHECK( phys::JumpHeight > 3.f * phys::TileSize );       // three tile steps must be climbable
}

TEST_CASE( "coins add score and give an extra life every 100" )
{
    Session s;
    s.reset( 2 );
    CHECK( s.character == 2 );
    CHECK( s.lives == Session::StartLives );

    for ( int i = 0; i < 99; ++i ) CHECK_FALSE( s.addCoin() );
    CHECK( s.coins == 99 );
    CHECK( s.addCoin() );
    CHECK( s.coins == 0 );
    CHECK( s.lives == Session::StartLives + 1 );
    CHECK( s.score == 100 * Session::CoinScore );
}

TEST_CASE( "losing the last life ends the game" )
{
    Session s;
    s.reset( 0 );
    CHECK( s.loseLife() );
    CHECK( s.loseLife() );
    CHECK_FALSE( s.loseLife() );
    CHECK( s.lives == 0 );
    CHECK_FALSE( s.loseLife() );       // never below zero
    CHECK( s.lives == 0 );
}
