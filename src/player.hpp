#ifndef MALAISE_PLAYER_HPP
#define MALAISE_PLAYER_HPP

#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "util.hpp"
#include "vec2i.hpp"

namespace malaise {

struct SpriteAnimation {
	std::string base_name;
	size_t frame_count = 0;
	float frame_length = 0.f;
	bool loops = false;
};

class Player {
public:
  Player() = default;

  Player(const std::unordered_map<std::string, sf::Sprite> &sprites_) {
	  load_sprites(sprites_);
  }
	
  void load_sprites(const std::unordered_map<std::string, sf::Sprite> &sprites_) {

	  for (auto& [name, sprite] : sprites_) {
		  if (name.find("spr_player") != std::string::npos) {
			  sprites.emplace(name, sprite);
		  }
	  }

	  set_sprite("spr_player_down");
  }

	void draw(sf::RenderTarget &target) {
		if (sprite) {
			sprite->setPosition(position.x, position.y);
			target.draw(*sprite);
		}
	}

	math::Vec2i get_position() const { return position; }
	
	math::Vec2i get_position_grid() const {
		math::Vec2i grid(
			position.x / util::TILE_SIZE / util::SPRITE_SCALE,
			position.y / util::TILE_SIZE / util::SPRITE_SCALE
		);
		return grid;
	}

	void set_position(const math::Vec2i pos) {
		position = pos;
	}

	void set_sprite(const std::string &name) {
		current_sprite_name = name;
		update_sprite();
	}

	void update_sprite() {
		std::stringstream next_frame{};
		next_frame << trunc_sprite_name() << "_" << animation_step;

		sprite = &sprites[next_frame.str()];
	}

	void move(const math::Vec2i pos) {
		math::Vec2i target = position + pos;

		if (pos.x > 0) {
			set_sprite("spr_player_right");
		} else if (pos.x < 0) {
			set_sprite("spr_player_left");
		} else if (pos.y > 0) {
			set_sprite("spr_player_down");
		} else {
			set_sprite("spr_player_up");
		}

		set_position(target);
	}

	void tick_animation(const float delta) {
		tick_counter += delta;

		if (tick_counter >= FRAME_LENGTH) {
			tick_counter = 0.0f;

			animation_step++;
			if (animation_step >= animation_length)
				animation_step = 0;

			update_sprite();
			// std::cout << "RUNNING!" << animation_step << "\n";
		}
	}
private:
	math::Vec2i position{};

	sf::Sprite *sprite = nullptr;
	std::string current_sprite_name = "";
	std::unordered_map<std::string, sf::Sprite> sprites;

	size_t animation_step = 0;
	size_t animation_length = 2;
	float tick_counter = 0.0f;
	static constexpr float FRAME_LENGTH = 60.f / 84.f; // synced to BPM for funsies

	const SpriteAnimation *current_animation = nullptr;
	const SpriteAnimation *default_animation = nullptr;

	size_t current_frame = 0;
	float animation_timer = 0.f;

	inline std::string trunc_sprite_name() {
		return current_sprite_name.substr(0, current_sprite_name.length());
	}
};

}

#endif // !MALAISE_PLAYER_HPP
