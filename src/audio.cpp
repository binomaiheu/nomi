#include <iostream>

#include "audio.hpp"
#include "utils.hpp"

namespace nomi
{

void SoundPlayer::play( const std::string& name )
{
    mSounds.remove_if( []( const sf::Sound& s ) { return s.getStatus() == sf::Sound::Status::Stopped; } );
    if ( mMuted ) return;

    try {
        const auto& buffer = mBuffers.load( name, dataPath( "sounds/" + name + ".ogg" ) );
        mSounds.emplace_back( buffer ).play();
    } catch ( const std::exception& e ) {
        // a missing sound should never stop the game
        std::cerr << e.what() << "\n";
    }
}

void MusicPlayer::play( const std::string& file, float volume )
{
    mVolume = volume;
    if ( mCurrent == file && mMusic.getStatus() == sf::Music::Status::Playing ) return;

    mMusic.stop();
    mCurrent = file;
    if ( !mMusic.openFromFile( dataPath( "music/" + file ) ) ) {
        std::cerr << "[MusicPlayer] cannot open " << file << "\n";
        return;
    }
    mMusic.setLooping( true );
    mMusic.setVolume( mMuted ? 0.f : mVolume );
    mMusic.play();
}

void MusicPlayer::stop()
{
    mMusic.stop();
    mCurrent.clear();
}

void MusicPlayer::setMuted( bool muted )
{
    mMuted = muted;
    mMusic.setVolume( mMuted ? 0.f : mVolume );
}

} // namespace nomi
