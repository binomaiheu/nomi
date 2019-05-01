#include "game.hpp"
#include "player.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Vector2.hpp>

namespace nomi
{

game::game()
    : mWindow(sf::VideoMode(1200, 600), "nomi")
    , mPlayer( 100.f, 150.f )
    , mTimePerFrame( sf::seconds(1.f/60.f) )
    , mGround( sf::Vector2f( 1200, 100 ) )
{
    mWindow.setFramerateLimit(60);
    mWindow.setKeyRepeatEnabled(false);

    mGround.setPosition(0,500);
    mGround.setFillColor( sf::Color(101,67,33) );
}

game::~game()
{
}

void game::run(void)
{
    sf::Clock clock;
    sf::Time  timeSinceLastUpdate = sf::Time::Zero;
    while (mWindow.isOpen())
    {
        
        processEvents();
        timeSinceLastUpdate += clock.restart();
        while ( timeSinceLastUpdate > mTimePerFrame ) 
        {
            timeSinceLastUpdate -= mTimePerFrame;   
            processEvents();
            update( mTimePerFrame );
        }
        render();
    }
}

void game::processEvents(void)
{
    sf::Event ev;
    while (mWindow.pollEvent(ev))
    {
        // call entity event handlers
        mPlayer.handleEvent(ev);
        
        if ( ev.type == sf::Event::Closed ) mWindow.close();
    }    
}

void game::update(sf::Time dt)
{
    mPlayer.update( dt );
}

void game::render(void)
{
    mWindow.clear(sf::Color(174,234,255) );
    mWindow.draw(mGround);
    mWindow.draw(mPlayer);
    mWindow.display();
}

} // namespace nomi
