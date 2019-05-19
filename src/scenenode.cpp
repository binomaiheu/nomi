#include <iostream>
#include <algorithm>
#include <cassert>

#include "utils.hpp"
#include "scenenode.hpp"

namespace nomi
{

SceneNode::SceneNode( SceneNode::Type type )
    : mType( type )
{    
}

SceneNode::~SceneNode()
{
}

void SceneNode::attachChild( std::unique_ptr<SceneNode> child )
{
    child->mParent = this;
    mChildren.push_back( std::move( child ) );
}

std::unique_ptr<SceneNode> SceneNode::detachChild( const SceneNode& node )
{
    auto found = std::find_if( mChildren.begin(), mChildren.end(), 
        [&] ( std::unique_ptr<SceneNode>& p ) -> bool { return p.get() == &node; } );
    assert( found  != mChildren.end() );

    std::unique_ptr<SceneNode> result = std::move( *found );
    result->mParent = nullptr;
    mChildren.erase(found);
    return result;
}

void SceneNode::drawCurrent( sf::RenderTarget& target, sf::RenderStates states ) const 
{
    // do nothing by default needs to be overwritten in derived class
}

void SceneNode::updateCurrent( sf::Time dt ) 
{
    // do nothing by default needs to be overwritten in derived class
}


void SceneNode::draw( sf::RenderTarget& target, sf::RenderStates states ) const 
{
    states.transform *= getTransform();
    drawCurrent( target, states ); // draw current node
    // draw children
    for ( const auto& c : mChildren ) c->draw( target, states );
}

void SceneNode::update( sf::Time dt )
{
    updateCurrent(dt);
    // update children
    for ( const auto& c : mChildren ) c->update( dt );
}

sf::Vector2f SceneNode::getWorldPosition() const
{
	return getWorldTransform() * sf::Vector2f();
}

sf::Transform SceneNode::getWorldTransform() const
{
	sf::Transform transform = sf::Transform::Identity;

	for (const SceneNode* node = this; node != nullptr; node = node->mParent)
		transform = node->getTransform() * transform;

	return transform;
}

void SceneNode::checkSceneCollision(SceneNode& sceneGraph, std::set<Pair>& collisionPairs)
{
	checkNodeCollision(sceneGraph, collisionPairs);

	for( auto& child : sceneGraph.mChildren)
		checkSceneCollision(*child, collisionPairs);
}

void SceneNode::checkNodeCollision(SceneNode& node, std::set<Pair>& collisionPairs)
{

    /*
    std::cout << "Checking collision between this node at : (" 
              << this->getPosition().x << ", " << this->getPosition().y 
              << ") and node at position "<< node.getPosition().x << ", " << node.getPosition().y << ") \n";
    std::cout << " --> this rect: " << this->getBoundingRect().left << ", " << this->getBoundingRect().top << ", " << this->getBoundingRect().width << ", " << this->getBoundingRect().height << "\n"; 
    std::cout << " --> node rect: " << node.getBoundingRect().left << ", " << node.getBoundingRect().top << ", " << node.getBoundingRect().width << ", " << node.getBoundingRect().height << "\n";     
    */

	if (this != &node && collision(*this, node) )
		collisionPairs.insert(std::minmax(this, &node));

	for ( auto& child : mChildren)
		child->checkNodeCollision(node, collisionPairs);
}

sf::FloatRect SceneNode::getBoundingRect() const
{
	return sf::FloatRect();
}

void SceneNode::resolveCollision( const sf::Vector3f& man, const SceneNode& other )
{
    // do nothing here, resolve in the dauther class
}


bool collision(const SceneNode& lhs, const SceneNode& rhs)
{
	return lhs.getBoundingRect().intersects(rhs.getBoundingRect());
}

float distance(const SceneNode& lhs, const SceneNode& rhs)
{
	return length( lhs.getWorldPosition() - rhs.getWorldPosition() );
}


}