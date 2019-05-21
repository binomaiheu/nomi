#pragma once

#include "entity.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

namespace nomi
{

class Player : public Entity
{
public:
    Player(float speed, float jumpHeight);
    virtual ~Player();

    void handleEvent( const sf::Event& ev );
    void handleInput(sf::Keyboard::Key key, bool isPressed);
    
    void updateCurrent( sf::Time dt );
    void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;


    sf::FloatRect getBoundingRect() const;
    
    void resolveCollision( const sf::Vector3f& man, const SceneNode& other );

private:
    sf::Texture mTexture;
    sf::Sprite mSprite;

    float mSpeed;
    float mJumpHeight;
    bool  mCanJump; // player is allowed to jump
    float mWeight;  // how heavy are we ?

    bool mIsMovingLeft;
    bool mIsMovingRight;
    bool mJump;  

private:
    void onCollision( sf::Vector2f direction );
};

} // namespace nomi