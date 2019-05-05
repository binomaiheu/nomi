#pragma once

#include <memory>
#include <map>
#include <string>
#include <stdexcept>
#include <cassert>

namespace nomi
{

template <typename resource, typename identifier>
class ResourceMgr
{
public:
    void            load( identifier id, const std::string& fileName );
    resource&       get( identifier id );
    const resource& get( identifier id ) const;

private: 
    std::map<identifier, std::unique_ptr<resource>> mResourceMap;
    
private:
    void insert(identifier id, std::unique_ptr<resource> r);
};


// implementation
template <typename resource, typename identifier>
void ResourceMgr<resource,identifier>::load( identifier id, const std::string& fileName )
{
    // Create and load resource
	std::unique_ptr<resource> r(new resource());
	if (!r->loadFromFile(fileName))
		throw std::runtime_error("resourcemgr::load - failed to load " + fileName);

	// If loading successful, insert resource to map
    insert( id, std::move(r) );
    return;
}

template <typename resource, typename identifier>
resource& ResourceMgr<resource,identifier>::get( identifier id )
{
	auto found = mResourceMap.find(id);
	assert(found != mResourceMap.end());

	return *found->second;
}

template <typename resource, typename identifier>
const resource& ResourceMgr<resource,identifier>::get( identifier id ) const
{
	auto found = mResourceMap.find(id);
	assert(found != mResourceMap.end());

	return *found->second;
}

template <typename resource, typename identifier>
void ResourceMgr<resource, identifier>::insert(identifier id, std::unique_ptr<resource> r) 
{
	// Insert and check success
	auto inserted = mResourceMap.insert(std::make_pair(id, std::move(r)));
	assert(inserted.second);
}

} // namespace