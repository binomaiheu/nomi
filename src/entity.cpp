#include "entity.hpp"

namespace nomi{

Entity::Entity()
    : SceneNode()
{}

Entity::~Entity()
{}

void Entity::updateCurrent( sf::Time dt )
{
    move( mVelocity * dt.asSeconds() );
}

} // namespace