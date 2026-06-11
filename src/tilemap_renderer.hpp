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
#include "render_util.hpp"
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
		for (size_t y = 0; y < map.get_height(); y++) {
			for (size_t x = 0; x < map.get_width(); x++) {
				TileType type = map.get(x, y);
				const TileDefinition definition = set.definition_for(type);

				if (definition.render_under_decoration) {
					TileType below = map.get(x, y + 1);

					if (below == TileType::VOID) {
						util::render::draw_tile(
							target,
							TileType::FLOOR_UNDER,
							set,
							{
								static_cast<int32_t>(x),
								static_cast<int32_t>(y + 1)
							}
						);
					}
				}

				const sf::Texture *texture = set.texture(type);
				if (!texture) continue;

				sf::Sprite sprite(*texture, set.rect_for(type));

				sprite.setOrigin({0.f, 0.f});
				sprite.setScale({
					util::SPRITE_SCALE,
					util::SPRITE_SCALE
				});

				if (definition.is_flipped_horizontal) {
					sprite.setOrigin({
						static_cast<float>(definition.texture_rect.size.x),
						sprite.getOrigin().y
					});
					sprite.scale({-1.f, 1.f});
				}

				if (definition.is_flipped_vertical) {
					sprite.setOrigin({
						sprite.getOrigin().x,
						static_cast<float>(definition.texture_rect.size.y)
					});
					sprite.scale({1.f, -1.f});
				}

				math::Vec2i position = util::grid_pos_to_world(x, y);

				sprite.setPosition({
					static_cast<float>(position.x),
					static_cast<float>(position.y)
				});

				target.draw(sprite);
			}
		}

		for (const auto &entity : map.get_entities()) {
			entity->draw(target, set);
		}
	}

	sf::VertexArray vertices;
};

}

#endif // !MALAISE_TILEMAP_RENDERER_HPP

