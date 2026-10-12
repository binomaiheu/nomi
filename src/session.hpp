#pragma once

#include <string>
#include <vector>

namespace nomi
{

// a playable character : a sprite sheet in share/pixmaps
struct Character
{
    std::string name;
    std::string sheet;
};

const std::vector<Character>& characters();

// score, coins and lives for one play-through
struct Session
{
    static constexpr int StartLives   = 3;
    static constexpr int CoinsPerLife = 100;
    static constexpr int CoinScore    = 10;
    static constexpr int StompScore   = 100;
    static constexpr int GoalScore    = 1000;

    int         score     = 0;
    int         coins     = 0;
    int         lives     = StartLives;
    std::size_t character = 0;

    void reset( std::size_t characterIndex )
    {
        *this = Session();
        character = characterIndex;
    }

    // returns true when the coin gave an extra life
    bool addCoin()
    {
        score += CoinScore;
        if ( ++coins >= CoinsPerLife ) {
            coins -= CoinsPerLife;
            ++lives;
            return true;
        }
        return false;
    }

    void addStomp()      { score += StompScore; }
    void addGoal()       { score += GoalScore; }

    // returns true while there are lives left
    bool loseLife()      { if ( lives > 0 ) --lives; return lives > 0; }
};

} // namespace nomi
