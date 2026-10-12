#include <algorithm>
#include <cmath>
#include <iostream>

#include "enemy.hpp"
#include "physics.hpp"
#include "pickup.hpp"
#include "utils.hpp"
#include "world.hpp"

namespace nomi
{

namespace
{
const std::vector<std::string>& enemySheets()
{
    static const std::vector<std::string> sheets = {
        "gnu_from_gnu.png", "tux_from_linux.png", "wilber_from_gimp.png", "kisi_from_konsolscript.png" };
    return sheets;
}

// enemy "sprite" property (gnu, tux, ...) to a sheet file name
std::string enemySheet( const std::string& sprite )
{
    for ( const auto& s : enemySheets() )
        if ( !sprite.empty() && s.rfind( sprite, 0 ) == 0 ) return s;
    return enemySheets().front();
}
}

World::World( const std::string& levelFile, TextureMgr& textures, const std::string& characterSheet,
              sf::Vector2f viewSize )
    : mWorldView( sf::FloatRect( { 0.f, 0.f }, viewSize ) )
{
    mLevelMap.load( levelFile );
    buildScene( textures, characterSheet );
    updateCamera( sf::Time::Zero, true );
}

World::~World()
{}

void World::buildScene( TextureMgr& textures, const std::string& characterSheet )
{
    const auto& map = mLevelMap.collision();
    auto& coinTex = textures.load( "coin", dataPath( "sprites/coin.png" ) );
    auto& flagTex = textures.load( "flag", dataPath( "sprites/flag.png" ) );

    bool haveSpawn = false;
    std::vector<std::unique_ptr<SceneNode>> enemies;

    for ( const auto& obj : mLevelMap.objects() )
    {
        if ( obj.cls == "spawn" ) {
            mSpawnPosition = obj.bottomCenter();
            haveSpawn = true;
        } else if ( obj.cls == "coin" ) {
            auto coin = std::make_unique<Coin>( coinTex );
            coin->setPosition( obj.center() );
            mSceneGraph.attachChild( std::move( coin ) );
        } else if ( obj.cls == "goal" ) {
            auto goal = std::make_unique<Goal>( flagTex );
            goal->setPosition( { obj.rect.position.x + 12.f, obj.rect.position.y + obj.rect.size.y } );
            mSceneGraph.attachChild( std::move( goal ) );
        } else if ( obj.cls == "enemy" ) {
            std::string sheet = enemySheet( obj.str( "sprite" ) );
            // never let the enemies look like the hero : they become Kit instead
            if ( sheet == characterSheet ) sheet = "kit_from_firefox.png";
            auto& tex = textures.load( "char/" + sheet, dataPath( "pixmaps/" + sheet ) );
            auto enemy = std::make_unique<Enemy>( tex, map, obj.num( "speed", phys::EnemySpeed ) );
            enemy->setPosition( obj.bottomCenter() );
            enemies.push_back( std::move( enemy ) );
        } else {
            std::cerr << "[World] unknown object class '" << obj.cls << "' in " << mLevelMap.fileName() << "\n";
        }
    }
    if ( !haveSpawn ) mSpawnPosition = { 2.5f * map.tileSize(), map.pixelHeight() / 2.f };

    // enemies and the player are drawn on top of coins and the flag
    for ( auto& e : enemies ) mSceneGraph.attachChild( std::move( e ) );

    auto& playerTex = textures.load( "char/" + characterSheet, dataPath( "pixmaps/" + characterSheet ) );
    auto player = std::make_unique<Player>( playerTex, map );
    mPlayer = player.get();
    mPlayer->respawn( mSpawnPosition );
    mSceneGraph.attachChild( std::move( player ) );
}

std::vector<WorldEvent> World::pollEvents()
{
    std::vector<WorldEvent> ev;
    std::swap( ev, mEvents );
    return ev;
}

void World::respawnPlayer()
{
    mPlayer->respawn( mSpawnPosition );
    updateCamera( sf::Time::Zero, true );
}

std::size_t World::enemyCount() const
{
    std::size_t n = 0;
    for ( const auto& obj : mLevelMap.objects() ) n += obj.cls == "enemy";
    return n;
}

std::vector<sf::FloatRect> World::enemyBounds() const
{
    std::vector<sf::FloatRect> out;
    mSceneGraph.collectBoundingRects( SceneNode::Enemy, out );
    return out;
}

std::size_t World::coinCount() const
{
    std::size_t n = 0;
    for ( const auto& obj : mLevelMap.objects() ) n += obj.cls == "coin";
    return n;
}

// updates the world, viewport etc...
void World::update( sf::Time dt, const Player::Input& input )
{
    mPlayer->setInput( mLevelDone ? Player::Input() : input );
    mSceneGraph.update( dt );

    if ( mPlayer->hasJumped() ) mEvents.push_back( { WorldEvent::Jumped, mPlayer->getPosition() } );

    if ( mPlayer->isAlive() )
    {
        handleCollisions();

        // fell into a pit
        if ( mPlayer->getPosition().y - 66.f > mLevelMap.ymax() ) {
            mPlayer->kill();
            mEvents.push_back( { WorldEvent::PlayerKilled, mPlayer->getPosition() } );
        }
    }
    else if ( mPlayer->deathFinished() )
    {
        // back to the start; the game state decides whether there are lives left
        mEvents.push_back( { WorldEvent::PlayerDied, mPlayer->getPosition() } );
        respawnPlayer();
    }

    mSceneGraph.removeWrecks();
    updateCamera( dt, false );
}

void World::handleCollisions()
{
    std::set<SceneNode::Pair> collisions;
    mSceneGraph.checkSceneCollision( mSceneGraph, collisions );

    for ( const auto& pair : collisions )
    {
        SceneNode* a = pair.first;
        SceneNode* b = pair.second;

        // enemies bumping into each other both turn around
        if ( a->getType() == SceneNode::Enemy && b->getType() == SceneNode::Enemy ) {
            auto* ea = static_cast<Enemy*>( a );
            auto* eb = static_cast<Enemy*>( b );
            if ( ea->isAlive() && eb->isAlive() ) {
                const bool aLeft = ea->getPosition().x < eb->getPosition().x;
                if ( ( ea->getVelocity().x > 0.f ) == aLeft ) ea->turnAround();
                if ( ( eb->getVelocity().x < 0.f ) == aLeft ) eb->turnAround();
            }
            continue;
        }

        // the rest only matters when the player is involved
        if ( b->getType() == SceneNode::Player ) std::swap( a, b );
        if ( a->getType() != SceneNode::Player || !mPlayer->isAlive() ) continue;

        switch ( b->getType() )
        {
        case SceneNode::Coin: {
            auto* coin = static_cast<Coin*>( b );
            if ( !coin->isCollected() ) {
                coin->collect();
                mEvents.push_back( { WorldEvent::CoinCollected, coin->getPosition() } );
            }
            break;
        }
        case SceneNode::Enemy: {
            auto* enemy = static_cast<Enemy*>( b );
            if ( !enemy->isAlive() ) break;
            if ( phys::isStomp( mPlayer->previousBottom(), mPlayer->getVelocity().y, enemy->getBoundingRect().position.y ) ) {
                enemy->stomp();
                mPlayer->bounce();
                mEvents.push_back( { WorldEvent::EnemyStomped, enemy->getPosition() } );
            } else if ( !mPlayer->isInvulnerable() ) {
                mPlayer->kill();
                mEvents.push_back( { WorldEvent::PlayerKilled, mPlayer->getPosition() } );
            }
            break;
        }
        case SceneNode::Goal: {
            auto* goal = static_cast<Goal*>( b );
            if ( !goal->isReached() ) {
                goal->reach();
                mPlayer->celebrate();
                mLevelDone = true;
                mEvents.push_back( { WorldEvent::LevelComplete, goal->getPosition() } );
            }
            break;
        }
        default:
            break;
        }
    }
}

void World::updateCamera( sf::Time dt, bool snap )
{
    const sf::Vector2f size = mWorldView.getSize();
    const sf::Vector2f p    = mPlayer->getPosition();

    // look a bit ahead in the walking direction, keep the player in the lower half
    sf::Vector2f target( p.x + mPlayer->getVelocity().x * 0.25f, p.y - size.y * 0.1f );
    if ( mPlayer->isDying() ) target = mWorldView.getCenter();

    const float halfW = size.x / 2.f, halfH = size.y / 2.f;
    target.x = std::clamp( target.x, mLevelMap.xmin() + halfW, std::max( mLevelMap.xmin() + halfW, mLevelMap.xmax() - halfW ) );
    target.y = std::clamp( target.y, mLevelMap.ymin() + halfH, std::max( mLevelMap.ymin() + halfH, mLevelMap.ymax() - halfH ) );

    if ( snap ) {
        mWorldView.setCenter( target );
        return;
    }
    // smooth follow, frame rate independent
    const float s = dt.asSeconds();
    const sf::Vector2f c = mWorldView.getCenter();
    const float kx = 1.f - std::exp( -8.f * s );
    const float ky = 1.f - std::exp( -4.f * s );
    mWorldView.setCenter( { c.x + ( target.x - c.x ) * kx, c.y + ( target.y - c.y ) * ky } );
}

// draw the world...
void World::draw( sf::RenderTarget& target ) const
{
    const sf::View previous = target.getView();

    // snap the camera to whole pixels so the tiles don't shimmer
    sf::View view = mWorldView;
    view.setCenter( { std::round( view.getCenter().x ), std::round( view.getCenter().y ) } );
    view.setViewport( previous.getViewport() );
    target.setView( view );

    mLevelMap.drawBackground( target );
    target.draw( mSceneGraph );
    mLevelMap.drawForeground( target );
    if ( gDebugDraw ) mLevelMap.collision().drawDebug( target );

    target.setView( previous );
}

} // namespace
