#include <algorithm>
#include <cassert>

#include "scenenode.hpp"

namespace nomi
{

SceneNode::SceneNode()
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

}