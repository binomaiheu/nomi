#include <algorithm>
#include <cassert>

#include "utils.hpp"
#include "scenenode.hpp"

namespace nomi
{

SceneNode::SceneNode( SceneNode::Type type )
    : mType( type )
    , mParent( nullptr )
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

void SceneNode::drawCurrent( sf::RenderTarget&, sf::RenderStates ) const
{
    // do nothing by default needs to be overwritten in derived class
}

void SceneNode::updateCurrent( sf::Time )
{
    // do nothing by default needs to be overwritten in derived class
}

void SceneNode::draw( sf::RenderTarget& target, sf::RenderStates states ) const
{
    if ( gDebugDraw ) drawBoundingRect( target );

    states.transform *= getTransform();
    drawCurrent( target, states ); // draw current node
    // draw children
    for ( const auto& c : mChildren ) c->draw( target, states );
}

void SceneNode::drawBoundingRect( sf::RenderTarget& target ) const
{
    const sf::FloatRect rect = getBoundingRect();
    if ( rect.size.x <= 0.f || rect.size.y <= 0.f ) return;

    sf::RectangleShape shape( rect.size );
    shape.setPosition( rect.position );
    shape.setFillColor( sf::Color::Transparent );
    shape.setOutlineColor( sf::Color::Green );
    shape.setOutlineThickness( 1.f );
    target.draw( shape );
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
	if (this != &node && collision(*this, node) )
		collisionPairs.insert(std::minmax(this, &node));

	for ( auto& child : mChildren)
		child->checkNodeCollision(node, collisionPairs);
}

void SceneNode::removeWrecks()
{
    mChildren.erase( std::remove_if( mChildren.begin(), mChildren.end(),
                         []( const std::unique_ptr<SceneNode>& c ) { return c->isMarkedForRemoval(); } ),
                     mChildren.end() );
    for ( auto& c : mChildren ) c->removeWrecks();
}

void SceneNode::collectBoundingRects( Type type, std::vector<sf::FloatRect>& out ) const
{
    if ( mType == type && !isMarkedForRemoval() ) out.push_back( getBoundingRect() );
    for ( const auto& c : mChildren ) c->collectBoundingRects( type, out );
}

sf::FloatRect SceneNode::getBoundingRect() const
{
	return sf::FloatRect();
}


bool collision(const SceneNode& lhs, const SceneNode& rhs)
{
	return lhs.getBoundingRect().findIntersection(rhs.getBoundingRect()).has_value();
}

float distance(const SceneNode& lhs, const SceneNode& rhs)
{
	return length( lhs.getWorldPosition() - rhs.getWorldPosition() );
}


}
