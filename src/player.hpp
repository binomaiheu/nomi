#pragma once

#include "animation.hpp"
#include "entity.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace nomi
{

class Player : public Entity
{
public:
    // what the player wants to do this tick (keyboard, gamepad or the autotest bot)
    struct Input
    {
        bool left  = false;
        bool right = false;
        bool jump  = false;   // held down
    };

    Player( const sf::Texture& sheet, const CollisionMap& map );
    virtual ~Player();

    void setInput( const Input& in ) { mInput = in; }

    void respawn( sf::Vector2f position );
    void bounce();                 // after stomping an enemy
    void kill();                   // start the death animation
    void celebrate();              // reached the goal

    bool isAlive() const          { return mState == State::Alive; }
    bool isDying() const          { return mState == State::Dying; }
    bool deathFinished() const    { return mState == State::Dying && mStateTime > 1.6f; }
    bool isInvulnerable() const   { return mInvulnerable > 0.f; }
    bool hasJumped()              { bool j = mJumped; mJumped = false; return j; }

protected:
    void updateCurrent( sf::Time dt ) override;
    void drawCurrent( sf::RenderTarget& target, sf::RenderStates states ) const override;

private:
    enum class State { Alive, Dying, Celebrating };

    void updateMovement( float dt );
    void updateAnimation( sf::Time dt );

    Input       mInput;
    bool        mJumpWasHeld   = false;
    float       mCoyote        = 0.f;   // time left to still jump after walking off a ledge
    float       mJumpBuffer    = 0.f;   // time left for a jump pressed slightly too early
    bool        mJumping       = false; // rising from a jump (for variable jump height)
    bool        mJumped        = false; // a jump started this tick (for the sound)
    float       mInvulnerable  = 0.f;
    bool        mFacingRight   = true;

    State       mState         = State::Alive;
    float       mStateTime     = 0.f;

    sf::Sprite  mSprite;
    Animator    mAnimator;
};

} // namespace nomi
