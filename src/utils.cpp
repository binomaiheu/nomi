#include <cmath>

#include "utils.hpp"

namespace nomi{

float length(sf::Vector2f v)
{
    return std::sqrt( v.x*v.x + v.y*v.y );
}

}