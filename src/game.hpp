#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Window/Event.hpp>

#include "player.hpp"

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
  
};

}
