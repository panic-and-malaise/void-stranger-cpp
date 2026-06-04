#ifndef MALAISE_TILE_DEFINITION_HPP
#define MALAISE_TILE_DEFINITION_HPP

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace malaise::tile {

struct TileDefinition {
	const sf::Texture *texture = nullptr;
	sf::IntRect texture_rect;
	bool is_collidable = false;
	bool is_fall = false;
	bool is_stairs = false;
};

}

#endif // !MALAISE_TILE_DEFINITION_HPP
