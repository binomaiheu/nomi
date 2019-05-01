#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Window/Event.hpp>

namespace nomi {

// main game class for the nomi game
class game {
public:
    game();
    virtual ~game();

    void run( void );

private:
    void processEvents( void );
    void update( void );
    void render( void );

    void handlePlayerInput( sf::Keyboard::Key key, bool isPressed );

private:
    sf::RenderWindow mWindow;
    sf::CircleShape  mPlayer;

    bool mIsMovingUp;
    bool mIsMovingDown;
    bool mIsMovingLeft;
    bool mIsMovingRight;
};

}

