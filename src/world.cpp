#include "world.hpp"

namespace nomi
{

World::World( sf::RenderWindow& window )
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
    mLevelMap.load_tmx( "share/tilemaps/tilemap2.tmx" );
    
    // TODO : where do we want the textures ???

    // TODO
    // set the mWorldBounds to the map size...
    
    // set view to the spawn position...

    // start the world clock
    mClock.restart();
}

World::~World()
{}

void World::loadTextures()
{

// TODO --> there should be q resource manager in the polaye rlcass for the separate reoucers associated with the player...

    mTextures.load( "Kit",    "share/pixmaps/kit_from_firefox.png" );
    mTextures.load( "Gnu",    "share/pixmaps/gnu_from_firefox.png" );
    mTextures.load( "Wilber", "share/pixmaps/wilber_from_firefox.png" );
    mTextures.load( "Tux",    "share/pixmaps/tux_from_firefox.png" );

    return;
}


// updates the worldm viewport etc...
void World::update( sf::Time dt )
{
    // update the world
    //layerZero->update( mClock.getElapsedTime() );

    // update the player..
    mPlayer.update( dt );
    
    //std::cout << "player position  : " << mPlayer.getPosition().x << "x" << mPlayer.getPosition().y << "\n";
    //std::cout << "worldview center : " << mWorldView.getCenter().x << "x" << mWorldView.getCenter().y << "\n";
    //std::cout << "          size   : " << mWorldView.getSize().x << "x" << mWorldView.getSize().y << "\n";
     
    float xminWorld = mWorldView.getCenter().x - mWorldView.getSize().x / 2.;
    float xmaxWorld = mWorldView.getCenter().x + mWorldView.getSize().x / 2.;

    // update the viewport... depending on the player position...    
    float edge_fraction = 0.30; // start scrolling at a fraction fro the edge

    if ( ( ( xminWorld > mLevelMap.xmin() )  && 
        ( mPlayer.getPosition().x < xminWorld + edge_fraction * mWorldView.getSize().x ) ) )
        mWorldView.move( mPlayer.getPosition().x - ( xminWorld + edge_fraction * mWorldView.getSize().x ) , 0. );

    if ( ( ( xmaxWorld <  mLevelMap.xmax() )  && 
        ( mPlayer.getPosition().x > xmaxWorld - edge_fraction * mWorldView.getSize().x ) ) )
        mWorldView.move( mPlayer.getPosition().x - ( xmaxWorld - edge_fraction * mWorldView.getSize().x ) , 0. );
        



    //mWorldView.move( 5.*dt.asSeconds(), 0. ); // just scroll...
    
}

// draw the world...
void World::draw()
{
    // set the view
    mWindow.setView( mWorldView );

    // draw the level map
    mWindow.draw(mLevelMap);  // tilemaplayer
    
    mWindow.draw(mPlayer);     // the player
}

}