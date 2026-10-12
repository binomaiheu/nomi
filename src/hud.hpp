#pragma once

#include <string>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>

#include "state.hpp"

namespace nomi
{

// fonts in share/fonts
constexpr const char* TitleFont = "fonts/KenneyFuture.ttf";
constexpr const char* TextFont  = "fonts/KenneyPixel.ttf";

const sf::Font& loadFont( Context& ctx, const std::string& file );

// white text with a dark outline, readable on any background
sf::Text makeText( const sf::Font& font, const std::string& str, unsigned size,
                   sf::Color fill = sf::Color::White, float outline = 3.f );
void     centerOrigin( sf::Text& text );

// coins, lives, score and level name, drawn in screen coordinates
class Hud
{
public:
    explicit Hud( Context& ctx );
    void draw( sf::RenderTarget& target, const Session& session, const std::string& levelName, float levelTime ) const;

private:
    Context&           mContext;
    const sf::Font&    mFont;
    const sf::Texture& mCoin;
    const sf::Texture& mHeart;
};

} // namespace nomi
