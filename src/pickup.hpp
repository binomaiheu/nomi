#pragma once

#include "animation.hpp"
#include "scenenode.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace nomi
{

// a spinning coin that floats in place; the position is its centre
class Coin : public SceneNode
{
public:
    explicit Coin( const sf::Texture& texture );

    void collect()              { mCollected = true; }
    bool isCollected() const    { return mCollected; }
    bool isMarkedForRemoval() const override { return mCollected; }

    sf::FloatRect getBoundingRect() const override;

protected:
    void updateCurrent( sf::Time dt ) override;
    void drawCurrent( sf::RenderTarget& target, sf::RenderStates states ) const override;

private:
    sf::Sprite mSprite;
    Animation  mSpin;
    Animator   mAnimator;
    float      mTime = 0.f;
    bool       mCollected = false;
};

// the finish flag; the position is the bottom centre of the pole
class Goal : public SceneNode
{
public:
    explicit Goal( const sf::Texture& texture );

    void reach()               { mReached = true; }
    bool isReached() const     { return mReached; }

    sf::FloatRect getBoundingRect() const override;

protected:
    void updateCurrent( sf::Time dt ) override;
    void drawCurrent( sf::RenderTarget& target, sf::RenderStates states ) const override;

private:
    sf::Sprite mSprite;
    Animation  mWave;
    Animator   mAnimator;
    bool       mReached = false;
};

} // namespace nomi
