#ifndef MALAISE_TILE_HPP
#define MALAISE_TILE_HPP

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "util.hpp"
#include "vec2i.hpp"

namespace malaise {

enum class TileType {
	FLOOR = 0,
	WALL,
	VOID,
};

class Tile {
public:
	Tile(const math::Vec2i position_, const sf::Sprite &sprite_, TileType type_ = TileType::FLOOR)
		: position(position_), sprite(sprite_), type(type_) {
			sprite.setPosition(
				position.x * util::TILE_SIZE * util::SPRITE_SCALE,
				position.y * util::TILE_SIZE * util::SPRITE_SCALE
			);
		}

	void draw(sf::RenderTarget &target) {
		target.draw(sprite);
	}

	TileType get_type() const {
		return type;
	}

	math::Vec2i get_position() const {
		return position;
	}
private:
	math::Vec2i position;
	sf::Sprite sprite;
	TileType type = TileType::FLOOR;
};

}

#endif // !MALAISE_TILE_HPP
