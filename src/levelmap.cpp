#include <filesystem>
#include <stdexcept>

#include <SFML/Graphics/View.hpp>

#include <tmxlite/ObjectGroup.hpp>
#include <tmxlite/TileLayer.hpp>

#include "levelmap.hpp"

namespace nomi
{

std::string propertyToString( const tmx::Property& p )
{
    switch ( p.getType() )
    {
    case tmx::Property::Type::Boolean: return p.getBoolValue() ? "true" : "false";
    case tmx::Property::Type::Float:   return std::to_string( p.getFloatValue() );
    case tmx::Property::Type::Int:     return std::to_string( p.getIntValue() );
    case tmx::Property::Type::String:  return p.getStringValue();
    case tmx::Property::Type::File:    return p.getFileValue();
    default:                           return "";
    }
}

std::string SpawnObject::str( const std::string& name, const std::string& def ) const
{
    auto it = props.find( name );
    return it == props.end() ? def : it->second;
}

float SpawnObject::num( const std::string& name, float def ) const
{
    auto it = props.find( name );
    if ( it == props.end() ) return def;
    try { return std::stof( it->second ); } catch ( ... ) { return def; }
}

LevelMap::LevelMap()
{}

void LevelMap::load( const std::string& fileName )
{
    if ( !mMap.load( fileName ) )
        throw std::runtime_error( "[LevelMap] unable to load " + fileName );
    if ( mMap.getOrientation() != tmx::Orientation::Orthogonal )
        throw std::runtime_error( "[LevelMap] only orthogonal maps are supported : " + fileName );

    mFileName = fileName;
    mLayers.clear();
    mObjects.clear();
    mProperties.clear();

    const auto tileCount = mMap.getTileCount();
    mCollision = CollisionMap( tileCount.x, tileCount.y, float( mMap.getTileSize().x ) );

    for ( const auto& p : mMap.getProperties() )
        mProperties[ p.getName() ] = propertyToString( p );

    const auto bg = mMap.getBackgroundColour();
    if ( bg.a > 0 ) mBackground = sf::Color( bg.r, bg.g, bg.b );
    mParallaxOrigin = { mMap.getParallaxOrigin().x, mMap.getParallaxOrigin().y };

    for ( std::size_t i = 0; i < mMap.getLayers().size(); i++ )
    {
        const auto& layer = *mMap.getLayers()[i];
        switch ( layer.getType() )
        {
        case tmx::Layer::Type::Tile:
        {
            if ( layer.getName() == "Ground" ) buildCollision( layer.getLayerAs<tmx::TileLayer>() );
            if ( !layer.getVisible() ) break;
            DrawLayer dl;
            dl.layer      = std::make_unique<TileMapLayer>( mMap, i );
            dl.parallax   = { layer.getParallaxFactor().x, layer.getParallaxFactor().y };
            dl.foreground = layer.getName().rfind( "Foreground", 0 ) == 0;
            mLayers.push_back( std::move( dl ) );
            break;
        }
        case tmx::Layer::Type::Object:
        {
            const auto& group = layer.getLayerAs<tmx::ObjectGroup>();
            loadObjects( group, layer.getName() == "solids" );
            break;
        }
        default:
            break;
        }
    }
}

void LevelMap::buildCollision( const tmx::TileLayer& layer )
{
    const auto& tiles = layer.getTiles();
    const auto  count = mMap.getTileCount();
    for ( unsigned y = 0; y < count.y; ++y )
    {
        for ( unsigned x = 0; x < count.x; ++x )
        {
            const auto id = tiles[ std::size_t( y ) * count.x + x ].ID;
            if ( id == 0 ) continue;

            TileCollision c = TileCollision::Solid;
            for ( const auto& ts : mMap.getTilesets() )
            {
                if ( !ts.hasTile( id ) ) continue;
                if ( const auto* tile = ts.getTile( id ) )
                {
                    for ( const auto& p : tile->properties )
                    {
                        if ( p.getName() == "oneway" && p.getType() == tmx::Property::Type::Boolean && p.getBoolValue() )
                            c = TileCollision::OneWay;
                        if ( p.getName() == "solid" && p.getType() == tmx::Property::Type::Boolean && !p.getBoolValue() )
                            c = TileCollision::None;
                    }
                }
                break;
            }
            mCollision.setTile( x, y, c );
        }
    }
}

void LevelMap::loadObjects( const tmx::ObjectGroup& group, bool solids )
{
    for ( const auto& obj : group.getObjects() )
    {
        const auto& aabb = obj.getAABB();
        sf::FloatRect rect( { aabb.left, aabb.top }, { aabb.width, aabb.height } );

        if ( solids || obj.getClass() == "solid" )
        {
            if ( rect.size.x > 0.f && rect.size.y > 0.f ) mCollision.addSolidRect( rect );
            continue;
        }

        SpawnObject so;
        so.cls = obj.getClass();

        // tile objects inherit class and properties from their tile in the tileset
        if ( obj.getTileID() != 0 )
        {
            for ( const auto& ts : mMap.getTilesets() )
            {
                if ( !ts.hasTile( obj.getTileID() ) ) continue;
                if ( const auto* tile = ts.getTile( obj.getTileID() ) )
                {
                    if ( so.cls.empty() ) so.cls = tile->className;
                    for ( const auto& p : tile->properties ) so.props[ p.getName() ] = propertyToString( p );
                }
                break;
            }
            // tile objects are anchored at their bottom-left corner
            rect = sf::FloatRect( { obj.getPosition().x, obj.getPosition().y - aabb.height }, { aabb.width, aabb.height } );
        }
        for ( const auto& p : obj.getProperties() ) so.props[ p.getName() ] = propertyToString( p );
        so.rect = rect;

        if ( !so.cls.empty() ) mObjects.push_back( std::move( so ) );
    }
}

std::string LevelMap::property( const std::string& name ) const
{
    auto it = mProperties.find( name );
    return it == mProperties.end() ? std::string() : it->second;
}

std::string LevelMap::name() const
{
    const std::string n = property( "name" );
    return n.empty() ? std::filesystem::path( mFileName ).stem().string() : n;
}

std::string LevelMap::nextLevel() const
{
    const std::string next = property( "next" );
    if ( next.empty() ) return {};
    return ( std::filesystem::path( mFileName ).parent_path() / next ).generic_string();
}

void LevelMap::drawBackground( sf::RenderTarget& target ) const
{
    drawLayers( target, false );
}

void LevelMap::drawForeground( sf::RenderTarget& target ) const
{
    drawLayers( target, true );
}

void LevelMap::drawLayers( sf::RenderTarget& target, bool foreground ) const
{
    const sf::View view = target.getView();
    for ( const auto& dl : mLayers )
    {
        if ( dl.foreground != foreground ) continue;
        if ( dl.parallax != sf::Vector2f( 1.f, 1.f ) )
        {
            // same as Tiled : the layer scrolls with factor p relative to the parallax origin
            sf::View pv = view;
            const sf::Vector2f c = view.getCenter() - mParallaxOrigin;
            pv.setCenter( mParallaxOrigin + sf::Vector2f( c.x * dl.parallax.x, c.y * dl.parallax.y ) );
            target.setView( pv );
            target.draw( *dl.layer );
            target.setView( view );
        }
        else
        {
            target.draw( *dl.layer );
        }
    }
}

} // namespace nomi
