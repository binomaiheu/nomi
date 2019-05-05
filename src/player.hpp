#pragma once

#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Event.hpp>

namespace nomi
{

class Player : public sf::Drawable, public sf::Transformable
{
public:
    Player(float speed, float jumpHeight);
    virtual ~Player();

    void handleEvent( const sf::Event& ev );
    void handleInput(sf::Keyboard::Key key, bool isPressed);
    void update( sf::Time dt );
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    sf::Vector2f getPosition() { return mSprite.getPosition(); } // should update this


private:
    sf::Texture mTexture;
    sf::Sprite mSprite;

    float mSpeed;
    float mJumpHeight;
    bool  mCanJump; // player is allowed to jump
    float mWeight;  // how heavy are we ?
    
    sf::Vector2f mVelocity; // player velocity

    bool mIsMovingLeft;
    bool mIsMovingRight;
    bool mJump;

private:
    void onCollision( sf::Vector2f direction );
};

} // namespace nomi