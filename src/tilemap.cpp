#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include "tilemap.hpp"

namespace nomi
{

tilemap::tilemap( const tmx::Map& map )
{
    std::cout << "[constructing tilemap]\n";
    std::cout << "Tiles : " << map.getTileCount().x << "x" << map.getTileCount().y << "\n";    
    std::cout << "Properties : " << map.getProperties().size() << "\n";
    for (const auto& p : map.getProperties() )
    {
        std::cout << " - Found property: " << p.getName() << std::endl;
        std::cout << " - Type: " << int(p.getType()) << std::endl;
    }
    std::cout << "Number of layers : " << map.getLayers().size() << "\n";
    for (const auto& l : map.getLayers() )
    {
        std::cout << " - Found Layer: " << l->getName() << std::endl;
        std::cout << " - Layer Type: " << int(l->getType()) << std::endl;
        std::cout << " - Layer Properties:" << std::endl;      

        for (const auto& p : l->getProperties() )
        {
            std::cout << "    " << p.getName() << ", type = " << int(p.getType()) << std::endl;
        }

    }    

    std::cout << "image : " << map.getTilesets().at(0).getImagePath() << "\n";
    // load tileset into texture
    mTilesetTextures.loadFromFile( map.getTilesets().at(0).getImagePath() );

    // prepare the quad arrqy (vertrex array) with the size of the tilemap
    mVertices.setPrimitiveType(sf::Quads);
    mVertices.resize(map.getTileCount().x * map.getTileCount().y * 4);
    
    auto& l = map.getLayers().at(0)->getLayerAs<tmx::TileLayer>();
    std::cout << "offset: x = " << l.getOffset().x << ", y = " << l.getOffset().y << "\n";
    
    for ( const auto& t : l.getTiles() ) 
    {
        std::cout << "tile coordinates : ";
        std::cout << "tile gid: " << t.ID << "\n"; 
        std::cout << "texture coordinates" << "\n";
    }


}
 

 tilemap::~tilemap()
 {}
    

 void tilemap::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // apply the transform
    states.transform *= getTransform();

    // apply the tileset texture
    states.texture = &mTilesetTextures;

    // draw the vertex array
    target.draw(mVertices, states);
}

} // namespace nomi

 