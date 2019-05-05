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

public: 
    explicit SceneNode();
    virtual ~SceneNode();

    void attachChild( std::unique_ptr<SceneNode> child );
    std::unique_ptr<SceneNode> detachChild( const SceneNode& node );

    void update( sf::Time dt );    

    sf::Vector2f			getWorldPosition() const;
	sf::Transform			getWorldTransform() const;

    void					checkSceneCollision(SceneNode& sceneGraph, std::set<Pair>& collisionPairs);
	void					checkNodeCollision(SceneNode& node, std::set<Pair>& collisionPairs);

    virtual sf::FloatRect   getBoundingRect() const;

private:
    virtual void draw( sf::RenderTarget& target, sf::RenderStates states ) const;    
    virtual void drawCurrent( sf::RenderTarget& target, sf::RenderStates states ) const;    

    virtual void updateCurrent( sf::Time dt );

private:
    std::vector<std::unique_ptr<SceneNode>> mChildren;
    SceneNode*                              mParent;

}; // class


bool	collision(const SceneNode& lhs, const SceneNode& rhs);
float	distance(const SceneNode& lhs, const SceneNode& rhs);

} // namespace