#include <algorithm>
#include <cmath>

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/View.hpp>

#include "collisionmap.hpp"

namespace nomi
{

namespace
{
constexpr float Eps = 0.01f;

bool overlaps( const sf::FloatRect& a, const sf::FloatRect& b )
{
    return a.position.x < b.position.x + b.size.x && a.position.x + a.size.x > b.position.x &&
           a.position.y < b.position.y + b.size.y && a.position.y + a.size.y > b.position.y;
}
}

CollisionMap::CollisionMap( unsigned width, unsigned height, float tileSize )
    : mWidth( width )
    , mHeight( height )
    , mTileSize( tileSize )
    , mTiles( std::size_t( width ) * height, TileCollision::None )
{
}

void CollisionMap::setTile( unsigned x, unsigned y, TileCollision c )
{
    if ( x < mWidth && y < mHeight ) mTiles[ std::size_t( y ) * mWidth + x ] = c;
}

TileCollision CollisionMap::tileAt( int x, int y ) const
{
    if ( x < 0 || x >= int( mWidth ) ) return TileCollision::Solid;  // walls at the map sides
    if ( y < 0 || y >= int( mHeight ) ) return TileCollision::None;  // open sky above, pits below
    return mTiles[ std::size_t( y ) * mWidth + x ];
}

bool CollisionMap::isGroundAt( sf::Vector2f p ) const
{
    const int tx = int( std::floor( p.x / mTileSize ) );
    const int ty = int( std::floor( p.y / mTileSize ) );
    if ( tileAt( tx, ty ) != TileCollision::None ) return true;
    for ( const auto& r : mRects )
        if ( r.contains( p ) ) return true;
    return false;
}

MoveResult CollisionMap::move( sf::FloatRect& box, sf::Vector2f delta ) const
{
    // sub-step so that a fast box never skips over a whole tile
    const float maxStep = mTileSize / 4.f;
    const int steps = std::max( 1, int( std::ceil( std::max( std::abs( delta.x ), std::abs( delta.y ) ) / maxStep ) ) );
    const sf::Vector2f step = delta / float( steps );

    MoveResult res;
    for ( int i = 0; i < steps; ++i )
    {
        if ( step.x != 0.f && !( res.hitLeft || res.hitRight ) ) moveX( box, step.x, res );
        if ( step.y != 0.f && !( res.onGround || res.hitCeiling ) ) moveY( box, step.y, res );
    }
    return res;
}

void CollisionMap::moveX( sf::FloatRect& box, float dx, MoveResult& res ) const
{
    box.position.x += dx;

    const float top    = box.position.y;
    const float bottom = box.position.y + box.size.y;
    const int   row0   = int( std::floor( top / mTileSize ) );
    const int   row1   = int( std::floor( ( bottom - Eps ) / mTileSize ) );

    if ( dx > 0.f ) {
        const int col = int( std::floor( ( box.position.x + box.size.x - Eps ) / mTileSize ) );
        for ( int r = row0; r <= row1; ++r ) {
            if ( tileAt( col, r ) == TileCollision::Solid ) {
                box.position.x = col * mTileSize - box.size.x;
                res.hitRight = true;
                break;
            }
        }
    } else {
        const int col = int( std::floor( box.position.x / mTileSize ) );
        for ( int r = row0; r <= row1; ++r ) {
            if ( tileAt( col, r ) == TileCollision::Solid ) {
                box.position.x = ( col + 1 ) * mTileSize;
                res.hitLeft = true;
                break;
            }
        }
    }

    for ( const auto& r : mRects ) {
        if ( !overlaps( box, r ) ) continue;
        if ( dx > 0.f ) { box.position.x = r.position.x - box.size.x;  res.hitRight = true; }
        else            { box.position.x = r.position.x + r.size.x;    res.hitLeft  = true; }
    }
}

void CollisionMap::moveY( sf::FloatRect& box, float dy, MoveResult& res ) const
{
    const float prevTop    = box.position.y;
    const float prevBottom = box.position.y + box.size.y;
    box.position.y += dy;

    const float left  = box.position.x;
    const float right = box.position.x + box.size.x;
    const int   col0  = int( std::floor( left / mTileSize ) );
    const int   col1  = int( std::floor( ( right - Eps ) / mTileSize ) );

    if ( dy > 0.f ) {
        const int row = int( std::floor( ( box.position.y + box.size.y - Eps ) / mTileSize ) );
        const float rowTop = row * mTileSize;
        for ( int c = col0; c <= col1; ++c ) {
            const auto t = tileAt( c, row );
            // a one-way tile only catches us if we were above it before this step
            if ( t == TileCollision::Solid || ( t == TileCollision::OneWay && prevBottom <= rowTop + Eps ) ) {
                box.position.y = rowTop - box.size.y;
                res.onGround = true;
                break;
            }
        }
        for ( const auto& r : mRects ) {
            if ( overlaps( box, r ) && prevBottom <= r.position.y + Eps ) {
                box.position.y = r.position.y - box.size.y;
                res.onGround = true;
            }
        }
    } else {
        const int row = int( std::floor( box.position.y / mTileSize ) );
        for ( int c = col0; c <= col1; ++c ) {
            if ( tileAt( c, row ) == TileCollision::Solid ) {
                box.position.y = ( row + 1 ) * mTileSize;
                res.hitCeiling = true;
                break;
            }
        }
        for ( const auto& r : mRects ) {
            if ( overlaps( box, r ) && prevTop >= r.position.y + r.size.y - Eps ) {
                box.position.y = r.position.y + r.size.y;
                res.hitCeiling = true;
            }
        }
    }
}

void CollisionMap::drawDebug( sf::RenderTarget& target ) const
{
    const sf::View& view = target.getView();
    const sf::Vector2f tl = view.getCenter() - view.getSize() / 2.f;
    const int c0 = std::max( 0, int( tl.x / mTileSize ) );
    const int r0 = std::max( 0, int( tl.y / mTileSize ) );
    const int c1 = std::min( int( mWidth ) - 1, int( ( tl.x + view.getSize().x ) / mTileSize ) );
    const int r1 = std::min( int( mHeight ) - 1, int( ( tl.y + view.getSize().y ) / mTileSize ) );

    sf::RectangleShape shape( { mTileSize - 2.f, mTileSize - 2.f } );
    shape.setFillColor( sf::Color::Transparent );
    shape.setOutlineThickness( 1.f );
    for ( int r = r0; r <= r1; ++r ) {
        for ( int c = c0; c <= c1; ++c ) {
            const auto t = tileAt( c, r );
            if ( t == TileCollision::None ) continue;
            shape.setOutlineColor( t == TileCollision::Solid ? sf::Color::Red : sf::Color::Yellow );
            shape.setPosition( { c * mTileSize + 1.f, r * mTileSize + 1.f } );
            target.draw( shape );
        }
    }
    shape.setOutlineColor( sf::Color::Magenta );
    for ( const auto& r : mRects ) {
        shape.setSize( r.size );
        shape.setPosition( r.position );
        target.draw( shape );
    }
}

} // namespace nomi
