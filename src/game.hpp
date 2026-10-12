#pragma once

#include <memory>
#include <string>

#include <SFML/Graphics/RenderWindow.hpp>

#include "world.hpp"

namespace nomi {

// main game class for the nomi game
class Game {
public:
    struct Options {
        std::string level;          // start directly in this level
        bool        debug    = false;
        bool        autotest = false;
    };

    explicit Game( const Options& opts );
    virtual ~Game();

    int run( void );

private:
    void processEvents( void );
    void update( sf::Time dt );
    void render( void );

private:
    Options                  mOptions;
    sf::Time                 mTimePerFrame;
    sf::RenderWindow         mWindow;
    std::unique_ptr<World>   mWorld;

    bool                     mIsPaused;
};

}
