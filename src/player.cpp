#include <iostream>
#include <stdexcept>

#include "player.hpp"

namespace nomi
{

player::player(float speed, float jumpHeight)
    : mSpeed(speed)
    , mJumpHeight(jumpHeight)
    , mCanJump(true)
    , mWeight(10.f) // gravity (how heavy is the player)
    , mIsMovingLeft(false)
    , mIsMovingRight(false)
{
    // only load smaller part    
    if (!mTexture.loadFromFile("share/pixmaps/kit_from_firefox.png", sf::IntRect(0, 0, 56, 80) ))
        throw std::runtime_error("Unable to load player texture...");

    mSprite.setTexture(mTexture);
    mSprite.setPosition(20.f, 500.f);
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
    sf::Vector2f velocity(0.f, 0.f);

    if (mIsMovingLeft) velocity.x  -= mSpeed;
    if (mIsMovingRight) velocity.x += mSpeed;

    if ( mSprite.getPosition().y < 500 )
        velocity.y += mWeight;

    mSprite.move( velocity * dt.asSeconds() );
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
        std::cout << "moving left..." << mIsMovingLeft << "\n";
        break;
    case sf::Keyboard::Right:
        mIsMovingRight = isPressed;
        std::cout << "moving right..." << mIsMovingRight << "\n";
        break;
    }
}

} // namespace nomi
