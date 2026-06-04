#ifndef MALAISE_TILE_HPP
#define MALAISE_TILE_HPP

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "util.hpp"
#include "vec2i.hpp"

namespace malaise {

enum class TileType {
	FLOOR = 0,
	FLOOR_UNDER = 1,
	GOAL = 2,
	VOID = 3,
	GLASS = 4,
	BOUNDS = 5,

	WALL_CORNER_TOP_LEFT = 6,
	WALL_TOP = 7,
	WALL_CORNER_TOP_RIGHT = 8,

	WALL_SIDE_LEFT = 9,
	WALL_SIDE_RIGHT = 10,

	WALL_CORNER_BOTTOM_LEFT = 11,
	WALL_BOTTOM = 12,
	WALL_CORNER_BOTTOM_RIGHT = 13,

	COUNT
};

constexpr size_t TileTypeCount = static_cast<size_t>(TileType::COUNT);

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
