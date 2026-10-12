#pragma once

#include "collisionmap.hpp"
#include "scenenode.hpp"

#include <SFML/System/Vector2.hpp>

namespace nomi
{

// A moving object with a hitbox that collides with the level geometry.
// The node position is the bottom-centre of the hitbox (where the feet are).
class Entity : public SceneNode
{
public:
    Entity( SceneNode::Type type, const CollisionMap& map, sf::Vector2f hitboxSize );
    virtual ~Entity();

    void setVelocity( sf::Vector2f v ) { mVelocity = v; }
    void setVelocity( float vx, float vy ) { mVelocity.x = vx; mVelocity.y = vy; }
    sf::Vector2f getVelocity( void ) const { return mVelocity; };

    bool onGround() const { return mLastMove.onGround; }

    sf::FloatRect getBoundingRect() const override;

    // bottom of the hitbox at the start of the current tick (used for stomp detection)
    float previousBottom() const { return mPrevBottom; }

    void destroy() { mDestroyed = true; }
    bool isMarkedForRemoval() const override { return mDestroyed; }

protected:
    // apply gravity and move through the collision map; sets mLastMove
    void applyPhysics( sf::Time dt, bool collide = true );

    const CollisionMap& mMap;
    sf::Vector2f        mHitbox;
    MoveResult          mLastMove;

private:
    sf::Vector2f  mVelocity;
    float         mPrevBottom = 0.f;
    bool          mDestroyed  = false;
};


} // namespace
