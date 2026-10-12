#pragma once

#include <memory>
#include <string>

#include <SFML/Graphics/RenderWindow.hpp>

#include "state.hpp"

namespace nomi {

// main game class for the nomi game : window, main loop and the state stack
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
    void render( void );
    void updateViewport( void );

private:
    Options                  mOptions;
    sf::Time                 mTimePerFrame;
    sf::RenderWindow         mWindow;
    Context                  mContext;
    StateStack               mStates;

    bool                     mIsPaused;
};

// headless scripted playthrough of a level, returns the process exit code
int runAutotest( const std::string& levelFile );

}
