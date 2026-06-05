#ifndef MALAISE_PLAYER_HPP
#define MALAISE_PLAYER_HPP

#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

#include "tile.hpp"
#include "tile_definition.hpp"
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
			frames.push_back(&sprites.at(frame_name));
		}
	}
};

struct AnimationInstance {
	const SpriteAnimation *animation = nullptr;

	size_t current_frame = 0;
	float timer = 0.f;
	float rotation = 0.f;

	bool finished = false;
	math::Vec2i offset{};
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
			if (name.find("spr_player")  != std::string::npos or
				name.find("spr_items")   != std::string::npos or
				name.find("spr_sparkle") != std::string::npos or
				name.find("spr_voidrod") != std::string::npos or
				name.find("spr_void_rod") != std::string::npos
				) {
				sprites.emplace(name, sprite);
			}
		}

		set_sprite("spr_player_down");
	}


	void init_animations() {
		static constexpr float BPM = 60.f / 84.f;

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

		SpriteAnimation enter;

		enter.base_name = "spr_player_enter";
		enter.frame_count = 6;
		enter.frame_length = PUSH_LENGTH;
		enter.load_frames_by_name(sprites);

		SpriteAnimation drop;

		drop.base_name = "spr_player_drop";
		drop.frame_count = 1;
		drop.frame_length = PUSH_LENGTH;
		drop.loops = true;
		drop.frames.push_back(&sprites.at("spr_player_drop_1"));

		animations.emplace("enter", enter);
		animations.emplace("drop", drop);

		SpriteAnimation blink;

		blink.base_name = "spr_player_gblink_d";
		blink.frame_count = 4;
		blink.frame_length = 0.1f;
		blink.loops = true;
		blink.load_frames_by_name(sprites);

		animations.emplace("blink", blink);

		static constexpr float VOID_ROD_LENGTH = 12.f / 60.f;

		SpriteAnimation sparkle = {
			.base_name = "spr_sparkle",
			.frame_count = 8,
			.frame_length = VOID_ROD_LENGTH / 8,
			.loops = false
		};
		sparkle.load_frames_by_name(sprites);

		animations.emplace("sparkle", sparkle);

		SpriteAnimation swipe = {
			.base_name = "spr_player_swipe",
			.frame_count = 10,
			.frame_length = VOID_ROD_LENGTH / 10.f,
			.loops = false
		};
		swipe.load_frames_by_name(sprites);

		animations.emplace("swipe", swipe);

		SpriteAnimation void_rod_r = {
			.base_name = "spr_void_rod_0",
			.frame_count = 1,
			.frame_length = VOID_ROD_LENGTH,
			.loops = false
		};
		void_rod_r.frames.push_back(&sprites.at("spr_void_rod_0"));

		SpriteAnimation void_rod_u = {
			.base_name = "spr_void_rod_1",
			.frame_count = 1,
			.frame_length = VOID_ROD_LENGTH,
			.loops = false
		};
		void_rod_u.frames.push_back(&sprites.at("spr_void_rod_1"));

		SpriteAnimation void_rod_l = {
			.base_name = "spr_void_rod_2",
			.frame_count = 1,
			.frame_length = VOID_ROD_LENGTH,
			.loops = false
		};
		void_rod_l.frames.push_back(&sprites.at("spr_void_rod_2"));

		SpriteAnimation void_rod_d = {
			.base_name = "spr_void_rod_3",
			.frame_count = 1,
			.frame_length = VOID_ROD_LENGTH,
			.loops = false
		};
		void_rod_d.frames.push_back(&sprites.at("spr_void_rod_3"));

		animations.emplace("void_rod_r", void_rod_r);
		animations.emplace("void_rod_u", void_rod_u);
		animations.emplace("void_rod_l", void_rod_l);
		animations.emplace("void_rod_d", void_rod_d);

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
			sprite->setPosition(
				position.x + render_offset.x,
				position.y + render_offset.y
			);

			for (auto &effect : effects) {
				sf::Sprite *frame = effect.animation->frames[effect.current_frame];
				sf::Vector2f bounds = {
					frame->getLocalBounds().width / 2,
					frame->getLocalBounds().height / 2
				};

				frame->setOrigin(bounds.x, bounds.y);
				frame->setPosition(
					position.x + render_offset.x + effect.offset.x + bounds.x * util::SPRITE_SCALE,
					position.y + render_offset.y + effect.offset.y + bounds.y * util::SPRITE_SCALE
				);
				frame->setRotation(effect.rotation);
				target.draw(*frame);
			}

			if (falling_in)
				target.draw(sprites.at("spr_player_drop_0"));

			target.draw(*sprite);
		}
	}

	void draw_items(sf::RenderTarget &target) {
		if (has_void_rod()) {
			auto void_rod = sprites.find("spr_voidrod_icon_1");

			switch (picked_up_tile) {
				case TileType::VOID:
					void_rod = sprites.find("spr_voidrod_icon_1");
					break;
				case TileType::FLOOR:
					void_rod = sprites.find("spr_voidrod_icon_2");
					break;
				case TileType::GLASS:
					void_rod = sprites.find("spr_voidrod_icon_3");
					break;
				case TileType::GOAL:
					void_rod = sprites.find("spr_voidrod_icon_5");
					break;
				default:
					void_rod = sprites.find("spr_voidrod_icon_9");
					break;
			}

			if (void_rod == sprites.end()) return;
			void_rod->second.setPosition(
				6 * util::TILE_SIZE * util::SPRITE_SCALE,
				util::WINDOW_HEIGHT - util::TILE_SIZE * util::SPRITE_SCALE
			);
			target.draw(void_rod->second);
		}

		auto items = sprites.find("spr_items_0");
		if (items == sprites.end()) return;

		items->second.setTextureRect({0, 0, 0, 0});
		items->second.setPosition(
			8 * util::TILE_SIZE * util::SPRITE_SCALE,
			util::WINDOW_HEIGHT - util::TILE_SIZE * util::SPRITE_SCALE
		);

		if (has_void_memory()) {
			items->second.setTextureRect({
				0, 0,
				16, 15
			});
		}

		target.draw(items->second);

	}

	math::Vec2i get_position() const { return position; }

	math::Vec2i get_position_centered() const {
		return {
			static_cast<int32_t>(position.x + (util::TILE_SIZE * util::SPRITE_SCALE / 2)),
			static_cast<int32_t>(position.y + (util::TILE_SIZE * util::SPRITE_SCALE / 2))
		};
	}
	
	math::Vec2i get_position_grid() const {
		math::Vec2i grid(
			position.x / util::TILE_SIZE / util::SPRITE_SCALE,
			position.y / util::TILE_SIZE / util::SPRITE_SCALE
		);
		return grid;
	}

	Facing get_facing() const {
		return facing;
	}

	math::Vec2i vector_facing() const {
		switch (facing) {
			case Facing::UP:
				return math::UP;
			case Facing::RIGHT:
				return math::RIGHT;
			case Facing::LEFT:
				return math::LEFT;
			default:
				return math::DOWN;
		}
		return math::DOWN;
	}

	void set_position(const math::Vec2i pos) {
		position = pos;
	}

	void set_position_grid(const math::Vec2i pos) {
		position = util::grid_pos_to_world(pos);
	}

	void set_sprite(const std::string &name) {
		current_sprite_name = name;
		update_sprite();
	}

	int get_health() const {
		return health;
	}

	void set_health(const int health_) {
		health = health_;
	}

	bool has_void_rod() const {
		return void_rod;
	}

	bool has_void_memory() const {
		return void_memory;
	}

	void update_sprite() {
		std::stringstream next_frame{};
		next_frame << current_sprite_name << "_" << animation_step;

		sprite = &sprites[next_frame.str()];
	}

	void play_fall_animation() {
		sounds["snd_player_fall"].play();
		sprites.at("spr_player_drop_0").setPosition(
			position.x + render_offset.x,
			position.y + render_offset.y
		);

		falling_in = true;
		play_animation("drop");
	}

	void reset_animation_timer() {
		animation_timer = 0.f;
	}

	void move(const math::Vec2i pos, tile::TileDefinition target_tile) {
		if (move_timer > 0.0f) return;
		if (move_timer <= 0.0f) move_timer = time_between_movements;

		if (target_tile.is_fall) return;
		
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
			move_timer = time_between_punches;

			sounds["snd_push_small"].play();

			play_push_animation();

			return;
		}

		if (target_tile.is_fall)
			play_animation("fall");

		set_position(target);
	}

	void update(const float delta) {
		if (move_timer > 0.0f) {
			move_timer -= delta;
		} else {
			move_timer = 0.0f;
		}
	}

	void tick_animation(const float delta) {
		for (auto it = effects.begin(); it != effects.end(); ) {
			tick_animation_instance(*it, delta);
			if (it->finished)
				it = effects.erase(it);
			else
				++it;
		}

		if (!current_animation) return;

		if (falling_in) {
			fall_timer += delta;

			static constexpr float FALL_DURATION = 1.0f;

			float t = std::min(fall_timer / FALL_DURATION, 1.f);

			render_offset.y = std::lerp(-position.y - 96.f, 0, t);

			if (t >= 1.f) {
				falling_in = false;
				play_animation("enter");
			}

			return;
		}

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

	void play_effect(const std::string &name, const math::Vec2i offset = {0, 0}, const float rotation = 0.f) {
		auto it = animations.find(name);

		if (it == animations.end())
			return;

		effects.push_back({
			.animation = &it->second,
			.current_frame = 0,
			.timer = 0.f,
			.rotation = rotation,
			.offset = offset
		});
	}

	void clear_effects() {
		effects.clear();
	}

	TileType pick_up_place_tile(const TileType tile) {
		play_push_animation();
		play_void_rod_animation();
		if (has_tile()) {
			TileType previous = picked_up_tile;
			picked_up_tile = TileType::VOID;
			return previous;
		}
		else
			picked_up_tile = tile;
		return picked_up_tile;
	}

	void clear_held_tile() {
		picked_up_tile = TileType::VOID;
	}

	bool has_tile() const {
		return picked_up_tile != TileType::VOID;
	}
private:
	math::Vec2i position{};
	math::Vec2i render_offset{};

	Facing facing = Facing::DOWN;
	int health = 10;

	bool void_rod = true;
	bool void_memory = true;
	bool void_wings = false;
	bool void_sword = false;
	bool void_rod_true = false;

	TileType picked_up_tile = TileType::VOID;

	sf::Sprite *sprite = nullptr;
	std::string current_sprite_name = "";

	std::unordered_map<std::string, sf::Sprite> sprites;
	std::unordered_map<std::string, sf::SoundBuffer> sound_buffers;
	std::unordered_map<std::string, sf::Sound> sounds;

	float move_timer = 0.0f;
	float time_between_movements = 0.1f;
	float time_between_punches = time_between_movements * 3;

	size_t animation_step = 0;
	size_t animation_length = 2;
	float tick_counter = 0.0f;
	static constexpr float FRAME_LENGTH = 60.f / 84.f; // synced to BPM for funsies

	std::unordered_map<std::string, SpriteAnimation> animations;

	const AnimationInstance primary_animation;
	std::vector<AnimationInstance> effects;

	const SpriteAnimation *current_animation = nullptr;
	const SpriteAnimation *default_animation = nullptr;

	size_t current_frame = 0;
	float animation_timer = 0.f;

	bool falling_in = true;
	float fall_timer = 0.0f;

	std::unordered_set<std::string> idle_animations = {
		"spr_player_up",
		"spr_player_down",
		"spr_player_left",
		"spr_player_right",
	};

	void play_push_animation() {
		if (idle_animations.contains(current_animation->base_name))
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
	}

	void play_void_rod_animation() {
		switch (facing) {
			case Facing::RIGHT:
				play_effect("sparkle", math::RIGHT * util::TILE_SIZE * util::SPRITE_SCALE);
				play_effect("swipe", (math::RIGHT + math::UP) * (util::TILE_SIZE / 2) * util::SPRITE_SCALE, -90.f);
				play_effect("void_rod_r", math::RIGHT * util::TILE_SIZE * util::SPRITE_SCALE);
				break;
			case Facing::LEFT:
				play_effect("sparkle", math::LEFT * util::TILE_SIZE * util::SPRITE_SCALE);
				play_effect("swipe", math::LEFT * (util::TILE_SIZE / 2) * util::SPRITE_SCALE, 90.f);
				play_effect("void_rod_l", math::LEFT * util::TILE_SIZE * util::SPRITE_SCALE);
				break;
			case Facing::DOWN:
				play_effect("sparkle", math::DOWN * util::TILE_SIZE * util::SPRITE_SCALE);
				play_effect("swipe", (math::DOWN + math::RIGHT) * (util::TILE_SIZE / 2) * util::SPRITE_SCALE);
				play_effect("void_rod_d", math::DOWN * util::TILE_SIZE * util::SPRITE_SCALE);
				break;
			case Facing::UP:
				play_effect("sparkle", math::UP * util::TILE_SIZE * util::SPRITE_SCALE);
				play_effect("swipe", (math::UP + math::LEFT) * (util::TILE_SIZE / 2) * util::SPRITE_SCALE, 180.f);
				play_effect("void_rod_u", math::UP * util::TILE_SIZE * util::SPRITE_SCALE);
				break;
			default:
			break;
		}
	}

	void tick_animation_instance(AnimationInstance &instance, const float delta) {
		const SpriteAnimation *current_animation = instance.animation;
		if (!current_animation) return;

		instance.timer += delta;

		if (!current_animation->uses_global_time) {
			if (instance.timer < current_animation->frame_length)
				return;

			instance.timer = 0.f;
			instance.current_frame++;

			if (instance.current_frame >= current_animation->frames.size()) {
				instance.current_frame = 0;
				if (!current_animation->loops) {

					instance.finished = true;
				}
			}
		} else {
			float beat_duration = current_animation->frame_length;

			instance.current_frame =
				static_cast<size_t>(instance.timer / beat_duration)
				% current_animation->frames.size();
		}
	}
};

}

#endif // !MALAISE_PLAYER_HPP
