#pragma once

#include "animation.hpp"
#include "entity.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace nomi
{

// walks back and forth, turning at walls and at ledges; can be stomped
class Enemy : public Entity
{
public:
    Enemy( const sf::Texture& sheet, const CollisionMap& map, float speed );

    void stomp();
    void turnAround() { mDirection = -mDirection; }

    bool isAlive() const { return !mSquashed; }

protected:
    void updateCurrent( sf::Time dt ) override;
    void drawCurrent( sf::RenderTarget& target, sf::RenderStates states ) const override;

private:
    float       mSpeed;
    float       mDirection = -1.f;   // start walking towards the player
    bool        mSquashed  = false;
    float       mSquashTime = 0.f;

    sf::Sprite  mSprite;
    Animator    mAnimator;
};

} // namespace nomi
