#include <iostream>
#include <stdexcept>
#include <string>
#include <cmath>

#include "player.hpp"

namespace nomi
{

player::player(float speed, float jumpHeight)
    : mSpeed(speed)
    , mJumpHeight(jumpHeight)
    , mCanJump(true)
    , mWeight(5.*981.f) // gravity (how heavy is the player)
    , mVelocity(0.f, 0.f)
    , mIsMovingLeft(false)
    , mIsMovingRight(false)
    , mJump(false)
{
    // only load smaller part    
    std::string filename = "share/pixmaps/kit_from_firefox.png";
    //std::string filename = "share/pixmaps/gnu_from_gnu.png";
    //std::string filename = "share/pixmaps/wilber_from_gimp.png";
    //std::string filename = "share/pixmaps/tux_from_linux.png";
    //std::string filename = "share/pixmaps/kisi_from_konsolscript.png";
    if (!mTexture.loadFromFile(filename, sf::IntRect(0, 0, 56, 80) ))
        throw std::runtime_error("Unable to load player texture...");

    mSprite.setTexture(mTexture);
    mSprite.setPosition(50.f, 400.f);
    mSprite.setOrigin(28.,80.); // set origin to the bottom center 
}

player::~player()
{
}

void player::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    target.draw(mSprite, states);
}

void player::update(sf::Time dt)
{    
    // every frame we reset the x velocity to 0 as we only need to remember the y velocity to be able to jump between frames
    // however instead of setting it 0, we multiply the current value with a number < 1, this results in an effect where the 
    // sprite slows down when stopping or gradually accelerates when starting to move, a bit more realistic effect. If this 
    // constant is closer to 0 the smaller this effect, if it is closer to 1, the larger the effect...
    mVelocity.x *= 0.85f;

    if (mIsMovingLeft)  {
        mVelocity.x -= mSpeed;        
    }
    if (mIsMovingRight) {
        mVelocity.x += mSpeed;        
    }

    if ( mSprite.getPosition().y < 650 )
        mVelocity.y += mWeight * dt.asSeconds();
    
    if ( mSprite.getPosition().y >=  650 ) { 
        mVelocity.y = 0.; 
        mCanJump = true; 
    }

    if ( mJump && mCanJump ) {
        mCanJump = false;
        mJump    = false;
        mVelocity.y = -sqrt( 2.0 * mWeight * mJumpHeight ); 
    }   

    // move the sprite
    mSprite.move( mVelocity * dt.asSeconds() );
    // set direction of the sprite, use setScale, not scale as we want this w.r.t. the original
    mVelocity.x > 0 ? mSprite.setScale(1.,1.) : mSprite.setScale(-1.,1.);
}

void player::handleEvent(const sf::Event &ev)
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

void player::handleInput(sf::Keyboard::Key key, bool isPressed)
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
void player::onCollision( sf::Vector2f direction ) 
{
    if ( direction.x > 0 ) {
        // collision to the right
        mVelocity.x = 0.f;
    } else if ( direction.x < 0 ) {
        // collision to the left
        mVelocity.x = 0.f;
    } else if ( direction.y < 0 ) {
        // collision to the bottom
        mVelocity.y = 0.;
        mCanJump = true; // can jump againm we have collided with something on the bottom
    } else {
        // collision to the top
        mVelocity.y = 0.;
    }

}

} // namespace nomi
