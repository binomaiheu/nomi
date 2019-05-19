#pragma once

#include <memory>

#include <tmxlite/Object.hpp>
#include <SFML/Graphics.hpp>

#include "scenenode.hpp"

namespace nomi
{

class Solid : public SceneNode
{
public:    
    Solid( const tmx::Object &obj ); 
    virtual ~Solid();

    virtual void drawCurrent( sf::RenderTarget& target, sf::RenderStates states ) const;    
    virtual void updateCurrent( sf::Time dt );

    sf::FloatRect getBoundingRect() const;

private:
    std::unique_ptr<sf::Shape>  mShape;
};


}