#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>
#include <tmxlite/detail/Log.hpp>


// format  : http://doc.mapeditor.org/en/stable/reference/tmx-map-format
// example : https://www.sfml-dev.org/tutorials/2.5/graphics-vertex-array.php 

namespace nomi
{

class tilemap : public sf::Drawable, public sf::Transformable
{
public: 
    tilemap( const tmx::Map& map );
    ~tilemap();

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
    sf::VertexArray mVertices;
    sf::Texture     mTilesetTextures;

};

} // namespace