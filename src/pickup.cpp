#include <cmath>

#include "pickup.hpp"

namespace nomi
{

// share/sprites/coin.png : 2 frames of 54x54 next to each other
Coin::Coin( const sf::Texture& texture )
    : SceneNode( SceneNode::Coin )
    , mSprite( texture )
{
    const int frame = int( texture.getSize().y );
    mSpin = makeRowAnimation( { frame, frame }, 0, { 0, 1 }, sf::seconds( 0.25f ) );
    mAnimator.play( mSpin );
    mSprite.setTextureRect( mAnimator.frame() );
    mSprite.setOrigin( { frame / 2.f, frame / 2.f } );
}

sf::FloatRect Coin::getBoundingRect() const
{
    const sf::Vector2f p = getWorldPosition();
    return sf::FloatRect( { p.x - 18.f, p.y - 18.f }, { 36.f, 36.f } );
}

void Coin::updateCurrent( sf::Time dt )
{
    mTime += dt.asSeconds();
    mAnimator.update( dt );
    mSprite.setTextureRect( mAnimator.frame() );
    // gentle bobbing
    mSprite.setPosition( { 0.f, 4.f * std::sin( mTime * 3.f ) } );
}

void Coin::drawCurrent( sf::RenderTarget& target, sf::RenderStates states ) const
{
    target.draw( mSprite, states );
}


// share/sprites/flag.png : 2 frames of 54x162 (flag + pole) next to each other
Goal::Goal( const sf::Texture& texture )
    : SceneNode( SceneNode::Goal )
    , mSprite( texture )
{
    const sf::Vector2i size( int( texture.getSize().x / 2 ), int( texture.getSize().y ) );
    mWave = makeRowAnimation( size, 0, { 0, 1 }, sf::seconds( 0.3f ) );
    mAnimator.play( mWave );
    mSprite.setTextureRect( mAnimator.frame() );
    // the pole is at the left side of the frame
    mSprite.setOrigin( { 12.f, float( size.y ) } );
}

sf::FloatRect Goal::getBoundingRect() const
{
    // a tall thin trigger around the pole, so jumping over the flag is not possible
    const sf::Vector2f p = getWorldPosition();
    return sf::FloatRect( { p.x - 10.f, p.y - 1000.f }, { 40.f, 1000.f } );
}

void Goal::updateCurrent( sf::Time dt )
{
    mAnimator.update( mReached ? dt * 3.f : dt );
    mSprite.setTextureRect( mAnimator.frame() );
}

void Goal::drawCurrent( sf::RenderTarget& target, sf::RenderStates states ) const
{
    target.draw( mSprite, states );
}

} // namespace nomi
