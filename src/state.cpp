#include "state.hpp"

namespace nomi
{

void StateStack::handleEvent( const sf::Event& ev )
{
    if ( !mStack.empty() ) mStack.back()->handleEvent( ev );
    applyPending();
}

void StateStack::update( sf::Time dt )
{
    if ( !mStack.empty() ) mStack.back()->update( dt );
    applyPending();
}

void StateStack::draw( sf::RenderTarget& target )
{
    // draw from the lowest state that is not covered by a full-screen state
    std::size_t first = mStack.size();
    while ( first > 0 ) {
        --first;
        if ( !mStack[ first ]->isOverlay() ) break;
    }
    for ( std::size_t i = first; i < mStack.size(); ++i ) mStack[ i ]->draw( target );
}

void StateStack::applyPending()
{
    // a pending change may queue more changes, so swap first
    while ( !mPending.empty() ) {
        auto pending = std::move( mPending );
        mPending.clear();
        for ( auto& change : pending ) change();
    }
}

} // namespace nomi
