#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Time.hpp>

#include "audio.hpp"
#include "resourcemgr.hpp"
#include "session.hpp"

namespace nomi
{

class StateStack;

// everything the game states share
struct Context
{
    sf::RenderWindow* window = nullptr;
    sf::Vector2f      viewSize { 1280.f, 800.f };   // logical screen size, letterboxed into the window
    TextureMgr        textures;
    FontMgr           fonts;
    SoundPlayer       sounds;
    MusicPlayer       music;
    Session           session;
    std::string       firstLevel;
};

class State
{
public:
    State( StateStack& stack, Context& context ) : mStack( stack ), mContext( context ) {}
    virtual ~State() = default;

    virtual void handleEvent( const sf::Event& ev ) = 0;
    virtual void update( sf::Time dt ) = 0;
    virtual void draw( sf::RenderTarget& target ) = 0;

    // states below an overlay keep being drawn but are not updated
    virtual bool isOverlay() const { return false; }

protected:
    StateStack& stack()     { return mStack; }
    Context&    context()   { return mContext; }

private:
    StateStack& mStack;
    Context&    mContext;
};

// stack of game states; changes are queued and applied between frames
class StateStack
{
public:
    explicit StateStack( Context& context ) : mContext( context ) {}

    template <typename T, typename... Args>
    void push( Args&&... args )
    {
        mPending.push_back( [this, args...]() mutable {
            mStack.push_back( std::make_unique<T>( *this, mContext, std::move( args )... ) );
        } );
    }
    void pop()   { mPending.push_back( [this] { if ( !mStack.empty() ) mStack.pop_back(); } ); }
    void clear() { mPending.push_back( [this] { mStack.clear(); } ); }

    void handleEvent( const sf::Event& ev );
    void update( sf::Time dt );
    void draw( sf::RenderTarget& target );

    bool empty() const { return mStack.empty(); }
    void applyPending();

private:
    Context&                                 mContext;
    std::vector<std::unique_ptr<State>>      mStack;
    std::vector<std::function<void()>>       mPending;
};

} // namespace nomi
