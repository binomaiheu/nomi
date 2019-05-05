#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include "levelmap.hpp"

namespace nomi
{

LevelMap::LevelMap()
{}

void LevelMap::load_tmx( const std::string& fileName )
{
    try {
        mMap.load( fileName );
    } catch ( std::exception& e ) {
        throw std::runtime_error( "[LevelMap] unable to load " + fileName );
    }

    std::cout << "[Constructing LevelMap]\n";
    std::cout << " Tiles : " << mMap.getTileCount().x << "x" << mMap.getTileCount().y << "\n";    
    std::cout << " Dimensions : xmin=" << xmin() << ", ymin=" << ymin() 
                                       << ", xmax=" << xmax() << ", ymax=" << ymax() << "\n";
    if ( mMap.getProperties().size() > 0 )
    {        
        std::cout << " Properties : name [type]\n";
        for (const auto& p : mMap.getProperties() )
            std::cout << "  " << p.getName() << " [ " << int(p.getType()) << "  ] " << std::endl;
    }

    std::cout << " Number of layers : " << mMap.getLayers().size() << "\n";
    for ( unsigned int i = 0; i < mMap.getLayers().size(); i++ ) 
    {        
        switch ( mMap.getLayers().at(i)->getType() ) 
        { 
            case tmx::Layer::Type::Tile: 
                std::cout << "  Adding tile layer : " << mMap.getLayers().at(i)->getName() << "\n";
                mTileMapLayers.push_back( std::move( std::make_unique<TileMapLayer>( mMap, i ) ) );
                break;

            case tmx::Layer::Type::Object: 
                std::cout << "  Adding object layer : " << mMap.getLayers().at(i)->getName() << "\n";
                const auto& objects = mMap.getLayers().at(i)->getLayerAs<tmx::ObjectGroup>().getObjects();
                std::cout << "   Found " << objects.size() << " objects in layer" << std::endl;
                for(const auto& object : objects)
                {
                    std::cout << "    Object name= [" << object.getName() 
                              << "], type = " << object.getType()
                              << ", x= " << object.getPosition().x 
                              << ", y= " << object.getPosition().y << "\n";
                    std::cout << "            aabb : " << object.getAABB().left << ", " << object.getAABB().top << ", " << object.getAABB().width << ", " << object.getAABB().height << "\n";
                    std::cout << "            points : ( ";
                    for ( auto& p : object.getPoints() ) std::cout << " ( " << p.x << ", " << p.y << " )";
                    std::cout << ")\n";

                    const auto& properties = object.getProperties();
                    std::cout << "     Object has " << properties.size() << " properties" << std::endl;
                    for(const auto& prop : properties)
                    {
                        std::cout << "      Found property: " << prop.getName() << ", type: " << int(prop.getType()) << std::endl;
                    }
                }

                break;            
        }
        
        if ( mMap.getLayers().at(i)->getProperties().size() > 0 ) 
        {
            std::cout << "   Properties : name [type]\n";
            for (const auto& p : mMap.getLayers().at(i)->getProperties() )
                std::cout << "  " << p.getName() << " [ " << int(p.getType()) << "  ] " << std::endl;
        }
    }    


}
 
void LevelMap::draw(sf::RenderTarget& target, sf::RenderStates states) const
{  
    // draw the tilemap layers
    for ( const auto& lyr : mTileMapLayers ) {
        target.draw( *lyr );
    }
}

} // namespace nomi

 