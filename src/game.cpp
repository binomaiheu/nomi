#include "game.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Vector2.hpp>

namespace nomi {

game::game() 
: mWindow( sf::VideoMode(800,600), "nomi" )
, mPlayer( 20.f )
{
    mPlayer.setPosition(100.f, 100.f);
    mPlayer.setFillColor( sf::Color::Red );
}

game::~game()
{
}

void game::run( void ) 
{ 
  while (mWindow.isOpen()) {
        processEvents();
        update();
        render();
  }
}

void game::processEvents( void )
{
    sf::Event ev;
    while( mWindow.pollEvent(ev) ) 
    {
        switch ( ev.type ) 
        {
            case sf::Event::Closed: 
                mWindow.close();
                break;
            case sf::Event::KeyPressed: 
                handlePlayerInput( ev.key.code, true );
                break;
            case sf::Event::KeyReleased: 
                handlePlayerInput( ev.key.code, false );
                break;
        }

    }
}

void game::handlePlayerInput( sf::Keyboard::Key key, bool isPressed )
{
    switch ( key )
    {
        case sf::Keyboard::W: 
            mIsMovingUp = isPressed;
            break;
        case sf::Keyboard::S: 
            mIsMovingDown = isPressed;
            break;
        case sf::Keyboard::A: 
            mIsMovingLeft = isPressed;
            break;
        case sf::Keyboard::D: 
            mIsMovingRight = isPressed;
            break;        

    }
}

void game::update( void )
{
    sf::Vector2f m(0.f,0.f);
    
    if ( mIsMovingUp ) m.y -= 1.f;
    if ( mIsMovingDown ) m.y += 1.f;
    if ( mIsMovingLeft ) m.x -= 1.f;
    if ( mIsMovingRight ) m.x += 1.f;

    mPlayer.move(m);
}

void game::render( void )
{
  mWindow.clear( sf::Color::Green );
  mWindow.draw( mPlayer );
  mWindow.display();

}



} // namespace
