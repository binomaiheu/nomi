#pragma once

#include <string>
#include <vector>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/View.hpp>

#include "levelmap.hpp"
#include "player.hpp"
#include "resourcemgr.hpp"
#include "scenenode.hpp"

namespace nomi
{

// things that happened during an update, handled by the game state (sounds, score, lives)
struct WorldEvent
{
    enum Type { Jumped, CoinCollected, EnemyStomped, PlayerKilled, PlayerDied, LevelComplete };
    Type         type;
    sf::Vector2f position;
};

class World
{
public:
    // characterSheet : file below share/pixmaps used for the player
    World( const std::string& levelFile, TextureMgr& textures, const std::string& characterSheet,
           sf::Vector2f viewSize );
    virtual ~World();
    World( const World& ) = delete;
    World& operator=( const World& ) = delete;

    void update( sf::Time dt, const Player::Input& input );
    void draw( sf::RenderTarget& target ) const;

    std::vector<WorldEvent> pollEvents();
    void respawnPlayer();

    const LevelMap& level() const   { return mLevelMap; }
    const Player&   player() const  { return *mPlayer; }
    sf::Vector2f    spawnPosition() const { return mSpawnPosition; }
    std::size_t     enemyCount() const;
    std::vector<sf::FloatRect> enemyBounds() const;
    std::size_t     coinCount() const;

private:
    void buildScene( TextureMgr& textures, const std::string& characterSheet );
    void handleCollisions();
    void updateCamera( sf::Time dt, bool snap );

    LevelMap                 mLevelMap;
    sf::View                 mWorldView;
    sf::Vector2f             mSpawnPosition;

    SceneNode                mSceneGraph;
    Player*                  mPlayer = nullptr;

    std::vector<WorldEvent>  mEvents;
    bool                     mLevelDone = false;
};


} // namspace
