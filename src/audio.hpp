#pragma once

#include <list>
#include <string>

#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/Sound.hpp>

#include "resourcemgr.hpp"

namespace nomi
{

// short sound effects from share/sounds/<name>.ogg
class SoundPlayer
{
public:
    void play( const std::string& name );
    void setMuted( bool muted ) { mMuted = muted; }
    bool isMuted() const        { return mMuted; }

private:
    SoundMgr             mBuffers;
    std::list<sf::Sound> mSounds;
    bool                 mMuted = false;
};

// streamed background music from share/music
class MusicPlayer
{
public:
    void play( const std::string& file, float volume = 40.f );  // keeps playing if already on
    void stop();
    void setMuted( bool muted );
    bool isMuted() const { return mMuted; }

private:
    sf::Music   mMusic;
    std::string mCurrent;
    float       mVolume = 40.f;
    bool        mMuted  = false;
};

} // namespace nomi
