#pragma once

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "scenenode.hpp"
#include "player.hpp"
#include "levelmap.hpp"
#include "resourcemgr.hpp"

namespace nomi
{

class World : private sf::NonCopyable
{
public:
    World( sf::RenderWindow& window );
    virtual ~World();

    void update( sf::Time dt );
    void draw();

    Player& getPlayer() { return mPlayer; } // needed for handleEvent --> better way to do this ??

private:
    sf::RenderWindow& mWindow;
    sf::View          mWorldView;
    sf::FloatRect     mWorldBounds;
    sf::Vector2f      mSpawnPosition;
    
    Player            mPlayer;

    sf::Clock         mClock;

    LevelMap          mLevelMap;
    
    ResourceMgr<sf::Texture, std::string> mTextures;

private: 
    void loadTextures();
    void handleCollisions();
};


} // namspace