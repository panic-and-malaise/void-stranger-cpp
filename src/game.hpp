#ifndef MALAISE_GAME_HPP
#define MALAISE_GAME_HPP

#include <SFML/System/Angle.hpp>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <format>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>
#include <sstream>

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Window.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

#include "cursor.hpp"
#include "entity.hpp"
#include "entity_chest_void_rod.hpp"
#include "event_manager.hpp"
#include "sprite_animation.hpp"
#include "tile_definition.hpp"
#include "tile_type.hpp"
#include "util.hpp"
#include "vec2i.hpp"
#include "player.hpp"
#include "tilemap.hpp"
#include "tileset.hpp"
#include "tilemap_renderer.hpp"
#include "iris_transition.hpp"
#include "bitmap_font.hpp"
#include "entity_chest.hpp"

namespace malaise {

class Game {
public:
	Game(int argc, char *argv[])
		: rng(rd()) {

		init_window();

		init_views();

		init_fonts();

		init_sprites();
		init_sounds();

		init_tiles();

		init_animations();
	}

	// --------------- PUBLIC METHODS ---------------;

	int main_loop(void) {
		// play_song("mus_extended_encore");

		sf::Music extended_encore;
		if (extended_encore.openFromFile(util::MUSIC_DIRECTORY + "mus_extended_encore.mp3"))
			event_manager.emplace_event(2.f, [&]() {
				extended_encore.play();
				player.reset_animation_timer();
				inputs_locked = false;
			});


		event_manager.emplace_event(1.f, [&]() {
			player.set_health(7);
			sounds.at("snd_push").play();
		});

		room_transition.start_open(player.get_position_centered());
		player.play_fall_animation();
		inputs_locked = true;

		previous_time = render_clock.getElapsedTime();

		while (running) {
			physics_accumulator += physics_clock.restart();

			handle_sfml_events();
			handle_realtime_inputs();

			render();
			
			current_time = render_clock.getElapsedTime();
			sf::Time time_elapsed = current_time - previous_time;

			float delta_time = time_elapsed.asSeconds();
			render_accumulator += time_elapsed;

			update_simulation(delta_time);
			update_events(delta_time);
			update_animations(delta_time);
			update_window_title(time_elapsed);

			previous_time = current_time;
		}

		return 0;
	}

	void stop(void) {
		running = false;
		window.close();
	}

private:
	// --------------- MEMBER FLAGS ---------------;
	bool running = true;
	bool physics_ticking = false;
	bool inputs_locked = false;

	// ---------- SFML WINDOW & VIEWS ----------;
	sf::RenderWindow window;

	sf::View world_view;
	sf::View ui_view;

	// ------------ PROGRAM CONSTANTS ------------;
	static constexpr size_t WINDOW_WIDTH = util::WINDOW_WIDTH;
	static constexpr size_t WINDOW_HEIGHT = util::WINDOW_HEIGHT;

	static constexpr size_t REFRESH_RATE = 60;
	static constexpr size_t PHYSICS_TICK_RATE = 10;

	const std::string WINDOW_TITLE = "Void Stranger";

	// ---------- RANDOM NUMBER GENERATION ----------;
	std::random_device rd{};
	std::mt19937 rng;

	// ----- CLOCKS, COUNTERS, AND TIME -----;
	sf::Clock render_clock;
	sf::Clock physics_clock;

	sf::Time previous_time;
	sf::Time current_time;

	sf::Time render_accumulator  = sf::Time::Zero;
	sf::Time physics_accumulator = sf::Time::Zero;

	const sf::Time render_timestep  = sf::milliseconds(1000 / REFRESH_RATE);
	const sf::Time physics_timestep = sf::milliseconds(1000 / PHYSICS_TICK_RATE);

	// ---------- MOUSE CONTROLS ----------;
	bool dragging = false;
	sf::Vector2i lastMousePos;

	// ---------- FONTS ----------;
	sf::Font main_font;
	sf::Font mario_font;
	font::BitmapFont alkhemikal;

	// --------------- OBJECT COLLECTIONS ---------------;
	malaise::events::EventManager event_manager;

	std::unordered_map<std::string, sf::Texture> textures;
	std::unordered_map<std::string, sf::Sprite> sprites;

	std::unordered_map<std::string, sf::SoundBuffer> sound_buffers;
	std::unordered_map<std::string, sf::Sound> sounds;

	// ---------- CURRENT POINTERS ----------;
	malaise::Cursor cursor;

	Player player;

	tile::TileMap tilemap{18, 9};
	tile::TileSet tileset;
	tile::TileMapRenderer tilemap_renderer;

	tile::TileType current_tile = tile::TileType::FLOOR;
	EntityType current_entity = EntityType::EGG;
	
	bool debug_mode = false;

	animation::SquareIrisTransition room_transition;

	std::unordered_map<std::string, animation::SpriteAnimation> animations;
	std::vector<animation::AnimationInstance> effects;

	// --------------- PRIVATE METHODS ---------------;

	void init_window(void) {
		window.create(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), WINDOW_TITLE);
		window.setFramerateLimit(REFRESH_RATE);
		window.setMouseCursorVisible(false);
	}

	void init_views(void) {
		world_view = window.getDefaultView();
		ui_view    = window.getDefaultView();
	}

	void init_fonts(void) {
		alkhemikal.load_from_files("spr_fnt_text_12.png", "glyphs_fnt_text_12.csv");
	}
	void init_sprites(void) {
		sf::Texture malaise_logo_texture;

		if (malaise_logo_texture.loadFromFile(util::RESOURCE_DIRECTORY + "sprites/malaise_logo.png")) {
			textures.emplace("malaise_logo", malaise_logo_texture);
			sprites.emplace("malaise_logo", textures.at("malaise_logo"));

			auto &malaise_sprite = sprites.at("malaise_logo");
			malaise_sprite.setPosition({WINDOW_WIDTH / 2.f - 128, WINDOW_HEIGHT / 2.f - 128});
			malaise_sprite.setColor(sf::Color(255, 255, 255, 0));
		}

		// Load all sprites from the sprites directory
		for (const auto &directory : std::filesystem::recursive_directory_iterator(util::SPRITE_DIRECTORY)) {
			std::string path = directory.path().string();
			std::string filename = std::filesystem::path(directory).filename().string();

			if (filename.empty() or filename.substr(0, 3) != "spr") continue;

			std::string sprite_name = std::filesystem::path(directory).filename().replace_extension("").string();
			sf::Texture texture;

			if (texture.loadFromFile(path)) {
				textures.emplace(sprite_name, texture);
				sprites.emplace(sprite_name, textures.at(sprite_name));
				sprites.at(sprite_name).setScale({util::SPRITE_SCALE, util::SPRITE_SCALE});

				// auto &sprite = sprites.at(filename);
				// sprite.setPosition(WINDOW_WIDTH / 2.f - 128, WINDOW_HEIGHT / 2.f - 128);
				// sprite.setColor(sf::Color(255, 255, 255, 0));
			}
		}
		
		cursor.load_sprites();

		player.load_sprites(sprites);
		player.init_animations();
		
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
				sf::Sound sound(sound_buffers[sound_name]);
				sound.setVolume(50.f);
				sounds.emplace(sound_name, sound);
			}
		}

		player.init_sounds();
	}

	void init_tiles(void) {
		tileset.init_tile_definitions(textures);

		tilemap.set(2, 0, tile::TileType::FLOOR);
		tilemap.set(2, 1, tile::TileType::FLOOR_UNDER);
		tilemap.set(3, 0, tile::TileType::GLASS);
		tilemap.set(4, 0, tile::TileType::GOAL);
		tilemap.set(5, 0, tile::TileType::WALL_CORNER_TOP_LEFT);
		tilemap.set(6, 0, tile::TileType::WALL_TOP);
		tilemap.set(7, 0, tile::TileType::WALL_CORNER_TOP_RIGHT);

		tilemap.load_from_file("br_002.txt");
		player.set_position_grid(tilemap.get_player_start_pos());
	}

	void init_animations(void) {
		animation::SpriteAnimation fall;

		fall.base_name = "spr_fall";
		fall.frame_count = 6;
		fall.frame_length = 0.1f;
		fall.loops = false;
		fall.load_frames_by_name(sprites);

		animations.emplace("fall", fall);
	}

	void update_simulation(const float delta) {
		while (physics_accumulator >= physics_timestep) { // Limit framerate to physics tickrate
			physics_accumulator -= physics_timestep;
		}

		player.update(delta);
		tilemap.update(delta);

		for (auto &entity : tilemap.get_entities()) {
			if (entity->falling) {
				play_effect("fall", entity->get_position() * util::TILE_SIZE_SCALED);
				sounds.at("snd_fall").play();
				entity->falling = false;
				entity->dead = true;
			}
		}

		if (player.is_dead()) {

			player.set_dead(false);
			inputs_locked = true;
			player.clear_effects();

			room_transition.start_close(player.get_position_centered());

			event_manager.emplace_event(1.3f, [&]() {
				if (player.get_locusts() < 0)
					stop();

				tilemap.load_from_file(tilemap.get_name() + ".txt");
				player.set_position_grid(tilemap.get_player_start_pos());
				player.play_animation("blink");
				player.clear_held_tile();

				room_transition.start_open(player.get_position_centered());
				inputs_locked = false;
			});
		}
	}

	void draw_ui_elements(void) {
		window.setView(ui_view);

		window.setView(world_view);

		std::string health = std::format("HP{:02}", player.get_health());
		alkhemikal.draw_text(window, health, {
			util::TILE_SIZE_SCALED,
			WINDOW_HEIGHT - util::TILE_SIZE_SCALED});

		std::string braine = tilemap.get_braine() > 255 ? "B???" : std::format("B{:03}", tilemap.get_braine());
		alkhemikal.draw_text(window, braine, {
			WINDOW_WIDTH - 2 * util::TILE_SIZE_SCALED,
			WINDOW_HEIGHT - util::TILE_SIZE_SCALED
		});

		if (player.ever_acquired_locust()) {
			std::string locusts = player.get_locusts() > 99 ? "∞" : std::format("{:02}", player.get_locusts());
			alkhemikal.draw_text(window, locusts, {
				5 * util::TILE_SIZE_SCALED,
				WINDOW_HEIGHT - util::TILE_SIZE_SCALED
			});
		}

		player.draw_items(window);

		if (debug_mode) {
			// painting tile
			const sf::Texture *texture = tileset.texture(current_tile);
			if (!texture) return;
			sf::Sprite sprite(*texture);
			sprite.setScale({util::SPRITE_SCALE, util::SPRITE_SCALE});

			sprite.setTextureRect(tileset.rect_for(current_tile));
			sprite.setPosition({0, WINDOW_HEIGHT - util::TILE_SIZE_SCALED});

			window.draw(sprite);

			texture = tileset.texture(static_cast<tile::TileType>(current_entity));
			
			if (!texture) return;
			sprite.setTexture(*texture);
			sprite.setTextureRect(sf::IntRect({0, 0}, {16, 16}));
			sprite.setPosition({util::TILE_SIZE_SCALED, WINDOW_HEIGHT - util::TILE_SIZE_SCALED});

			window.draw(sprite);

			sf::Vector2i hover_mouse_pos = sf::Mouse::getPosition(window);
			sf::Vector2f hover_world_pos = window.mapPixelToCoords(hover_mouse_pos);

			cursor.render(window, util::float_vector_to_integer(hover_world_pos));
		}

		for (auto &effect : effects) {
			sf::Sprite *frame = effect.animation->frames[effect.current_frame];
			sf::Vector2f bounds = {
				frame->getLocalBounds().size.x / 2,
				frame->getLocalBounds().size.y / 2
			};

			frame->setOrigin(bounds);
			frame->setPosition({
				effect.offset.x + bounds.x * util::SPRITE_SCALE,
				effect.offset.y + bounds.y * util::SPRITE_SCALE
			});
			frame->setRotation(sf::degrees(effect.rotation));
			window.draw(*frame);
		}


		room_transition.draw(window);
	}

	void draw_world_elements(void) {
		window.setView(world_view);

		draw_tiles();
		player.draw(window);
	}

	void render(void) {
		window.clear();

		draw_world_elements();
		draw_ui_elements();

		window.display();
	}

	void update_animations(const float delta_time) {
		player.tick_animation(delta_time);
		room_transition.update(delta_time);

		for (auto it = effects.begin(); it != effects.end(); ) {
			tick_animation_instance(*it, delta_time, true);
			if (it->finished)
				it = effects.erase(it);
			else
				++it;
		}
	}

	void update_events(const float delta_time) {
		event_manager.update_and_run_events(delta_time);
	}

	void handle_window_resize(const unsigned int width, const unsigned int height) {
		float window_ratio = static_cast<float>(width) / static_cast<float>(height);

		float target_ratio = WINDOW_WIDTH / (float)WINDOW_HEIGHT;

		float size_x = 1.f;
		float size_y = 1.f;
		float pos_x = 0.f;
		float pos_y = 0.f;

		if (window_ratio > target_ratio) {
			size_x = target_ratio / window_ratio;
			pos_x = (1.f - size_x) / 2.f;
		}
		else {
			size_y = window_ratio / target_ratio;
			pos_y = (1.f - size_y) / 2.f;
		}

		world_view.setViewport(
			sf::FloatRect({pos_x, pos_y}, {size_x, size_y})
		);
	}

	void handle_realtime_inputs(void) {
		if (inputs_locked) return;

		// -------------------- SIMULATION TOGGLE --------------------;
		physics_ticking = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);

		if (not debug_mode) return;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
			cursor.set_type(Cursor::Type::EYEDROPPER);
			return;
		}

		cursor.set_type(Cursor::Type::PAINT_BRUSH);

		// -------------------- TILE PAINTING --------------------;
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
			sf::Vector2i pixel_pos = sf::Mouse::getPosition(window);
			sf::Vector2f world_pos = window.mapPixelToCoords(pixel_pos);

			math::Vec2i center = {
				static_cast<int32_t>(world_pos.x),
				static_cast<int32_t>(world_pos.y)
			};
			math::Vec2i grid_pos = util::world_pos_to_grid(center);

			tilemap.set(grid_pos.x, grid_pos.y, current_tile);

		// -------------------- TILE ERASING --------------------;
		} else if (!physics_ticking && sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
			sf::Vector2i pixel_pos = sf::Mouse::getPosition(window);
			sf::Vector2f world_pos = window.mapPixelToCoords(pixel_pos);

			cursor.set_type(Cursor::Type::ERASER);

			math::Vec2i center = {
				static_cast<int32_t>(std::floor(world_pos.x)),
				static_cast<int32_t>(std::floor(world_pos.y))
			};
			math::Vec2i grid_pos = util::world_pos_to_grid(center);

			tilemap.set(grid_pos.x, grid_pos.y, tile::TileType::VOID);

		// -------------------- ENTITY PAINTING --------------------;
		} else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Middle)) {
			sf::Vector2i pixel_pos = sf::Mouse::getPosition(window);
			sf::Vector2f world_pos = window.mapPixelToCoords(pixel_pos);

			math::Vec2i center = {
				static_cast<int32_t>(world_pos.x),
				static_cast<int32_t>(world_pos.y)
			};
			math::Vec2i grid_pos = util::world_pos_to_grid(center);

			if (tilemap.get_entity(grid_pos.x, grid_pos.y)) return;

			switch (current_entity) {
				case EntityType::EGG:
					tilemap.add_entity<Egg>(grid_pos);
					break;
				case EntityType::CHEST:
					tilemap.add_entity<entity::Chest>(grid_pos);
					break;
				case EntityType::ATONER:
					tilemap.add_entity<Atoner>(grid_pos);
					break;
				case EntityType::CHEST_VOID_ROD:
					tilemap.add_entity<entity::ChestVoidRod>(grid_pos);
					break;
				default:
					break;
			}
		}
	}
	
	void handle_single_inputs(const sf::Event &event) {
		if (inputs_locked) return;
		const auto* key = event.getIf<sf::Event::KeyPressed>();
		switch (key->code) {
			case sf::Keyboard::Key::Up:
				try_move_player(math::UP);
				break;
			case sf::Keyboard::Key::Down:
				try_move_player(math::DOWN);
				break;
			case sf::Keyboard::Key::Left:
				try_move_player(math::LEFT);
				break;
			case sf::Keyboard::Key::Right:
				try_move_player(math::RIGHT);
				break;
			case sf::Keyboard::Key::Z: {
				math::Vec2i target = player.vector_facing() * util::TILE_SIZE_SCALED;
				math::Vec2i target_world_pos = player.get_position() + target;
				math::Vec2i target_grid_pos = util::world_pos_to_grid(target_world_pos);

				tile::TileType tile = tilemap.get(target_grid_pos.x, target_grid_pos.y);
				tile::TileDefinition target_tile = tileset.definition_for(tile);

				Entity *target_entity = tilemap.get_entity(target_grid_pos.x, target_grid_pos.y);

				if (target_entity) {
					if (target_entity->on_interact(player))
						sounds.at("snd_open").play();
					break;
				}

				if (not player.has_void_rod()) break;

				if (!player.has_tile()) {
					if (target_tile.is_pickable) {
						player.pick_up_place_tile(tile);
						tilemap.set(target_grid_pos.x, target_grid_pos.y, tile::TileType::VOID);
						sounds.at("snd_voidrod_store").play();

					}
				} else if (tile == tile::TileType::VOID) {
					tile::TileType place = player.pick_up_place_tile(tile);
					tilemap.set(target_grid_pos.x, target_grid_pos.y, place);
					sounds.at("snd_voidrod_place").play();
				}

				break;
			}
			case sf::Keyboard::Key::D:
				debug_mode = not debug_mode;
				break;
			case sf::Keyboard::Key::L:
				tilemap.export_to_file(tilemap.get_name() + ".txt", player.get_position_grid());
				break;
			default:
				break;
		}
	}

	void handle_scroll_tile_switching(const float scroll_delta) {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
			scroll_entities(scroll_delta > 0.f ? 1 : -1);
		} else {
			scroll_tiles(scroll_delta > 0.f ? 1 : -1);
		}
	}

	void handle_sfml_events(void) {
		while (const auto event = window.pollEvent()) {
			if (event->is<sf::Event::Closed>()) {
				stop();
			}
			else if (const auto* resized = event->getIf<sf::Event::Resized>()) {
				// Fix stretching on resize
				handle_window_resize(
					resized->size.x,
					resized->size.y
				);
			}
			else if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
				switch (key->code) {
					case sf::Keyboard::Key::Escape:
					case sf::Keyboard::Key::F8: // Godot muscle memory...
						stop();
						break;

					case sf::Keyboard::Key::Enter:
						// advance_scrollable_text();
						break;

					default:
						handle_single_inputs(*event);
						break;
				}
			}
			else if (const auto* wheel = event->getIf<sf::Event::MouseWheelScrolled>()) {
				handle_scroll_tile_switching(wheel->delta);
			}
			else if (const auto* mouse_pressed = event->getIf<sf::Event::MouseButtonPressed>()) {
				if (inputs_locked)
					continue;
				switch (mouse_pressed->button) {
					case sf::Mouse::Button::Left: {

						// Eyedropper logic
						if (cursor.get_type() == Cursor::Type::EYEDROPPER) {
							const sf::Vector2i hoverMousePos =
								sf::Mouse::getPosition(window);

							const sf::Vector2f hoverWorldPos =
								window.mapPixelToCoords(hoverMousePos);

							const math::Vec2i hoverGridPos =
								util::world_pos_to_grid(hoverWorldPos);

							current_tile =
								tilemap.get(hoverGridPos.x, hoverGridPos.y);
						}

						break;
					}

					default:
						break;
				}
			}
			else if (const auto* mouse_released = event->getIf<sf::Event::MouseButtonReleased>()) {
				if (inputs_locked)
					continue;
			}
			else if (event->is<sf::Event::MouseMoved>()) {
				if (inputs_locked)
					continue;
			}
		}
	}

	void update_window_title(const sf::Time &time_elapsed) {
		// Update only twice per second for readability
		constexpr unsigned int UPDATES_PER_SECOND = 2;
		if (render_accumulator.asSeconds() >= 1.f / UPDATES_PER_SECOND) {
			float fps = 1.f / time_elapsed.asSeconds();

			// window.setTitle(WINDOW_TITLE + " | " + std::to_string((int)(fps + 0.5f)) + " FPS");

			render_accumulator = sf::Time::Zero;
		}
	}

	inline math::Vec2i world_pos_to_grid(const math::Vec2i world_pos) {
		return {
			static_cast<int32_t>(world_pos.x / util::TILE_SIZE / util::SPRITE_SCALE),
			static_cast<int32_t>(world_pos.y / util::TILE_SIZE / util::SPRITE_SCALE)
		};
	}

	inline void try_move_player(const math::Vec2i direction) {
		math::Vec2i target = direction * util::TILE_SIZE_SCALED;
		math::Vec2i target_grid_pos = util::world_pos_to_grid(player.get_position() + target);

		math::Vec2i target_push_pos = util::world_pos_to_grid(player.get_position() + target + target);

		tile::TileDefinition target_tile = tileset.definition_for(tilemap.get(target_grid_pos.x, target_grid_pos.y));
		tile::TileDefinition push_tile = tileset.definition_for(tilemap.get(target_push_pos.x, target_push_pos.y));

		Entity *target_entity = tilemap.get_entity(target_grid_pos.x, target_grid_pos.y);
		Entity *push_entity = tilemap.get_entity(target_push_pos.x, target_push_pos.y);

		player.move(target, target_tile, target_entity, push_tile, push_entity);

		if (target_tile.is_stairs and not tilemap.get_level_next().empty() and not target_entity) {
			inputs_locked = true;
			sounds.at("snd_stairs").play();
			player.clear_effects();
			room_transition.start_close(player.get_position_centered());

			event_manager.emplace_event(1.3f, [&]() {
				tilemap.load_from_file(tilemap.get_level_next());
				player.set_position_grid(tilemap.get_player_start_pos());
				player.play_animation("blink");
				player.clear_held_tile();

				room_transition.start_open(player.get_position_centered());
				inputs_locked = false;
			});

			event_manager.emplace_event(3.f, [&]() {
				player.play_animation("idle_down");
			});
		}
	}

	inline void draw_tiles() {
		tilemap_renderer.draw(window, tilemap, tileset);
	}

	void play_song(const std::string &song_name) {
		std::string full_path = util::MUSIC_DIRECTORY + song_name + ".mp3";
		sf::Music song;
		if (!song.openFromFile(full_path)) {
			std::cerr << "Couldn't open " << full_path << "!\n";

			return;
		}
		song.play();
	}

	inline void scroll_tiles(const int step) {
		size_t tile_count = static_cast<size_t>(tile::TileType::COUNT);

		int new_index = static_cast<int>(current_tile) + step;
		if (new_index < 0)
			new_index += tile_count;
		else if (new_index >= tile_count)
			new_index -= tile_count;

		current_tile = static_cast<tile::TileType>(new_index);
	}

	inline void scroll_entities(const int step) {
		size_t tile_min = static_cast<size_t>(EntityType::EGG);
		size_t tile_count = static_cast<size_t>(EntityType::CHEST_VOID_ROD);

		int new_index = static_cast<int>(current_entity) + step;

		if (new_index < tile_min)
			new_index = tile_count;
		else if (new_index > tile_count)
			new_index = tile_min;

		// std::cout << "(" << tile_min << " -> " << tile_count << "): " << new_index << '\n';

		current_entity = static_cast<EntityType>(new_index);
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

	void tick_animation_instance(animation::AnimationInstance &instance, const float delta, const bool is_effect = false) {
		const animation::SpriteAnimation *current_animation = instance.animation;
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

#endif // !MALAISE_GAME_HPP
