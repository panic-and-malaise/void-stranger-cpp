#ifndef MALAISE_TILESET_HPP
#define MALAISE_TILESET_HPP

#include <array>
#include <unordered_map>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "tile_type.hpp"
#include "tile_definition.hpp"

namespace malaise::tile {

class TileSet {
public:
	void init_tile_definitions(const std::unordered_map<std::string, sf::Texture> &textures) {
		definitions[(size_t)TileType::BOUNDS] = {
			.texture = nullptr,
			.texture_rect = {0, 0, 16, 16},
			.is_collidable = true,
			.render_under_decoration = false
		};

		definitions[(size_t)TileType::VOID] = {
			.texture = nullptr,
			.texture_rect = {0, 0, 16, 16},
			.is_collidable = false,
			.is_fall = true,
			.render_under_decoration = false
		};

		definitions[(size_t)TileType::FLOOR] = {
			.texture = &textures.at("spr_floor_0"),
			.texture_rect = {0, 0, 16, 16},
			.is_collidable = false,
			.is_pickable = true
		};

		definitions[(size_t)TileType::FLOOR_UNDER] = {
			.texture = &textures.at("spr_floor_1"),
			.texture_rect = {0, 0, 16, 16},
			.is_collidable = false,
			.is_fall = true,
			.render_under_decoration = false
		};
		
		definitions[(size_t)TileType::GOAL] = {
			.texture = &textures.at("spr_stairs_0"),
			.texture_rect = {0, 0, 16, 16},
			.is_collidable = false,
			.is_stairs = true,
			.is_pickable = true
		};

		definitions[(size_t)TileType::GLASS] = {
			.texture = &textures.at("spr_glassfloor_0"),
			.texture_rect = {0, 0, 16, 16},
			.is_collidable = false,
			.is_pickable = true
		};

		definitions[(size_t)TileType::WALL_CORNER_TOP_LEFT] = {
			.texture = &textures.at("spr_tile_bg_1"),
			.texture_rect = {60, 0, 16, 16}, // 3 (16px (tile size) + 4px (border between tiles))
			.is_collidable = true
		};

		definitions[(size_t)TileType::WALL_TOP] = {
			.texture = &textures.at("spr_tile_bg_1"),
			.texture_rect = {80, 0, 16, 16},
			.is_collidable = true
		};

		definitions[(size_t)TileType::WALL_CORNER_TOP_RIGHT] = {
			.texture = &textures.at("spr_tile_bg_1"),
			.texture_rect = {100, 0, 16, 16},
			.is_collidable = true
		};

		definitions[(size_t)TileType::WALL_SIDE_LEFT] = {
			.texture = &textures.at("spr_tile_bg_1"),
			.texture_rect = {120, 0, 16, 16},
			.is_collidable = true
		};

		definitions[(size_t)TileType::WALL_SIDE_RIGHT] = {
			.texture = &textures.at("spr_tile_bg_1"),
			.texture_rect = {20, 20, 16, 16},
			.is_collidable = true
		};

		definitions[(size_t)TileType::WALL_CORNER_BOTTOM_LEFT] = {
			.texture = &textures.at("spr_tile_bg_1"),
			.texture_rect = {40, 20, 16, 16}, // 3 (16px (tile size) + 4px (border between tiles))
			.is_collidable = true
		};

		definitions[(size_t)TileType::WALL_BOTTOM] = {
			.texture = &textures.at("spr_tile_bg_1"),
			.texture_rect = {60, 20, 16, 16},
			.is_collidable = true
		};

		definitions[(size_t)TileType::WALL_CORNER_BOTTOM_RIGHT] = {
			.texture = &textures.at("spr_tile_bg_1"),
			.texture_rect = {80, 20, 16, 16},
			.is_collidable = true
		};

		definitions[(size_t)TileType::STATUE_ABADDON] = {
			.texture = &textures.at("spr_statue_abaddon_0"),
			.texture_rect = {0, 0, 96, 64},
			.is_collidable = true
		};

		definitions[(size_t)TileType::WALL_0] = {
			.texture = &textures.at("spr_tile_bg_1"),
			.texture_rect = {0, 40, 16, 16},
			.is_collidable = true
		};

		definitions[(size_t)TileType::WALL_1] = {
			.texture = &textures.at("spr_tile_bg_1"),
			.texture_rect = {40, 40, 16, 16},
			.is_collidable = true
		};

		definitions[(size_t)TileType::WALL_2] = {
			.texture = &textures.at("spr_tile_bg_1"),
			.texture_rect = {60, 40, 16, 16},
			.is_collidable = true
		};

		definitions[(size_t)TileType::WALL_3] = {
			.texture = &textures.at("spr_tile_bg_1"),
			.texture_rect = {80, 20, 16, 16},
			.is_flipped_vertical = true,
			.is_collidable = true,
		};

		definitions[(size_t)TileType::WALL_EDGE_0] = {
			.texture = &textures.at("spr_tile_edges"),
			.texture_rect = {40, 0, 16, 16},
			.is_collidable = true,
			.render_under_decoration = false
		};

		definitions[(size_t)TileType::WALL_EDGE_1] = {
			.texture = &textures.at("spr_tile_edges"),
			.texture_rect = {60, 0, 16, 16},
			.is_collidable = true,
			.render_under_decoration = false
		};

		definitions[(size_t)TileType::WALL_EDGE_2] = {
			.texture = &textures.at("spr_tile_edges"),
			.texture_rect = {80, 0, 16, 16},
			.is_collidable = true,
			.render_under_decoration = false
		};

		definitions[(size_t)TileType::WALL_EDGE_3] = {
			.texture = &textures.at("spr_tile_edges"),
			.texture_rect = {100, 0, 16, 16},
			.is_collidable = true,
			.render_under_decoration = false
		};

		definitions[(size_t)TileType::WALL_EDGE_4] = {
			.texture = &textures.at("spr_tile_edges"),
			.texture_rect = {120, 0, 16, 16},
			.is_collidable = true,
			.render_under_decoration = false
		};

		definitions[(size_t)TileType::WALL_EDGE_5] = {
			.texture = &textures.at("spr_tile_edges"),
			.texture_rect = {0, 20, 16, 16},
			.is_collidable = true,
			.render_under_decoration = false
		};

		definitions[(size_t)TileType::WALL_EDGE_6] = {
			.texture = &textures.at("spr_tile_edges"),
			.texture_rect = {20, 20, 16, 16},
			.is_collidable = true,
			.render_under_decoration = false
		};

		definitions[(size_t)TileType::WALL_EDGE_7] = {
			.texture = &textures.at("spr_tile_edges"),
			.texture_rect = {100, 20, 16, 16},
			.is_collidable = true,
			.render_under_decoration = false
		};

		definitions[(size_t)TileType::WALL_EDGE_8] = {
			.texture = &textures.at("spr_tile_edges"),
			.texture_rect = {120, 20, 16, 16},
			.is_collidable = true,
			.render_under_decoration = false
		};

		definitions[(size_t)TileType::WALL_EDGE_9] = {
			.texture = &textures.at("spr_tile_edges"),
			.texture_rect = {0, 40, 16, 16},
			.is_collidable = true,
			.render_under_decoration = false
		};

		definitions[(size_t)TileType::WALL_EDGE_10] = {
			.texture = &textures.at("spr_tile_edges"),
			.texture_rect = {20, 40, 16, 16},
			.is_collidable = true,
			.render_under_decoration = false
		};

		definitions[(size_t)TileType::WALL_EDGE_11] = {
			.texture = &textures.at("spr_tile_edges"),
			.texture_rect = {40, 40, 16, 16},
			.is_collidable = true,
			.render_under_decoration = false
		};

		definitions[(size_t)TileType::WALL_EDGE_12] = {
			.texture = &textures.at("spr_tile_edges"),
			.texture_rect = {60, 40, 16, 16},
			.is_collidable = true,
			.render_under_decoration = false
		};

		definitions[(size_t)TileType::WALL_EDGE_13] = {
			.texture = &textures.at("spr_tile_edges"),
			.texture_rect = {80, 40, 16, 16},
			.is_collidable = true,
			.render_under_decoration = false
		};

		definitions[(size_t)TileType::WALL_EDGE_14] = {
			.texture = &textures.at("spr_tile_edges"),
			.texture_rect = {100, 40, 16, 16},
			.is_collidable = true,
			.render_under_decoration = false
		};

		definitions[(size_t)TileType::BLANK_WHITE] = {
			.texture = &textures.at("spr_white"),
			.texture_rect = {0, 0, 16, 16},
		};

		definitions[(size_t)TileType::WALL_6] = {
			.texture = &textures.at("spr_tilemap_br2"),
			.texture_rect = {0, 0, 16, 16},
			.is_collidable = true,
		};

		definitions[(size_t)TileType::WALL_7] = {
			.texture = &textures.at("spr_tilemap_br2"),
			.texture_rect = {17, 0, 16, 16},
			.is_collidable = true,
		};

		definitions[(size_t)TileType::WALL_8] = {
			.texture = &textures.at("spr_tilemap_br2"),
			.texture_rect = {34, 0, 16, 16},
			.is_collidable = true,
		};

		definitions[(size_t)TileType::WALL_9] = {
			.texture = &textures.at("spr_tilemap_br2"),
			.texture_rect = {51, 0, 16, 16},
			.is_collidable = true,
		};

		definitions[(size_t)TileType::WALL_10] = {
			.texture = &textures.at("spr_tilemap_br2"),
			.texture_rect = {0, 17, 16, 16},
			.is_collidable = true,
		};

		definitions[(size_t)TileType::WALL_11] = {
			.texture = &textures.at("spr_tilemap_br2"),
			.texture_rect = {17, 17, 16, 16},
			.is_collidable = true,
		};

		definitions[(size_t)TileType::WALL_12] = {
			.texture = &textures.at("spr_tilemap_br2"),
			.texture_rect = {34, 17, 16, 16},
			.is_collidable = true,
		};

		definitions[(size_t)TileType::WALL_13] = {
			.texture = &textures.at("spr_tilemap_br2"),
			.texture_rect = {51, 17, 16, 16},
			.is_collidable = true,
		};

		definitions[(size_t)TileType::WALL_14] = {
			.texture = &textures.at("spr_tilemap_br2"),
			.texture_rect = {0, 34, 16, 16},
			.is_collidable = true,
		};

		definitions[(size_t)TileType::WALL_15] = {
			.texture = &textures.at("spr_tilemap_br2"),
			.texture_rect = {17, 34, 16, 16},
			.is_collidable = true,
		};

		definitions[(size_t)TileType::WALL_16] = {
			.texture = &textures.at("spr_tilemap_br2"),
			.texture_rect = {34, 34, 16, 16},
			.is_collidable = true,
		};

		definitions[(size_t)TileType::WALL_17] = {
			.texture = &textures.at("spr_tilemap_br2"),
			.texture_rect = {51, 34, 16, 16},
			.is_collidable = true,
		};

		definitions[(size_t)TileType::WALL_18] = {
			.texture = &textures.at("spr_tilemap_br2"),
			.texture_rect = {0, 51, 16, 16},
			.is_collidable = true,
		};

		definitions[(size_t)TileType::WALL_19] = {
			.texture = &textures.at("spr_tilemap_br2"),
			.texture_rect = {17, 51, 16, 16},
			.is_collidable = true,
		};

		definitions[(size_t)TileType::CHEST_CLOSE] = {
			.texture = &textures.at("spr_chest_regular_0"),
			.texture_rect = { 0, 0, 16, 16 },
			.is_collidable = true,
		};

		definitions[(size_t)TileType::CHEST_OPEN] = {
			.texture = &textures.at("spr_chest_regular_1"),
			.texture_rect = { 0, 0, 16, 16 },
			.is_collidable = true,
		};

		definitions[(size_t)TileType::EGG] = {
			.texture = &textures.at("spr_boulder_0"),
			.texture_rect = { 0, 0, 16, 16 },
			.is_collidable = true,
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
