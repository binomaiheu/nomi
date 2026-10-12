#include <cmath>
#include <iostream>

#include "game.hpp"
#include "physics.hpp"
#include "session.hpp"
#include "world.hpp"

// A headless playthrough : a simple bot runs right, jumping over walls, pits and enemies.
// It checks that a level loads, that the simulation runs without crashing and that the
// level can actually be finished.

namespace nomi
{

namespace
{

struct Bot
{
    float jumpHold = 0.f;   // keep the jump key down for a while for a full jump

    Player::Input think( const World& world, float dt )
    {
        Player::Input in;
        in.right = true;

        const Player& p = world.player();
        const auto& map = world.level().collision();
        const sf::Vector2f pos = p.getPosition();
        const float T = map.tileSize();

        // is there ground somewhere below this x position?
        auto groundBelow = [&]( float x ) {
            for ( float y = pos.y + 4.f; y < map.pixelHeight(); y += T / 2.f )
                if ( map.isGroundAt( { x, y } ) ) return true;
            return false;
        };

        if ( !p.onGround() ) {
            in.jump = jumpHold > 0.f;
            jumpHold -= dt;
            // air control : when coming down above a small platform, stop before overshooting it
            if ( p.getVelocity().y > 0.f && groundBelow( pos.x ) && !groundBelow( pos.x + 48.f ) )
                in.right = false;
            return in;
        }
        jumpHold = 0.f;

        // a wall within the next tile (at knee or head height)
        const int col = int( std::floor( ( pos.x + 40.f ) / T ) );
        const bool wall = map.tileAt( col, int( std::floor( ( pos.y - 10.f ) / T ) ) ) == TileCollision::Solid ||
                          map.tileAt( col, int( std::floor( ( pos.y - 60.f ) / T ) ) ) == TileCollision::Solid;
        // no ground a little ahead : a pit, jump at the edge
        const bool pit = !map.isGroundAt( { pos.x + 30.f, pos.y + 8.f } );
        // an enemy coming up
        bool enemy = false;
        for ( const auto& e : world.enemyBounds() ) {
            const float dx = e.position.x - pos.x;
            if ( dx > 0.f && dx < 170.f && std::abs( e.position.y + e.size.y - pos.y ) < 80.f ) enemy = true;
        }

        if ( wall || pit || enemy ) {
            in.jump = true;
            jumpHold = 0.45f;
        }
        return in;
    }
};

}

int runAutotest( const std::string& levelFile )
{
    TextureMgr textures;
    World world( levelFile, textures, characters().front().sheet, { 1280.f, 800.f } );
    Session session;

    std::cout << "[autotest] level " << world.level().name() << " (" << levelFile << ")\n"
              << "[autotest] " << world.coinCount() << " coins, " << world.enemyCount() << " enemies, "
              << world.level().collision().size().x << "x" << world.level().collision().size().y << " tiles\n";

    const sf::Time dt = sf::seconds( 1.f / 60.f );
    const float maxTime = 300.f;
    Bot bot;
    int deaths = 0, stomps = 0;
    float maxX = 0.f;
    bool complete = false;

    float t = 0.f;
    for ( ; t < maxTime && !complete; t += dt.asSeconds() )
    {
        world.update( dt, bot.think( world, dt.asSeconds() ) );
        maxX = std::max( maxX, world.player().getPosition().x );

        for ( const auto& ev : world.pollEvents() )
        {
            switch ( ev.type )
            {
            case WorldEvent::CoinCollected: session.addCoin(); break;
            case WorldEvent::EnemyStomped:  ++stomps; session.addStomp(); break;
            case WorldEvent::PlayerDied:
                ++deaths;
                std::cout << "[autotest] died at x=" << int( ev.position.x ) << " (t=" << int( t ) << "s)\n";
                break;
            case WorldEvent::LevelComplete: complete = true; break;
            default: break;
            }
        }
        if ( deaths > 15 ) break;
    }

    const float progress = maxX / world.level().xmax();
    std::cout << "[autotest] " << ( complete ? "reached the goal" : "did NOT reach the goal" )
              << " after " << int( t ) << "s : progress " << int( progress * 100.f ) << "%, "
              << session.coins << " coins, " << stomps << " enemies stomped, " << deaths << " deaths\n";
    return complete ? 0 : 1;
}

} // namespace nomi
