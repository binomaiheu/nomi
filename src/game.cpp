#include "game.hpp"
#include "player.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Vector2.hpp>

namespace nomi
{

game::game()
    : mWindow(sf::VideoMode(800, 600), "nomi")
    , mPlayer( 100.f, 150.f )
    , mTimePerFrame( sf::seconds(1.f/60.f) )
{
    mWindow.setFramerateLimit(60);

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
    mWindow.clear(sf::Color::Blue);
    mWindow.draw(mPlayer);
    mWindow.display();
}

} // namespace nomi
