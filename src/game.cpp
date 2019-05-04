#include "game.hpp"
#include "world.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Vector2.hpp>

namespace nomi
{

game::game()
    : mTimePerFrame( sf::seconds(1.f/60.f) )
    , mWindow(sf::VideoMode(1280, 800), "nomi")
{
    // create the world...
    mWorld = std::make_unique<world>( mWindow );
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
        mWorld->getPlayer().handleEvent(ev);
        
        if ( ev.type == sf::Event::Closed ) mWindow.close();
    }    
}

void game::update(sf::Time dt)
{
    mWorld->update( dt );
}

void game::render(void)
{    
    mWindow.clear(sf::Color::Black);

    mWorld->draw();

    mWindow.display();
}

} // namespace nomi
