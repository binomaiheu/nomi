#pragma once

#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>
#include <tmxlite/detail/Log.hpp>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/System/Time.hpp>

#include <memory>
#include <vector>
#include <array>
#include <map>
#include <string>
#include <limits>
#include <iostream>
#include <cmath>

namespace nomi
{

class TileMapLayer final : public sf::Drawable
{
public:

    TileMapLayer(const tmx::Map& map, std::size_t idx);    
    TileMapLayer(const TileMapLayer&) = delete;
    TileMapLayer& operator = (const TileMapLayer&) = delete;
    ~TileMapLayer() = default;

    const sf::FloatRect& getGlobalBounds() const { return m_globalBounds; }

    void setTile(int tileX, int tileY, tmx::TileLayer::Tile tile, bool refresh = true);

    tmx::TileLayer::Tile getTile(int tileX, int tileY);

    void setColor(int tileX, int tileY, sf::Color color, bool refresh = true);

    sf::Color getColor(int tileX, int tileY);

    void update(sf::Time elapsed);

private:
	//increasing m_chunkSize by 4; fixes render problems when mapsize != chunksize
	//sf::Vector2f m_chunkSize = sf::Vector2f(1024.f, 1024.f);
    sf::Vector2f  m_chunkSize = sf::Vector2f(512.f, 512.f);
    sf::Vector2u  m_chunkCount;
    sf::Vector2u  m_MapTileSize;   // general Tilesize of Map
    sf::FloatRect m_globalBounds;

    using TextureResource = std::map<std::string, std::unique_ptr<sf::Texture>>;
    TextureResource m_textureResource;

    struct AnimationState
    {
        sf::Vector2u       tileCords;
        sf::Time           startTime;
        tmx::Tileset::Tile animTile;
        std::uint8_t       flipFlags;
    };

    class Chunk final : public sf::Transformable, public sf::Drawable
    {
    public:
        using Ptr  = std::unique_ptr<Chunk>;
		using Tile = std::array<sf::Vertex, 4u>;

        Chunk( const tmx::TileLayer& layer, std::vector<const tmx::Tileset*> tilesets,
               const sf::Vector2f& position, const sf::Vector2f& tileCount, const sf::Vector2u& tileSize,
               std::size_t rowSize,  
               TextureResource& tr, const std::map<std::uint32_t, tmx::Tileset::Tile>& animTiles);
        ~Chunk() = default;
        Chunk(const Chunk&) = delete;
        Chunk& operator = (const Chunk&) = delete;
                    
        std::vector<AnimationState>& getActiveAnimations() { return m_activeAnimations; }

        bool      empty() const { return m_chunkArrays.empty(); }

        tmx::TileLayer::Tile getTile(int x, int y) const { return m_chunkTileIDs[calcIndexFrom(x,y)]; }

        int       calcIndexFrom(int x, int y) const { return x + y * chunkTileCount.x; }
    
        void      generateTiles(bool registerAnimation = false);

        void      setTile(int x, int y, tmx::TileLayer::Tile tile, bool refresh);
        
        sf::Color getColor(int x, int y) const;
        
        void      setColor(int x, int y, sf::Color color, bool refresh);
        
        void      maybeRegenerate(bool refresh);
                        
        void      flipY(sf::Vector2f *v0, sf::Vector2f *v1, sf::Vector2f *v2, sf::Vector2f *v3);
        void      flipX(sf::Vector2f *v0, sf::Vector2f *v1, sf::Vector2f *v2, sf::Vector2f *v3);
        void      flipD(sf::Vector2f *v0, sf::Vector2f *v1, sf::Vector2f *v2, sf::Vector2f *v3);

        void      doFlips(std::uint8_t bits, sf::Vector2f *v0, sf::Vector2f *v1, sf::Vector2f *v2, sf::Vector2f *v3);

    private:    

        class ChunkArray final : public sf::Drawable
        {
        public:
            using Ptr = std::unique_ptr<ChunkArray>;
            tmx::Vector2u  tileSetSize;
            sf::Vector2u   tsTileCount;
            std::uint32_t  m_firstGID, m_lastGID;

            explicit ChunkArray(const sf::Texture& t, const tmx::Tileset& ts);
            ~ChunkArray() = default;
            ChunkArray(const ChunkArray&) = delete;
            ChunkArray& operator = (const ChunkArray&) = delete;

            void reset() { m_vertices.clear(); }

            sf::Vector2u getTextureSize() const { return m_texture.getSize(); }

            void addTile(const Chunk::Tile& tile);

        private:
            const sf::Texture& m_texture;
            std::vector<sf::Vertex> m_vertices;

            void draw(sf::RenderTarget& rt, sf::RenderStates states) const override;
        };

        sf::Uint8 layerOpacity;     // opacity of the layer
        sf::Vector2f layerOffset;   // Layer offset
        sf::Vector2u mapTileSize;   // general Tilesize of Map
        sf::Vector2f chunkTileCount;   // chunk tilecount
        std::vector<tmx::TileLayer::Tile> m_chunkTileIDs; // stores all tiles in this chunk for later manipulation
        std::vector<sf::Color>            m_chunkColors; // stores colors for extended color effects
        std::map<std::uint32_t, tmx::Tileset::Tile> m_animTiles;    // animation catalogue
        std::vector<AnimationState>       m_activeAnimations;     // Animations to be done in this chunk
        std::vector<ChunkArray::Ptr>      m_chunkArrays;

        void draw(sf::RenderTarget& rt, sf::RenderStates states) const override;

    }; // sub  class Chunk definition

    std::vector<Chunk::Ptr>     m_chunks;
    
    mutable std::vector<Chunk*> m_visibleChunks;
    
    Chunk::Ptr& getChunkAndTransform(int x, int y, sf::Vector2u& chunkRelative);
    
    void createChunks(const tmx::Map& map, const tmx::TileLayer& layer);

    void updateVisibility(const sf::View& view) const;

    void draw(sf::RenderTarget& rt, sf::RenderStates states) const override;

};

} // namespace