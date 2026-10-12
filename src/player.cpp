#include <algorithm>
#include <cmath>

#include "physics.hpp"
#include "player.hpp"

namespace nomi
{

Player::Player( const sf::Texture& sheet, const CollisionMap& map )
    : Entity( SceneNode::Player, map, { 30.f, 66.f } )
    , mSprite( sheet )
{
    // choose the figure via the rectangle, origin at the bottom center (the feet)
    mSprite.setTextureRect( CharacterAnimations::get().idle.frames.front() );
    mSprite.setOrigin( { CharacterAnimations::FrameWidth / 2.f, float( CharacterAnimations::FrameHeight ) } );
    mAnimator.play( CharacterAnimations::get().idle );
}

Player::~Player()
{
}

void Player::respawn( sf::Vector2f position )
{
    setPosition( position );
    setVelocity( 0.f, 0.f );
    mState = State::Alive;
    mStateTime = 0.f;
    mInvulnerable = phys::InvulnerableTime;
    mJumping = false;
    mFacingRight = true;
}

void Player::bounce()
{
    auto v = getVelocity();
    // holding jump while stomping gives a higher bounce, like in the classics
    v.y = mInput.jump ? -phys::jumpVelocity() : -phys::StompBounce;
    mJumping = mInput.jump;
    setVelocity( v );
}

void Player::kill()
{
    if ( mState != State::Alive ) return;
    mState = State::Dying;
    mStateTime = 0.f;
    setVelocity( 0.f, -900.f );   // the classic hop before falling off the screen
    mAnimator.play( CharacterAnimations::get().dead, true );
}

void Player::celebrate()
{
    if ( mState != State::Alive ) return;
    mState = State::Celebrating;
    mStateTime = 0.f;
    setVelocity( 0.f, 0.f );
}

void Player::updateCurrent( sf::Time dt )
{
    const float s = dt.asSeconds();
    mStateTime += s;
    mInvulnerable = std::max( 0.f, mInvulnerable - s );

    switch ( mState )
    {
    case State::Alive:
        updateMovement( s );
        applyPhysics( dt );
        break;
    case State::Dying:
        // fall through the floor, no collisions
        if ( mStateTime > 0.4f ) applyPhysics( dt, false );
        break;
    case State::Celebrating:
        mInput = Input();
        updateMovement( s );
        applyPhysics( dt );
        break;
    }

    updateAnimation( dt );
}

void Player::updateMovement( float dt )
{
    sf::Vector2f v = getVelocity();

    // horizontal : accelerate towards the wanted speed, use friction to stop
    const int dir = ( mInput.right ? 1 : 0 ) - ( mInput.left ? 1 : 0 );
    if ( dir != 0 ) {
        const float accel = onGround() ? phys::PlayerAccel : phys::PlayerAirAccel;
        // turning around is quicker than speeding up
        const float boost = ( v.x * dir < 0.f ) ? 2.f : 1.f;
        v.x += dir * accel * boost * dt;
        v.x = std::clamp( v.x, -phys::PlayerMaxSpeed, phys::PlayerMaxSpeed );
        mFacingRight = dir > 0;
    } else {
        const float drop = phys::PlayerFriction * dt;
        v.x = std::abs( v.x ) <= drop ? 0.f : v.x - std::copysign( drop, v.x );
    }

    // jumping : coyote time and jump buffering make it feel forgiving
    mCoyote = onGround() ? phys::CoyoteTime : std::max( 0.f, mCoyote - dt );

    const bool jumpPressed = mInput.jump && !mJumpWasHeld;
    mJumpWasHeld = mInput.jump;
    mJumpBuffer = jumpPressed ? phys::JumpBufferTime : std::max( 0.f, mJumpBuffer - dt );

    if ( mJumpBuffer > 0.f && mCoyote > 0.f ) {
        v.y = -phys::jumpVelocity();
        mJumpBuffer = 0.f;
        mCoyote = 0.f;
        mJumping = true;
        mJumped = true;
    }

    // releasing the jump key early cuts the jump short : tap for small hops
    if ( mJumping && !mInput.jump && v.y < 0.f ) {
        v.y *= phys::JumpCutFactor;
        mJumping = false;
    }
    if ( v.y >= 0.f ) mJumping = false;

    setVelocity( v );
}

void Player::updateAnimation( sf::Time dt )
{
    const auto& anims = CharacterAnimations::get();
    const sf::Vector2f v = getVelocity();

    if ( mState == State::Dying ) {
        // keep the dizzy animation
    } else if ( !onGround() ) {
        mAnimator.play( v.y < 0.f ? anims.jump : anims.fall );
    } else if ( std::abs( v.x ) > 20.f ) {
        mAnimator.play( anims.walk );
    } else {
        mAnimator.play( anims.idle );
    }
    mAnimator.update( dt );
    mSprite.setTextureRect( mAnimator.frame() );
    mSprite.setScale( { mFacingRight ? 1.f : -1.f, 1.f } );
}

void Player::drawCurrent( sf::RenderTarget& target, sf::RenderStates states ) const
{
    // blink while invulnerable
    if ( mInvulnerable > 0.f && int( mInvulnerable * 10.f ) % 2 == 0 ) return;
    target.draw( mSprite, states );
}

} // namespace nomi
