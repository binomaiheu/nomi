#pragma once

#include <memory>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Event.hpp>

#include "tmxlayer.hpp"
#include "player.hpp"
#include "tilemap.hpp"

namespace nomi {

// main game class for the nomi game
class game {
public:
    game();
    virtual ~game();

    void run( void );

private:
    void processEvents( void );
    void update( sf::Time dt );
    void render( void );   

private:
    sf::RenderWindow mWindow;    
    nomi::player     mPlayer;
    sf::Time         mTimePerFrame;
    sf::RectangleShape mGround;
  
    tmx::Map mMap;
    std::shared_ptr<MapLayer> layerZero;
    std::unique_ptr<tilemap>  mTileMap;

};

}
