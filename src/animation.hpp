#pragma once

#include <SFML/Graphics.hpp>

namespace nomi
{

class Animation
{
public:
    Animation( const sf::Texture& tx, sf::Vector2u imSize, sf::Time switchTime );
    virtual ~Animation();


};


} // namespace