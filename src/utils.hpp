#pragma once

#include <string>

#include <SFML/System/Vector2.hpp>

namespace nomi
{

float length(sf::Vector2f v );

// remember where the executable lives, so assets can be found next to it
void initDataPath( const char* argv0 );

// absolute path of a file below the share/ asset directory
std::string dataPath( const std::string& relative );

// global debug switch (draws hitboxes and collision geometry)
extern bool gDebugDraw;

}
