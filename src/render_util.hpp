#ifndef MALAISE_RENDER_UTIL_HPP
#define MALAISE_RENDER_UTIL_HPP

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "tile_type.hpp"
#include "tileset.hpp"
#include "tile_definition.hpp"
#include "util.hpp"
#include "vec2i.hpp"

namespace malaise::util::render {

inline void draw_tile(sf::RenderTarget &target, const tile::TileType type, const tile::TileSet &set, const math::Vec2i position_grid) {
	const tile::TileDefinition definition = set.definition_for(type);

	const sf::Texture *texture = set.texture(type);
	if (!texture) return;

	sf::Sprite sprite(*texture);
	sprite.setTextureRect(set.rect_for(type));

	sprite.setOrigin({ 0, 0 });
	sprite.setScale({ util::SPRITE_SCALE, util::SPRITE_SCALE });

	if (definition.is_flipped_horizontal) {
		sprite.setOrigin({
			static_cast<float>(definition.texture_rect.size.x),
			sprite.getOrigin().y
		});
		sprite.scale({-1, 1});
	}

	if (definition.is_flipped_vertical) {
		sprite.setOrigin({
			sprite.getOrigin().x,
			static_cast<float>(definition.texture_rect.size.y)
		});
		sprite.scale({1, -1});
	}

	math::Vec2i position = util::grid_pos_to_world(position_grid);
	sprite.setPosition({
		static_cast<float>(position.x),
		static_cast<float>(position.y)
	});

	target.draw(sprite);
}

}

#endif // !MALAISE_RENDER_UTIL_HPP
