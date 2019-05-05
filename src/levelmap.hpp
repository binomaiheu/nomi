#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>
#include <tmxlite/detail/Log.hpp>

#include "tilemaplayer.hpp"

// format  : http://doc.mapeditor.org/en/stable/reference/tmx-map-format
// example : https://www.sfml-dev.org/tutorials/2.5/graphics-vertex-array.php 

namespace nomi
{

class LevelMap : public sf::Drawable, public sf::Transformable
{
public:     
    LevelMap();    
    ~LevelMap() = default;

    void load_tmx( const std::string& fileName );
    
    // get the dimensions 
    float xmin( void ) { return mMap.getBounds().left; }
    float ymin( void ) { return mMap.getBounds().top; }
    float xmax( void ) { return mMap.getBounds().left + mMap.getBounds().width; }
    float ymax( void ) { return mMap.getBounds().top  + mMap.getBounds().height; }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

private:
    sf::VertexArray mVertices;
    sf::Texture     mTilesetTextures;

    tmx::Map        mMap;
    std::vector<std::unique_ptr<TileMapLayer>> mTileMapLayers;
    //std::vector<std::unique_ptr<ObjectLayer>>  mObjectLayers;    
};

} // namespace