#pragma once

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include <tmxlite/Map.hpp>

#include "player.hpp"
#include "tmxlayer.hpp"
#include "tilemap.hpp"

namespace nomi
{

class world : private sf::NonCopyable
{
public:
    world( sf::RenderWindow& window );
    virtual ~world();

    void update( sf::Time dt );
    void draw();

    player& getPlayer() { return mPlayer; } // needed for handleEvent --> better way to do this ??

private:
    sf::RenderWindow& mWindow;
    sf::View          mWorldView;
    sf::FloatRect     mWorldBounds;
    sf::Vector2f      mSpawnPosition;
    
    player            mPlayer;

    sf::Clock         mClock;

    tmx::Map          mMap;
    std::shared_ptr<MapLayer> layerZero;
    std::unique_ptr<tilemap>  mTileMap;
};


} // namspace