#include "world.hpp"

namespace nomi
{

world::world( sf::RenderWindow& window )
: mWindow( window )
, mWorldView( window.getDefaultView() )
, mWorldBounds( 0., 0., mWorldView.getSize().x, mWorldView.getSize().y ) // default, overwrite when map is loaded
, mSpawnPosition( mWorldView.getSize().x / 2., mWorldBounds.height - 0.2 * mWorldView.getSize().y  )
, mPlayer( 80.f, 220.f )
{
    // some window settings
    mWindow.setFramerateLimit(60);
    mWindow.setKeyRepeatEnabled(false);

    // load textures & map
    mMap.load("share/tilemaps/tilemap2.tmx");
    layerZero = std::make_shared<MapLayer>(mMap, 0);
        

    
    // TODO
    // set the mWorldBounds to the map size...
    
    // set view to the spawn position...

    // start the world clock
    mClock.restart();
}

world::~world()
{}

// updates the worldm viewport etc...
void world::update( sf::Time dt )
{
    // update the world
    //layerZero->update( mClock.getElapsedTime() );

    // update the player..
    mPlayer.update( dt );
    std::cout << "player position  : " << mPlayer.getPosition().x << "x" << mPlayer.getPosition().y << "\n";
    std::cout << "worldview center : " << mWorldView.getCenter().x << "x" << mWorldView.getCenter().y << "\n";
    std::cout << "          size   : " << mWorldView.getSize().x << "x" << mWorldView.getSize().y << "\n";
     
    float xminWorld = mWorldView.getCenter().x - mWorldView.getSize().x / 2.;
    float xmaxWorld = mWorldView.getCenter().x + mWorldView.getSize().x / 2.;

    // update the viewport... depending on the player position...    
    float edge_fraction = 0.30; // start scrolling at a fraction fro the edge

    if ( ( ( xminWorld > 0 )  && 
        ( mPlayer.getPosition().x < xminWorld + edge_fraction * mWorldView.getSize().x ) ) )
        mWorldView.move( mPlayer.getPosition().x - ( xminWorld + edge_fraction * mWorldView.getSize().x ) , 0. );

    if ( ( ( xmaxWorld < ( mMap.getBounds().left + mMap.getBounds().width ) )  && 
        ( mPlayer.getPosition().x > xmaxWorld - edge_fraction * mWorldView.getSize().x ) ) )
        mWorldView.move( mPlayer.getPosition().x - ( xmaxWorld - edge_fraction * mWorldView.getSize().x ) , 0. );
        



    //mWorldView.move( 5.*dt.asSeconds(), 0. ); // just scroll...
    
}

// draw the world...
void world::draw()
{
    // set the view
    mWindow.setView( mWorldView );

    // draw the world...
    mWindow.draw(*layerZero);  // maplayer
    mWindow.draw(mPlayer);     // the player
}

}