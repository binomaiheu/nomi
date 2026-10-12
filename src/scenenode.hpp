#pragma once

#include <memory>
#include <vector>
#include <set>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

namespace nomi
{

class SceneNode : public sf::Transformable, public sf::Drawable
{
public:
    typedef std::pair<SceneNode*, SceneNode*> Pair;

    // categorisation for the SceneNodes --> can add different types of scene nodes here...
    enum Type {
        None     = 0,        // no type
        Player   = 1 << 0,
        Enemy    = 1 << 1,
        Coin     = 1 << 2,
        Goal     = 1 << 3
    };

public:
    explicit SceneNode( SceneNode::Type type = None );
    virtual ~SceneNode();
    SceneNode( const SceneNode& ) = delete;
    SceneNode& operator=( const SceneNode& ) = delete;

    void attachChild( std::unique_ptr<SceneNode> child );
    std::unique_ptr<SceneNode> detachChild( const SceneNode& node );

    void update( sf::Time dt );

    sf::Vector2f			getWorldPosition() const;
	sf::Transform			getWorldTransform() const;

    void					checkSceneCollision(SceneNode& sceneGraph, std::set<Pair>& collisionPairs);
	void					checkNodeCollision(SceneNode& node, std::set<Pair>& collisionPairs);

    // remove children that are marked for removal (dead enemies, collected coins, ...)
    void                    removeWrecks();
    virtual bool            isMarkedForRemoval() const { return false; }

    virtual sf::FloatRect   getBoundingRect() const;

    // bounding rectangles of all nodes of the given type in this subtree
    void                    collectBoundingRects( Type type, std::vector<sf::FloatRect>& out ) const;

    Type                    getType( void ) const { return mType; }

protected:
    virtual void drawCurrent( sf::RenderTarget& target, sf::RenderStates states ) const;
    virtual void updateCurrent( sf::Time dt );

private:
    void draw( sf::RenderTarget& target, sf::RenderStates states ) const override;
    void drawBoundingRect( sf::RenderTarget& target ) const;

private:
    SceneNode::Type                         mType;

    std::vector<std::unique_ptr<SceneNode>> mChildren;
    SceneNode*                              mParent;

}; // class


bool	collision(const SceneNode& lhs, const SceneNode& rhs);
float	distance(const SceneNode& lhs, const SceneNode& rhs);

} // namespace
