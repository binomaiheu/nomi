#pragma once

#include <memory>
#include <vector>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

namespace nomi
{

class SceneNode : public sf::Transformable, public sf::Drawable,
                  private sf::NonCopyable
{
public: 
    SceneNode();
    virtual ~SceneNode();

    void attachChild( std::unique_ptr<SceneNode> child );
    std::unique_ptr<SceneNode> detachChild( const SceneNode& node );

    void update( sf::Time dt );    

private:
    virtual void draw( sf::RenderTarget& target, sf::RenderStates states ) const;    
    virtual void drawCurrent( sf::RenderTarget& target, sf::RenderStates states ) const;    

    virtual void updateCurrent( sf::Time dt );

private:
    std::vector<std::unique_ptr<SceneNode>> mChildren;
    SceneNode*                              mParent;

}; // class


} // namespace