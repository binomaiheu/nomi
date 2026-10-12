#include <algorithm>

#include "entity.hpp"
#include "physics.hpp"

namespace nomi{

Entity::Entity( SceneNode::Type type, const CollisionMap& map, sf::Vector2f hitboxSize )
    : SceneNode( type )
    , mMap( map )
    , mHitbox( hitboxSize )
{}

Entity::~Entity()
{}

sf::FloatRect Entity::getBoundingRect() const
{
    const sf::Vector2f p = getWorldPosition();
    return sf::FloatRect( { p.x - mHitbox.x / 2.f, p.y - mHitbox.y }, mHitbox );
}

void Entity::applyPhysics( sf::Time dt, bool collide )
{
    const float s = dt.asSeconds();
    mPrevBottom = getPosition().y;

    mVelocity.y = std::min( mVelocity.y + phys::Gravity * s, phys::MaxFallSpeed );

    if ( !collide ) {
        move( mVelocity * s );
        mLastMove = MoveResult();
        return;
    }

    sf::FloatRect box = getBoundingRect();
    mLastMove = mMap.move( box, mVelocity * s );
    setPosition( { box.position.x + mHitbox.x / 2.f, box.position.y + mHitbox.y } );

    if ( mLastMove.onGround || mLastMove.hitCeiling ) mVelocity.y = 0.f;
    if ( mLastMove.hitLeft  || mLastMove.hitRight )   mVelocity.x = 0.f;
}

} // namespace
