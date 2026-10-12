#pragma once

#include <vector>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Time.hpp>

namespace nomi
{

// a sequence of frames (texture rectangles) on a sprite sheet
struct Animation
{
    std::vector<sf::IntRect> frames;
    sf::Time                 frameTime = sf::seconds( 0.1f );
    bool                     loop      = true;
};

// frames taken from one row of a sheet with fixed-size cells, in the given column order
Animation makeRowAnimation( sf::Vector2i frameSize, int row, const std::vector<int>& columns,
                            sf::Time frameTime, bool loop = true );

// plays an animation, call update every tick and use frame() as the sprite texture rect
class Animator
{
public:
    void play( const Animation& anim, bool restart = false );
    void update( sf::Time dt );

    sf::IntRect frame() const;
    bool        finished() const;
    const Animation* current() const { return mAnim; }

private:
    const Animation* mAnim    = nullptr;
    sf::Time         mElapsed = sf::Time::Zero;
    std::size_t      mIndex   = 0;
};

// The character sheets in share/pixmaps all share the same layout : 3 columns x 9 rows of 56x80 frames.
// row 0 idle, 1 walk, 2 jump/fall, 3 hurt, 4-7 punch/kick (unused), 8 dizzy (dead)
struct CharacterAnimations
{
    Animation idle, walk, jump, fall, hurt, dead;

    static const CharacterAnimations& get();
    static constexpr int FrameWidth  = 56;
    static constexpr int FrameHeight = 80;
};

} // namespace
