#ifndef MALAISE_TILEMAP_RENDERER_HPP
#define MALAISE_TILEMAP_RENDERER_HPP

#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include "tile.hpp"
#include "tile_definition.hpp"
#include "tilemap.hpp"
#include "tileset.hpp"
#include "util.hpp"
#include "vec2i.hpp"

namespace malaise::tile {

class TileMapRenderer {
public:
	void rebuild(const TileMap &map, const TileSet &set) {
		return; //TODO: Implement
		for (auto tile : map.get_tiles()) {

		}
	}

	void draw(sf::RenderTarget &target, const TileMap &map, const TileSet &set) {
		// simple sprite based rendering version, try VertexArray later
		sf::Sprite sprite;

		for (size_t y = 0; y < map.get_height(); y++) {
			for (size_t x = 0; x < map.get_width(); x++) {
				TileType type = map.get(x, y);
				const TileDefinition definition = set.definition_for(type);

				if (definition.render_under_decoration) {
					TileType below = map.get(x, y + 1);

					if (below == TileType::VOID) {
						draw_tile(target, TileType::FLOOR_UNDER, set, {static_cast<int32_t>(x), static_cast<int32_t>(y + 1)});
					}
				}

				const sf::Texture *texture = set.texture(type);
				if (!texture) continue;

				sprite.setTexture(*texture);
				sprite.setTextureRect(set.rect_for(type));

				sprite.setOrigin(0, 0);
				sprite.setScale(util::SPRITE_SCALE, util::SPRITE_SCALE);

				if (definition.is_flipped_horizontal) {
					sprite.setOrigin(
						definition.texture_rect.width,
						sprite.getOrigin().y
					);
					sprite.scale(-1, 1);
				}

				if (definition.is_flipped_vertical) {
					sprite.setOrigin(
						sprite.getOrigin().x,
						definition.texture_rect.height
					);
					sprite.scale(1, -1);
				}

				math::Vec2i position = util::grid_pos_to_world(x, y);
				sprite.setPosition(
					position.x,
					position.y
				);

				target.draw(sprite);
			}
		}
	}
private:
	void draw_tile(sf::RenderTarget &target, const TileType type, const TileSet &set, const math::Vec2i position_grid) {
		sf::Sprite sprite;

		const TileDefinition definition = set.definition_for(type);

		const sf::Texture *texture = set.texture(type);
		if (!texture) return;

		sprite.setTexture(*texture);
		sprite.setTextureRect(set.rect_for(type));

		sprite.setOrigin(0, 0);
		sprite.setScale(util::SPRITE_SCALE, util::SPRITE_SCALE);

		if (definition.is_flipped_horizontal) {
			sprite.setOrigin(
				definition.texture_rect.width,
				sprite.getOrigin().y
			);
			sprite.scale(-1, 1);
		}

		if (definition.is_flipped_vertical) {
			sprite.setOrigin(
				sprite.getOrigin().x,
				definition.texture_rect.height
			);
			sprite.scale(1, -1);
		}

		math::Vec2i position = util::grid_pos_to_world(position_grid);
		sprite.setPosition(
			position.x,
			position.y
		);

		target.draw(sprite);
	}

	sf::VertexArray vertices;
};
}

#endif // !MALAISE_TILEMAP_RENDERER_HPP

