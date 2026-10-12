#include <cstdio>

#include <SFML/Graphics/Sprite.hpp>

#include "hud.hpp"
#include "utils.hpp"

namespace nomi
{

const sf::Font& loadFont( Context& ctx, const std::string& file )
{
    auto& font = ctx.fonts.load( file, dataPath( file ) );
    font.setSmooth( false );   // keep the pixel fonts crisp
    return font;
}

sf::Text makeText( const sf::Font& font, const std::string& str, unsigned size, sf::Color fill, float outline )
{
    sf::Text text( font, str, size );
    text.setFillColor( fill );
    text.setOutlineColor( sf::Color( 30, 30, 50 ) );
    text.setOutlineThickness( outline );
    return text;
}

void centerOrigin( sf::Text& text )
{
    const sf::FloatRect b = text.getLocalBounds();
    text.setOrigin( { b.position.x + b.size.x / 2.f, b.position.y + b.size.y / 2.f } );
}

Hud::Hud( Context& ctx )
    : mContext( ctx )
    , mFont( loadFont( ctx, TextFont ) )
    , mCoin( ctx.textures.load( "coin", dataPath( "sprites/coin.png" ) ) )
    , mHeart( ctx.textures.load( "heart", dataPath( "sprites/heart.png" ) ) )
{
}

void Hud::draw( sf::RenderTarget& target, const Session& session, const std::string& levelName, float levelTime ) const
{
    const sf::Vector2f size = mContext.viewSize;

    // coins
    sf::Sprite coin( mCoin, sf::IntRect( { 0, 0 }, { int( mCoin.getSize().y ), int( mCoin.getSize().y ) } ) );
    coin.setPosition( { 20.f, 16.f } );
    target.draw( coin );
    char buf[32];
    std::snprintf( buf, sizeof( buf ), "x %02d", session.coins );
    sf::Text coins = makeText( mFont, buf, 48 );
    coins.setPosition( { 82.f, 10.f } );
    target.draw( coins );

    // lives as hearts
    sf::Sprite heart( mHeart );
    const int shown = std::min( session.lives, 5 );
    for ( int i = 0; i < shown; ++i ) {
        heart.setPosition( { 220.f + i * 58.f, 16.f } );
        target.draw( heart );
    }
    if ( session.lives > shown ) {
        sf::Text more = makeText( mFont, "+" + std::to_string( session.lives - shown ), 48 );
        more.setPosition( { 220.f + shown * 58.f, 10.f } );
        target.draw( more );
    }

    // score, right aligned
    std::snprintf( buf, sizeof( buf ), "SCORE %06d", session.score );
    sf::Text score = makeText( mFont, buf, 48 );
    score.setPosition( { size.x - score.getLocalBounds().size.x - 24.f, 10.f } );
    target.draw( score );

    // level name for the first seconds
    if ( levelTime < 3.f ) {
        sf::Text name = makeText( loadFont( mContext, TitleFont ), levelName, 56, sf::Color( 255, 230, 120 ), 5.f );
        centerOrigin( name );
        name.setPosition( { size.x / 2.f, size.y * 0.3f } );
        target.draw( name );
    }
}

} // namespace nomi
