#ifndef MALAISE_SPRITE_ANIMATION_HPP
#define MALAISE_SPRITE_ANIMATION_HPP

#include <vector>
#include <string>
#include <unordered_map>

#include <SFML/Graphics/Sprite.hpp>

namespace malaise::animation {

struct SpriteAnimation {
	std::string base_name;
	std::vector<sf::Sprite*> frames;

	size_t frame_count = 0;
	float frame_length = 0.f;

	bool loops = false;
	bool uses_global_time = false;

	void load_frames_by_name(std::unordered_map<std::string, sf::Sprite> &sprites) {
		for (size_t i = 0; i < frame_count; i++) {
			auto frame_name = base_name + "_" + std::to_string(i);
			frames.push_back(&sprites.at(frame_name));
		}
	}
};

}

#endif // !MALAISE_SPRITE_ANIMATION_HPP
