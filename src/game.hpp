#pragma once

#include <memory>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Event.hpp>

#include "world.hpp"

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
    sf::Time                 mTimePerFrame;
    sf::RenderWindow         mWindow;
    std::unique_ptr<world>   mWorld;
};

}
