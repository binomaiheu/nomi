#include "world.hpp"

namespace nomi
{

// helper function 
sf::Vector3f getManifold(const SceneNode::Pair& node)
{
    const auto normal = node.second->getWorldPosition() - node.first->getWorldPosition();
    sf::FloatRect overlap;

    std::cout << "1. pos x="<<node.first->getWorldPosition().x<<", y="<<node.first->getWorldPosition().y<<"\n";
    std::cout << "2. pos x="<<node.second->getWorldPosition().x<<", y="<<node.second->getWorldPosition().y<<"\n";
    std::cout << "normal x="<<normal.x<<", y="<<normal.y<<"\n";

    node.first->getBoundingRect().intersects(node.second->getBoundingRect(), overlap);

    sf::Vector3f manifold(0,0,0);

    // decide whether to label this a collision from the left of right
    if (overlap.width < overlap.height)
    {
        manifold.x = (normal.x < 0) ? -1.f : 1.f;  // collision from the left of right
        manifold.z = overlap.width;                // penetration depth
    }
    else
    {
        manifold.y = (normal.y < 0) ? -1.f : 1.f;  // collision from the top or bottom
        manifold.z = overlap.height;               // penetration depth
    }

    return manifold;
}

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


    handleCollisions();


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

void World::handleCollisions()
{
    std::set<SceneNode::Pair> collisions;

    // for now... check collisions between player and the object        
    mPlayer.checkSceneCollision( mLevelMap.objectTree(), collisions );

    //resolve collision for each pair
	for (const auto& pair : collisions)
	{        
    	auto man = getManifold(pair);
		pair.second->resolveCollision(man, *(pair.first));
		man.z = -man.z;
		pair.first->resolveCollision(man, *(pair.second));
	}
}


} // namespace