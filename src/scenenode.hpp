#pragma once

#include <memory>
#include <vector>
#include <set>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

namespace nomi
{

class SceneNode : public sf::Transformable, public sf::Drawable,
                  private sf::NonCopyable
{
public:
    typedef std::pair<SceneNode*, SceneNode*> Pair;

    // categorisaion for the SceneNodes --> can add different types of scene nodes here...
    enum Type {
        None     = 0,        // no type        
        MapLayer = 1 << 0,   // a map tile layer
        Solid    = 1 << 1    // a solid object
    };

public: 
    explicit SceneNode( SceneNode::Type type = None );
    virtual ~SceneNode();

    void attachChild( std::unique_ptr<SceneNode> child );
    std::unique_ptr<SceneNode> detachChild( const SceneNode& node );

    void update( sf::Time dt );    

    sf::Vector2f			getWorldPosition() const;
	sf::Transform			getWorldTransform() const;

    void					checkSceneCollision(SceneNode& sceneGraph, std::set<Pair>& collisionPairs);
	void					checkNodeCollision(SceneNode& node, std::set<Pair>& collisionPairs);

    virtual sf::FloatRect   getBoundingRect() const;

    unsigned int            getType( void ) const { return mType; }

    virtual void            resolveCollision( const sf::Vector3f& man, const SceneNode& other );

private:
    virtual void draw( sf::RenderTarget& target, sf::RenderStates states ) const;    
    virtual void drawCurrent( sf::RenderTarget& target, sf::RenderStates states ) const;    

    virtual void updateCurrent( sf::Time dt );

private:
    SceneNode::Type                         mType;

    std::vector<std::unique_ptr<SceneNode>> mChildren;
    SceneNode*                              mParent;

}; // class


bool	collision(const SceneNode& lhs, const SceneNode& rhs);
float	distance(const SceneNode& lhs, const SceneNode& rhs);

} // namespace