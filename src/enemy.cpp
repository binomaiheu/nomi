#include "enemy.hpp"

namespace nomi
{

Enemy::Enemy( const sf::Texture& sheet, const CollisionMap& map, float speed )
    : Entity( SceneNode::Enemy, map, { 34.f, 62.f } )
    , mSpeed( speed )
    , mSprite( sheet )
{
    mSprite.setOrigin( { CharacterAnimations::FrameWidth / 2.f, float( CharacterAnimations::FrameHeight ) } );
    mAnimator.play( CharacterAnimations::get().walk );
    mSprite.setTextureRect( mAnimator.frame() );
}

void Enemy::stomp()
{
    if ( mSquashed ) return;
    mSquashed = true;
    mSquashTime = 0.f;
    setVelocity( 0.f, 0.f );
    mAnimator.play( CharacterAnimations::get().dead, true );
}

void Enemy::updateCurrent( sf::Time dt )
{
    if ( mSquashed ) {
        // sit there dizzy for a moment, then disappear
        mSquashTime += dt.asSeconds();
        if ( mSquashTime > 0.8f ) destroy();
    } else {
        setVelocity( mDirection * mSpeed, getVelocity().y );
        applyPhysics( dt );

        // turn around at walls and before walking off a ledge
        if ( mLastMove.hitLeft || mLastMove.hitRight ) {
            mDirection = mLastMove.hitLeft ? 1.f : -1.f;
        } else if ( onGround() ) {
            const sf::Vector2f p = getPosition();
            const sf::Vector2f ahead( p.x + mDirection * ( mHitbox.x / 2.f + 4.f ), p.y + 4.f );
            if ( !mMap.isGroundAt( ahead ) ) mDirection = -mDirection;
        }

        // fell out of the level
        if ( getPosition().y > mMap.pixelHeight() + 200.f ) destroy();
    }

    mAnimator.update( dt );
    mSprite.setTextureRect( mAnimator.frame() );
    mSprite.setScale( { mDirection > 0.f ? 1.f : -1.f, 1.f } );
}

void Enemy::drawCurrent( sf::RenderTarget& target, sf::RenderStates states ) const
{
    target.draw( mSprite, states );
}

} // namespace nomi
