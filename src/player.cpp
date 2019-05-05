#include <iostream>
#include <stdexcept>
#include <string>
#include <cmath>

#include "player.hpp"

namespace nomi
{

Player::Player(float speed, float jumpHeight)
    : Entity()
    , mSpeed(speed)
    , mJumpHeight(jumpHeight)
    , mCanJump(true)
    , mWeight(5.*981.f) // gravity (how heavy is the player)    
    , mIsMovingLeft(false)
    , mIsMovingRight(false)
    , mJump(false)
{
    // only load smaller part    
    std::string filename = "share/pixmaps/kit_from_firefox.png";
    //std::string filename = "share/pixmaps/gnu_from_gnu.png";
    //std::string filename = "share/pixmaps/wilber_from_gimp.png";
    //std::string filename = "share/pixmaps/tux_from_linux.png";
    if (!mTexture.loadFromFile(filename))
        throw std::runtime_error("Unable to load player texture...");

    // choose the figure via the rectangle...   
    mSprite.setTexture(mTexture);
    mSprite.setTextureRect( sf::IntRect(0, 0, 56, 80) );    
    mSprite.setOrigin(28.,80.); // set origin to the bottom center 

    // set spawn positino
    this->setVelocity( 0.f, 0.f );
    this->setPosition(50.f, 400.f);
}

Player::~Player()
{
}

void Player::drawCurrent(sf::RenderTarget &target, sf::RenderStates states) const
{

    // transforms are handled in the draw() method of SceneNode, so just draw the sprites or other..
    target.draw(mSprite, states);
}

void Player::updateCurrent(sf::Time dt)
{    
    sf::Vector2f v = this->getVelocity();

    // every frame we reset the x velocity to 0 as we only need to remember the y velocity to be able to jump between frames
    // however instead of setting it 0, we multiply the current value with a number < 1, this results in an effect where the 
    // sprite slows down when stopping or gradually accelerates when starting to move, a bit more realistic effect. If this 
    // constant is closer to 0 the smaller this effect, if it is closer to 1, the larger the effect...
    v.x *= 0.85f;

    if (mIsMovingLeft)  v.x -= mSpeed;            
    if (mIsMovingRight) v.x += mSpeed;        

    if ( getPosition().y < 650 ) v.y += mWeight * dt.asSeconds();
    
    if ( getPosition().y >=  650 ) {
        v.y = 0.; 
        mCanJump = true; 
    }

    if ( mJump && mCanJump ) {
        mCanJump = false;
        mJump    = false;
        v.y = -sqrt( 2.0 * mWeight * mJumpHeight ); 
    }   

    // set direction of the sprite, use setScale, not scale as we want this w.r.t. the original
    v.x > 0 ? setScale(1.,1.) : setScale(-1.,1.);

    // update with new velocity
    setVelocity(v);

    // call the base class implementation of the update ( just to move the sprite )
    Entity::updateCurrent( dt );
}

void Player::handleEvent(const sf::Event &ev)
{

    switch (ev.type)
    {
    case sf::Event::KeyPressed:
        handleInput(ev.key.code, true);
        break;
    case sf::Event::KeyReleased:
        handleInput(ev.key.code, false);
        break;
    }
}

void Player::handleInput(sf::Keyboard::Key key, bool isPressed)
{
    switch (key)
    {
    case sf::Keyboard::Left:
        mIsMovingLeft = isPressed;        
        break;
    case sf::Keyboard::Right:
        mIsMovingRight = isPressed;        
        break;
    case sf::Keyboard::Space:
        if ( isPressed ) mJump = true;
        break;
    }
}

// needed when properly adding collision detection, see : 
// https://www.youtube.com/watch?v=l2iCYCLi6MU&list=PL21OsoBLPpMOO6zyVlxZ4S4hwkY_SLRW9&index=13
void Player::onCollision( sf::Vector2f direction ) 
{   
    sf::Vector2f v = this->getVelocity();

    if ( direction.x > 0 ) {
        // collision to the right
        v.x = 0.f;
    } else if ( direction.x < 0 ) {
        // collision to the left
        v.x = 0.f;
    } else if ( direction.y < 0 ) {
        // collision to the bottom
        v.y = 0.;
        mCanJump = true; // can jump againm we have collided with something on the bottom
    } else {
        // collision to the top
        v.y = 0.;
    }
    setVelocity( v );
}

} // namespace nomi
