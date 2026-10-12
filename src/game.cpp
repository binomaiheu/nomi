#include "game.hpp"
#include "world.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Vector2.hpp>

namespace nomi
{

Game::Game( const Options& opts )
    : mOptions( opts )
    , mTimePerFrame( sf::seconds(1.f/60.f) )
    , mWindow( sf::VideoMode( { 1280u, 800u } ), "nomi" )
    , mIsPaused( false )
{
    // create the world...
    mWorld = std::make_unique<World>( mWindow );
}

Game::~Game()
{
}

int Game::run(void)
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
            if ( ! mIsPaused ) update( mTimePerFrame ); // only update if not paused...
        }
        render();
    }
    return 0;
}

void Game::processEvents(void)
{
    while ( const std::optional<sf::Event> ev = mWindow.pollEvent() )
    {
        // call entity event handlers
        mWorld->getPlayer().handleEvent( *ev );

        if ( ev->is<sf::Event::Closed>() ) mWindow.close();

        if ( ev->is<sf::Event::FocusGained>() ) mIsPaused = false;
        else if ( ev->is<sf::Event::FocusLost>() ) mIsPaused = true;
    }
}

void Game::update(sf::Time dt)
{
    mWorld->update( dt );
}

void Game::render(void)
{
    mWindow.clear(sf::Color::Black);

    mWorld->draw();

    mWindow.display();
}

} // namespace nomi
