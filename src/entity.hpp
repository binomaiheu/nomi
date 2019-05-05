#pragma once 

#include "scenenode.hpp"

#include <SFML/System/Vector2.hpp>

namespace nomi
{

class Entity : public SceneNode
{
public:
    Entity();
    virtual ~Entity();

    void setVelocity( sf::Vector2f v ) { mVelocity = v; }
    void setVelocity( float vx, float vy ) { mVelocity.x = vx; mVelocity.y = vy; }

    sf::Vector2f getVelocity( void ) const { return mVelocity; };

protected:
	virtual void updateCurrent( sf::Time dt );

private: 
    sf::Vector2f  mVelocity;    
};


} // namespace