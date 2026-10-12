#include "game.hpp"
#include "states.hpp"
#include "utils.hpp"

#include <SFML/Graphics/Image.hpp>
#include <SFML/Window/Event.hpp>

namespace nomi
{

Game::Game( const Options& opts )
    : mOptions( opts )
    , mTimePerFrame( sf::seconds(1.f/60.f) )
    , mStates( mContext )
    , mIsPaused( false )
{
    gDebugDraw = opts.debug;
    mContext.firstLevel = opts.level.empty() ? dataPath( "levels/level1.tmx" ) : opts.level;
    if ( opts.autotest ) return;   // no window needed

    mWindow.create( sf::VideoMode( { 1280u, 800u } ), "nomi" );
    sf::Image icon;
    if ( icon.loadFromFile( dataPath( "sprites/icon.png" ) ) ) mWindow.setIcon( icon );
    mWindow.setVerticalSyncEnabled( true );
    mWindow.setKeyRepeatEnabled( false );
    mContext.window = &mWindow;
    updateViewport();

    // --level goes straight into the game, otherwise start at the title screen
    if ( opts.level.empty() ) {
        mStates.push<TitleState>();
    } else {
        mContext.session.reset( 0 );
        mStates.push<PlayingState>( mContext.firstLevel );
    }
    mStates.applyPending();
}

Game::~Game()
{
}

int Game::run(void)
{
    if ( mOptions.autotest ) return runAutotest( mContext.firstLevel );

    sf::Clock clock;
    sf::Time  timeSinceLastUpdate = sf::Time::Zero;
    while ( mWindow.isOpen() && !mStates.empty() )
    {
        processEvents();
        timeSinceLastUpdate += clock.restart();
        // fixed time step : the game logic always advances in steps of 1/60 s
        while ( timeSinceLastUpdate > mTimePerFrame )
        {
            timeSinceLastUpdate -= mTimePerFrame;
            processEvents();
            if ( ! mIsPaused ) mStates.update( mTimePerFrame ); // only update if not paused...
        }
        render();
    }
    return 0;
}

void Game::processEvents(void)
{
    while ( const std::optional<sf::Event> ev = mWindow.pollEvent() )
    {
        if ( ev->is<sf::Event::Closed>() ) mWindow.close();
        else if ( ev->is<sf::Event::Resized>() ) updateViewport();
        else if ( ev->is<sf::Event::FocusGained>() ) mIsPaused = false;
        else if ( ev->is<sf::Event::FocusLost>() ) mIsPaused = true;

        mStates.handleEvent( *ev );
    }
}

void Game::updateViewport(void)
{
    // keep the logical 1280x800 screen, add black bars when the window has another aspect ratio
    const sf::Vector2f win( mWindow.getSize() );
    const float scale = std::min( win.x / mContext.viewSize.x, win.y / mContext.viewSize.y );
    const sf::Vector2f vp( mContext.viewSize.x * scale / win.x, mContext.viewSize.y * scale / win.y );

    sf::View view( sf::FloatRect( { 0.f, 0.f }, mContext.viewSize ) );
    view.setViewport( sf::FloatRect( { ( 1.f - vp.x ) / 2.f, ( 1.f - vp.y ) / 2.f }, vp ) );
    mWindow.setView( view );
}

void Game::render(void)
{
    mWindow.clear( sf::Color::Black );
    mStates.draw( mWindow );
    mWindow.display();
}

} // namespace nomi
