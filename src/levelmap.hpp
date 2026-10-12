#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <tmxlite/Map.hpp>

#include "collisionmap.hpp"
#include "tilemaplayer.hpp"

// format  : http://doc.mapeditor.org/en/stable/reference/tmx-map-format
//
// nomi map conventions (see README) :
//  - tile layer "Ground"        : collision; every tile is solid unless the tile has bool property oneway=true
//  - other tile layers          : decoration, drawn behind the entities; layers whose name starts with
//                                 "Foreground" are drawn in front; Tiled's parallax factor is respected
//  - object layer "solids"      : extra solid rectangles
//  - object layer "entities"    : objects with class spawn / coin / enemy / goal
//  - map properties             : name (string), next (file of the next level, relative to this map)

namespace nomi
{

// an object from the "entities" layer
struct SpawnObject
{
    std::string                        cls;     // spawn, coin, enemy, goal
    sf::FloatRect                      rect;    // in map pixels
    std::map<std::string, std::string> props;

    std::string str( const std::string& name, const std::string& def = "" ) const;
    float       num( const std::string& name, float def ) const;

    sf::Vector2f bottomCenter() const { return { rect.position.x + rect.size.x / 2.f, rect.position.y + rect.size.y }; }
    sf::Vector2f center() const       { return rect.getCenter(); }
};

class LevelMap
{
public:
    LevelMap();
    ~LevelMap() = default;

    void load( const std::string& fileName );   // throws std::runtime_error on failure

    // get the dimensions
    float xmin( void ) const { return 0.f; }
    float ymin( void ) const { return 0.f; }
    float xmax( void ) const { return mCollision.pixelWidth(); }
    float ymax( void ) const { return mCollision.pixelHeight(); }

    void drawBackground( sf::RenderTarget& target ) const;
    void drawForeground( sf::RenderTarget& target ) const;

    const CollisionMap&             collision() const      { return mCollision; }
    const std::vector<SpawnObject>& objects() const        { return mObjects; }
    const std::string&              fileName() const       { return mFileName; }
    std::string                     property( const std::string& name ) const;
    std::string                     name() const;
    std::string                     nextLevel() const;     // full path, empty if this is the last level
    sf::Color                       backgroundColor() const { return mBackground; }

private:
    struct DrawLayer
    {
        std::unique_ptr<TileMapLayer> layer;
        sf::Vector2f                  parallax { 1.f, 1.f };
        bool                          foreground = false;
    };

    void buildCollision( const tmx::TileLayer& layer );
    void loadObjects( const tmx::ObjectGroup& group, bool solids );
    void drawLayers( sf::RenderTarget& target, bool foreground ) const;

    std::string                        mFileName;
    tmx::Map                           mMap;
    std::vector<DrawLayer>             mLayers;
    CollisionMap                       mCollision;
    std::vector<SpawnObject>           mObjects;
    std::map<std::string, std::string> mProperties;
    sf::Color                          mBackground { 120, 200, 255 };
    sf::Vector2f                       mParallaxOrigin;
};

// helper : the value of a tmx property as a string
std::string propertyToString( const tmx::Property& p );

} // namespace
