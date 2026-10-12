#include "animation.hpp"

namespace nomi
{

Animation makeRowAnimation( sf::Vector2i frameSize, int row, const std::vector<int>& columns,
                            sf::Time frameTime, bool loop )
{
    Animation a;
    a.frameTime = frameTime;
    a.loop = loop;
    for ( int c : columns )
        a.frames.emplace_back( sf::Vector2i( c * frameSize.x, row * frameSize.y ), frameSize );
    return a;
}

void Animator::play( const Animation& anim, bool restart )
{
    if ( mAnim == &anim && !restart ) return;
    mAnim = &anim;
    mElapsed = sf::Time::Zero;
    mIndex = 0;
}

void Animator::update( sf::Time dt )
{
    if ( !mAnim || mAnim->frames.empty() ) return;
    mElapsed += dt;
    while ( mElapsed >= mAnim->frameTime )
    {
        mElapsed -= mAnim->frameTime;
        if ( mIndex + 1 < mAnim->frames.size() ) ++mIndex;
        else if ( mAnim->loop ) mIndex = 0;
        else break;
    }
}

sf::IntRect Animator::frame() const
{
    if ( !mAnim || mAnim->frames.empty() ) return {};
    return mAnim->frames[ mIndex ];
}

bool Animator::finished() const
{
    return mAnim && !mAnim->loop && mIndex + 1 >= mAnim->frames.size();
}

const CharacterAnimations& CharacterAnimations::get()
{
    static const CharacterAnimations anims = [] {
        const sf::Vector2i size( FrameWidth, FrameHeight );
        CharacterAnimations a;
        a.idle = makeRowAnimation( size, 0, { 0, 1, 2, 1 }, sf::seconds( 0.25f ) );
        a.walk = makeRowAnimation( size, 1, { 0, 1, 2, 1 }, sf::seconds( 0.10f ) );
        a.jump = makeRowAnimation( size, 2, { 0, 1 },       sf::seconds( 0.12f ), false );
        a.fall = makeRowAnimation( size, 2, { 2 },          sf::seconds( 0.12f ) );
        a.hurt = makeRowAnimation( size, 3, { 0, 1, 2 },    sf::seconds( 0.12f ) );
        a.dead = makeRowAnimation( size, 8, { 0, 1, 2 },    sf::seconds( 0.15f ) );
        return a;
    }();
    return anims;
}

} // namespace nomi
