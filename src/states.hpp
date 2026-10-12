#pragma once

#include <memory>
#include <string>

#include "hud.hpp"
#include "state.hpp"
#include "world.hpp"

namespace nomi
{

// read keyboard and gamepad
Player::Input readInput();

// title screen with character select
class TitleState : public State
{
public:
    TitleState( StateStack& stack, Context& ctx );

    void handleEvent( const sf::Event& ev ) override;
    void update( sf::Time dt ) override;
    void draw( sf::RenderTarget& target ) override;

private:
    std::size_t mSelected;
    float       mTime = 0.f;
    Animator    mIdle, mWalk;
};

// playing one level
class PlayingState : public State
{
public:
    PlayingState( StateStack& stack, Context& ctx, std::string levelFile );

    void handleEvent( const sf::Event& ev ) override;
    void update( sf::Time dt ) override;
    void draw( sf::RenderTarget& target ) override;

private:
    void handleWorldEvents();

    std::string            mLevelFile;
    std::unique_ptr<World> mWorld;
    Hud                    mHud;
    float                  mTime = 0.f;
    float                  mCompleteTimer = -1.f;   // counts down after reaching the flag
};

// a message drawn over the game : pause, level complete, game over, victory
class MessageState : public State
{
public:
    enum class Kind { Paused, LevelComplete, GameOver, Victory };

    MessageState( StateStack& stack, Context& ctx, Kind kind, std::string nextLevel = {} );

    void handleEvent( const sf::Event& ev ) override;
    void update( sf::Time dt ) override;
    void draw( sf::RenderTarget& target ) override;
    bool isOverlay() const override { return true; }

private:
    void proceed();

    Kind        mKind;
    std::string mNextLevel;
    float       mTime = 0.f;
};

} // namespace nomi
