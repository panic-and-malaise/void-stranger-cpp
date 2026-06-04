#ifndef MALAISE_PLAYER_HPP
#define MALAISE_PLAYER_HPP

#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "tile.hpp"
#include "tileset.hpp"
#include "util.hpp"
#include "vec2i.hpp"

namespace malaise {

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
			// std::cout << frame_name << '\n';
			frames.push_back(&sprites.at(frame_name));
		}
	}
};

enum class Facing {
	UP = 0,
	DOWN,
	LEFT,
	RIGHT
};

class Player {
public:
	Player() = default;

	Player(const math::Vec2i pos) : position(std::move(pos)) {}

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


	void init_animations() {
		static constexpr float BPM = 60.f / 84.f;

		std::cout << "Loading player animations...\n";

		SpriteAnimation idle_up;

		idle_up.base_name = "spr_player_up";
		idle_up.frame_count = 2;
		idle_up.frame_length = BPM;
		idle_up.loops = true;
		idle_up.uses_global_time = true;
		idle_up.load_frames_by_name(sprites);

		SpriteAnimation idle_down = idle_up;
		idle_down.frames.clear();

		idle_down.base_name = "spr_player_down";
		idle_down.load_frames_by_name(sprites);

		SpriteAnimation idle_right = idle_up;
		idle_right.frames.clear();

		idle_right.base_name = "spr_player_right";
		idle_right.load_frames_by_name(sprites);

		SpriteAnimation idle_left = idle_up;
		idle_left.frames.clear();

		idle_left.base_name = "spr_player_left";
		idle_left.load_frames_by_name(sprites);

		animations.emplace("idle_up", std::move(idle_up));
		animations.emplace("idle_down", std::move(idle_down));
		animations.emplace("idle_left", std::move(idle_left));
		animations.emplace("idle_right", std::move(idle_right));

		default_animation = &animations.at("idle_down");

		SpriteAnimation fall;

		fall.base_name = "spr_player_fall";
		fall.frame_count = 6;
		fall.frame_length = 0.3;
		fall.loops = false;
		fall.load_frames_by_name(sprites);

		animations.emplace("fall", std::move(fall));

		const float PUSH_LENGTH = 0.12;

		SpriteAnimation push_up;

		push_up.base_name = "spr_player_attack_u";
		push_up.frame_count = 2;
		push_up.frame_length = PUSH_LENGTH;
		push_up.load_frames_by_name(sprites);

		SpriteAnimation push_down = push_up;
		push_down.frames.clear();

		push_down.base_name = "spr_player_attack_d";
		push_down.load_frames_by_name(sprites);

		SpriteAnimation push_right = push_up;
		push_right.frames.clear();

		push_right.base_name = "spr_player_attack_r";
		push_right.load_frames_by_name(sprites);

		SpriteAnimation push_left = push_up;
		push_left.frames.clear();

		push_left.base_name = "spr_player_attack_l";
		push_left.load_frames_by_name(sprites);

		animations.emplace("push_up", std::move(push_up));
		animations.emplace("push_down", std::move(push_down));
		animations.emplace("push_left", std::move(push_left));
		animations.emplace("push_right", std::move(push_right));

		play_animation("idle_down");
	}

	void init_sounds(void) {
		for (const auto &directory : std::filesystem::recursive_directory_iterator(util::SOUND_DIRECTORY)) {
			std::string path = directory.path().string();
			std::string filename = std::filesystem::path(directory).filename().string();

			if (filename.empty() or filename.substr(0, 3) != "snd") continue;

			std::string sound_name = std::filesystem::path(directory).filename().replace_extension("").string();
			sf::SoundBuffer buffer;

			if (buffer.loadFromFile(path)) {
				sound_buffers.emplace(sound_name, buffer);
				sf::Sound sound;
				sound.setVolume(50.f);
				sound.setBuffer(sound_buffers[sound_name]);
				sounds.emplace(sound_name, sound);
			}
		}
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

	void move(const math::Vec2i pos, tile::TileDefinition target_tile) {
		math::Vec2i target = position + pos;

		if (pos.x > 0)
			facing = Facing::RIGHT;
		else if (pos.x < 0)
			facing = Facing::LEFT;
		else if (pos.y > 0)
			facing = Facing::DOWN;
		else
			facing = Facing::UP;

		switch (facing) {
			case Facing::RIGHT:
				play_animation("idle_right");
				break;
			case Facing::LEFT:
				play_animation("idle_left");
				break;
			case Facing::DOWN:
				play_animation("idle_down");
				break;
			case Facing::UP:
				play_animation("idle_up");
				break;
			default:
				break;
		}

		// Can't move, push against wall
		if (target_tile.is_collidable) {
			sounds["snd_push_small"].play();

			default_animation = current_animation;

			switch (facing) {
				case Facing::RIGHT:
					play_animation("push_right");
					break;
				case Facing::LEFT:
					play_animation("push_left");
					break;
				case Facing::DOWN:
					play_animation("push_down");
					break;
				case Facing::UP:
					play_animation("push_up");
					break;
				default:
				break;
			}
			return;
		}

		if (target_tile.is_fall)
			play_animation("fall");

		set_position(target);
	}

	void tick_animation(const float delta) {
		if (!current_animation) return;

		animation_timer += delta;

		if (!current_animation->uses_global_time) {
			if (animation_timer < current_animation->frame_length)
				return;

			animation_timer = 0.f;
			current_frame++;

			if (current_frame >= current_animation->frames.size()) {
				current_frame = 0;
				if (!current_animation->loops)
					current_animation = default_animation;
			}
		} else {
			float beat_duration = current_animation->frame_length;

			current_frame =
				static_cast<size_t>(animation_timer / beat_duration)
				% current_animation->frames.size();
		}

		sprite = current_animation->frames[current_frame];
	}

	void play_animation(const std::string &name) {
		auto it = animations.find(name);

		if (it == animations.end())
			return;

		if (&it->second == current_animation)
			return;

		current_animation = &it->second;

		current_frame = 0;
		animation_timer = 0.f;

		sprite = current_animation->frames[0];
	}
private:
	math::Vec2i position{};
	Facing facing = Facing::DOWN;

	sf::Sprite *sprite = nullptr;
	std::string current_sprite_name = "";

	std::unordered_map<std::string, sf::Sprite> sprites;
	std::unordered_map<std::string, sf::SoundBuffer> sound_buffers;
	std::unordered_map<std::string, sf::Sound> sounds;

	size_t animation_step = 0;
	size_t animation_length = 2;
	float tick_counter = 0.0f;
	static constexpr float FRAME_LENGTH = 60.f / 84.f; // synced to BPM for funsies

	std::unordered_map<std::string, SpriteAnimation> animations;

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
