#ifndef MALAISE_TILESET_HPP
#define MALAISE_TILESET_HPP

#include <array>
#include <unordered_map>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "tile.hpp"
#include "tile_definition.hpp"

namespace malaise::tile {

class TileSet {
public:
	void init_tile_definitions(const std::unordered_map<std::string, sf::Texture> &textures) {
		definitions[(size_t)TileType::BOUNDS] = {
			nullptr,
			{0, 0, 16, 16},
			true
		};

		definitions[(size_t)TileType::FLOOR] = {
			&textures.at("spr_floor_0"),
			{0, 0, 16, 16},
			false
		};

		definitions[(size_t)TileType::FLOOR_UNDER] = {
			&textures.at("spr_floor_1"),
			{0, 0, 16, 16},
			false,
			true
		};
		
		definitions[(size_t)TileType::GOAL] = {
			&textures.at("spr_stairs_0"),
			{0, 0, 16, 16},
			false,
			false,
			true
		};

		definitions[(size_t)TileType::GLASS] = {
			&textures.at("spr_glassfloor_0"),
			{0, 0, 16, 16},
			false
		};

		definitions[(size_t)TileType::WALL_CORNER_TOP_LEFT] = {
			&textures.at("spr_tile_bg_1"),
			{60, 0, 16, 16}, // 3 (16px (tile size) + 4px (border between tiles))
			true
		};

		definitions[(size_t)TileType::WALL_TOP] = {
			&textures.at("spr_tile_bg_1"),
			{80, 0, 16, 16},
			true
		};

		definitions[(size_t)TileType::WALL_CORNER_TOP_RIGHT] = {
			&textures.at("spr_tile_bg_1"),
			{100, 0, 16, 16},
			true
		};
	}

	const TileDefinition definition_for(const TileType type) const {
		return definitions[static_cast<size_t>(type)];
	}

	const sf::Texture* texture(const TileType type) const {
		return definitions[static_cast<size_t>(type)].texture;
	}

	sf::IntRect rect_for(const TileType type) const {
		return definitions[static_cast<size_t>(type)].texture_rect;
	}
private:
	sf::Texture atlas;
	std::array<TileDefinition, TileTypeCount> definitions;
};

}

#endif // !MALAISE_TILESET_HPP
