#pragma once

#include <SFML/Graphics.hpp>

namespace nomi
{

class animation
{
public:
    animation( const sf::Texture& tx, sf::Vector2u imSize, sf::Time switchTime );
    virtual ~animation();


};


} // namespace