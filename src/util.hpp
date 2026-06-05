#ifndef MALAISE_UTIL_HPP
#define MALAISE_UTIL_HPP

#include <filesystem>
#include <string>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/System/Vector2.hpp>

#include "vec2i.hpp"

#ifndef NDEBUG // Automagic define that the compiler sets based on the release mode
#define DEBUG_PRINT(msg) std::cout << msg << "\n"
#else
#define DEBUG_PRINT(msg) ;
#endif

namespace malaise::util {

const std::string RESOURCE_DIRECTORY = std::filesystem::current_path().string() + "/resources/";
const std::string SPRITE_DIRECTORY = RESOURCE_DIRECTORY + "sprites/";

const std::string MUSIC_DIRECTORY = RESOURCE_DIRECTORY + "music/";
const std::string SOUND_DIRECTORY = RESOURCE_DIRECTORY + "sounds/";

const std::string LEVEL_DIRECTORY = RESOURCE_DIRECTORY + "levels/";

const std::string FONT_DIRECTORY = RESOURCE_DIRECTORY + "fonts/";

static constexpr size_t WINDOW_WIDTH = 672;
static constexpr size_t WINDOW_HEIGHT = 432;

static constexpr size_t SPRITE_SCALE = 3;
static constexpr size_t TILE_SIZE = 16;

inline constexpr float PI = 3.14159265358979323846;

inline sf::Vector2i float_vector_to_integer(const sf::Vector2f vf) {
	return { static_cast<int>(vf.x), static_cast<int>(vf.y) };
}

inline sf::Vector2f integer_vector_to_float(const sf::Vector2i vi) {
	return { static_cast<float>(vi.x), static_cast<float>(vi.y) };
}

inline void center_element(sf::Transformable &element, const sf::FloatRect &bounds) {
	element.setOrigin(bounds.width / 2, bounds.height / 2);
}

inline math::Vec2i grid_pos_to_world(const size_t x, const size_t y) {
	return {
		static_cast<int32_t>(x * util::TILE_SIZE * util::SPRITE_SCALE),
		static_cast<int32_t>(y * util::TILE_SIZE * util::SPRITE_SCALE)
	};
}

inline math::Vec2i grid_pos_to_world(const math::Vec2i grid) {
	return {
		static_cast<int32_t>(grid.x * util::TILE_SIZE * util::SPRITE_SCALE),
		static_cast<int32_t>(grid.y * util::TILE_SIZE * util::SPRITE_SCALE)
	};
}

inline math::Vec2i world_pos_to_grid(const math::Vec2i world) {
	return {
		static_cast<int32_t>(world.x / util::TILE_SIZE / util::SPRITE_SCALE),
		static_cast<int32_t>(world.y / util::TILE_SIZE / util::SPRITE_SCALE)
	};
}

};

#endif // !MALAISE_UTIL_HPP
