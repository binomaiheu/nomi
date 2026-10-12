#pragma once

#include <cstdint>
#include <vector>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>

namespace nomi
{

enum class TileCollision : std::uint8_t
{
    None,
    Solid,      // blocks from every side
    OneWay      // can be jumped through from below, stood on from above
};

struct MoveResult
{
    bool onGround   = false;
    bool hitLeft    = false;
    bool hitRight   = false;
    bool hitCeiling = false;
};

// Grid of collision tiles plus free-form solid rectangles (from the Tiled "solids" object layer).
// Moving boxes are resolved one axis at a time, which is stable and simple for a tile platformer.
class CollisionMap
{
public:
    CollisionMap() = default;
    CollisionMap( unsigned width, unsigned height, float tileSize );

    void          setTile( unsigned x, unsigned y, TileCollision c );
    TileCollision tileAt( int x, int y ) const;   // left/right of the map counts as a wall

    void          addSolidRect( const sf::FloatRect& r ) { mRects.push_back( r ); }
    const std::vector<sf::FloatRect>& solidRects() const { return mRects; }

    // is there something to stand on at this point (solid or one-way)?
    bool          isGroundAt( sf::Vector2f p ) const;

    // move the box by delta, stopping at solid geometry
    MoveResult    move( sf::FloatRect& box, sf::Vector2f delta ) const;

    sf::Vector2u  size() const     { return { mWidth, mHeight }; }
    float         tileSize() const { return mTileSize; }
    float         pixelWidth() const  { return mWidth * mTileSize; }
    float         pixelHeight() const { return mHeight * mTileSize; }

    // outline every collision tile and rectangle that is in view (debug mode)
    void          drawDebug( sf::RenderTarget& target ) const;

private:
    void          moveX( sf::FloatRect& box, float dx, MoveResult& res ) const;
    void          moveY( sf::FloatRect& box, float dy, MoveResult& res ) const;

    unsigned                   mWidth    = 0;
    unsigned                   mHeight   = 0;
    float                      mTileSize = 64.f;
    std::vector<TileCollision> mTiles;
    std::vector<sf::FloatRect> mRects;
};

} // namespace nomi
