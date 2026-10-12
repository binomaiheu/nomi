#pragma once

#include <memory>
#include <map>
#include <string>
#include <stdexcept>

#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace nomi
{

// SFML resources load with loadFromFile, except fonts which are streamed with openFromFile
template <typename resource>
bool loadResource( resource& r, const std::string& fileName ) { return r.loadFromFile( fileName ); }
inline bool loadResource( sf::Font& r, const std::string& fileName ) { return r.openFromFile( fileName ); }

template <typename resource, typename identifier>
class ResourceMgr
{
public:
    // load a resource, does nothing if the id is already loaded
    resource&       load( identifier id, const std::string& fileName );
    resource&       get( identifier id );
    const resource& get( identifier id ) const;
    bool            has( identifier id ) const { return mResourceMap.count( id ) > 0; }

private:
    std::map<identifier, std::unique_ptr<resource>> mResourceMap;
};


// implementation
template <typename resource, typename identifier>
resource& ResourceMgr<resource,identifier>::load( identifier id, const std::string& fileName )
{
    auto found = mResourceMap.find( id );
    if ( found != mResourceMap.end() ) return *found->second;

    // Create and load resource
	auto r = std::make_unique<resource>();
	if ( !loadResource( *r, fileName ) )
		throw std::runtime_error("resourcemgr::load - failed to load " + fileName);

	// If loading successful, insert resource to map
    return *mResourceMap.emplace( id, std::move( r ) ).first->second;
}

template <typename resource, typename identifier>
resource& ResourceMgr<resource,identifier>::get( identifier id )
{
	auto found = mResourceMap.find(id);
	if ( found == mResourceMap.end() )
        throw std::runtime_error( "resourcemgr::get - resource not loaded" );
	return *found->second;
}

template <typename resource, typename identifier>
const resource& ResourceMgr<resource,identifier>::get( identifier id ) const
{
	auto found = mResourceMap.find(id);
	if ( found == mResourceMap.end() )
        throw std::runtime_error( "resourcemgr::get - resource not loaded" );
	return *found->second;
}

using TextureMgr = ResourceMgr<sf::Texture, std::string>;
using FontMgr    = ResourceMgr<sf::Font, std::string>;
using SoundMgr   = ResourceMgr<sf::SoundBuffer, std::string>;

} // namespace
