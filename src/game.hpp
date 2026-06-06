#ifndef MALAISE_GAME_HPP
#define MALAISE_GAME_HPP

#include <cstdint>
#include <ctime>
#include <filesystem>
#include <format>
#include <iostream>
#include <memory>
#include <queue>
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

#include "animation.hpp"
#include "animation_matrix.hpp"
#include "button.hpp"
#include "cursor.hpp"
#include "event_manager.hpp"
#include "text_dynamic.hpp"
#include "tile_definition.hpp"
#include "typewriter.hpp"
#include "util.hpp"
#include "vec2i.hpp"
#include "djikstra.hpp"
#include "player.hpp"
#include "tile.hpp"
#include "tilemap.hpp"
#include "tileset.hpp"
#include "tilemap_renderer.hpp"
#include "iris_transition.hpp"
#include "bitmap_font.hpp"

namespace malaise {

class Game {
public:
	Game(int argc, char *argv[])
		: rng(rd()) {

		print_controls_help();
		player.set_position({
			5 * util::TILE_SIZE * util::SPRITE_SCALE,
			7 * util::TILE_SIZE * util::SPRITE_SCALE
		});

		init_window();
		init_simulation(argc, argv);

		init_views();

		init_fonts();
		init_cursor();

		init_sprites();
		init_sounds();

		init_tiles();

		init_text_boxes();
		// init_dynamic_text();
		init_buttons();

		init_animations();
		init_events();
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
			handle_mouse_pan();
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

	const std::string WINDOW_TITLE = "SPA-DZ-02";

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
	malaise::animation::AnimationMatrix animation_matrix;
	std::vector<malaise::animation::Typewriter> typewriters;

	std::vector<std::shared_ptr<sf::Text>> text_boxes;
	std::vector<malaise::text::TextDynamic> dynamic_text_objects;
	std::queue<std::shared_ptr<malaise::text::TextDynamic>> scrollable_text_objects;

	std::vector<malaise::Button> buttons;

	malaise::events::EventManager event_manager;

	std::unordered_map<std::string, sf::Texture> textures;
	std::unordered_map<std::string, sf::Sprite> sprites;

	std::unordered_map<std::string, sf::SoundBuffer> sound_buffers;
	std::unordered_map<std::string, sf::Sound> sounds;

	// ---------- CURRENT POINTERS ----------;
	malaise::Cursor cursor;

	Player player;

	Tile goal{ {0,0}, {}, TileType::VOID };

	tile::TileMap tilemap{18, 9};
	tile::TileSet tileset;
	tile::TileMapRenderer tilemap_renderer;
	TileType current_tile = TileType::FLOOR;
	
	bool debug_mode = false;

	animation::SquareIrisTransition room_transition;

	std::vector<math::Vec2i> path_to_goal;

	// --------------- PRIVATE METHODS ---------------;

	void init_window(void) {
		window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), WINDOW_TITLE);
		window.setFramerateLimit(REFRESH_RATE);
		window.setMouseCursorVisible(false);
	}

	void init_simulation(int argc, char *argv[]) {
		DEBUG_PRINT("\n\n----- Please compile in Release mode for best perfomance!!! -----");
		DEBUG_PRINT("\n----- Please compile in Release mode for best perfomance!!! -----");
		DEBUG_PRINT("\n----- Please compile in Release mode for best perfomance!!! -----\n\n");

		// ----- SEED AND SIMULATION -----
		uint32_t seed = rng();

		if (argc > 1) { // Set seed based on program parameter
			seed = std::stoi(argv[1]);
		}
	}

	/* Initialize main render views and move the "camera" to the center
	 * of the initial "soup" (https://conwaylife.com/wiki/Soup)
	 * */
	void init_views(void) {
		world_view = window.getDefaultView();
		ui_view    = window.getDefaultView();

		// world_view.move(
		// 	-static_cast<float>(WINDOW_WIDTH)  / 2 + static_cast<float>(simulation.get_starting_enclosure_size()) / 2,
		// 	-static_cast<float>(WINDOW_HEIGHT) / 2 + static_cast<float>(simulation.get_starting_enclosure_size()) / 2
		// );
	}

	void init_fonts(void) {
		main_font.loadFromFile(util::RESOURCE_DIRECTORY + "fonts/" + "RetroByte.ttf");
		mario_font.loadFromFile(util::RESOURCE_DIRECTORY + "fonts/" + "Mario64.ttf");
		alkhemikal.load_from_files("spr_fnt_text_12.png", "glyphs_fnt_text_12.csv");
	}

	void init_buttons(void) {
		// buttons.reserve(16);

		// Example of how to instantiate a button
		// buttons.emplace_back(sf::Vector2f(100, 0), 100, 80, [&] {
		// 	stop();
		// });
	}

	void init_text_boxes() {
		// text_boxes.reserve(16);

		// text_boxes.push_back(std::make_unique<sf::Text>());
		// auto welcome_text = text_boxes.back(); // 0

		// // No need to set string if a typewriter is going to be used
		// welcome_text->setPosition(375, 300);
		// welcome_text->setFont(mario_font);
		// welcome_text->setCharacterSize(24);
		// welcome_text->setFillColor(sf::Color::White);
		// util::center_element(*welcome_text, welcome_text->getLocalBounds());
	}

	void init_dynamic_text(void) {
		dynamic_text_objects.reserve(16);

		auto initial_welcome_text = push_scrollable_text(main_font);
		initial_welcome_text->set_position({
			static_cast<float>(WINDOW_WIDTH / 2.f - 100),
			static_cast<float>(WINDOW_HEIGHT - 200)
		});
		initial_welcome_text->push_strings("Welcome to ", "\nConway's Game of Life");
		
		animation_matrix.push_and_create(animation::animation_idle(initial_welcome_text->get_segment(1), 1.5f));
		animation_matrix.push_current(animation::animation_idle_pop((initial_welcome_text->get_segment(1))));

		auto controls_explanation = push_scrollable_text(main_font);
		controls_explanation->set_position({
			static_cast<float>(100),
			static_cast<float>(100)
		});
		controls_explanation->push_strings("Use ", "MIDDLE MOUSE", " to pan / ", "SCROLL WHEEL", " to zoom");

		animation_matrix.push_and_create(animation::animation_idle(controls_explanation->get_segment(1)));
		animation_matrix.push_current(animation::animation_idle_shake(controls_explanation->get_segment(1)));

		animation_matrix.push_and_create(animation::animation_idle(controls_explanation->get_segment(3), 2.f));
		animation_matrix.push_current(animation::animation_idle_shake(controls_explanation->get_segment(3)));

		auto controls_explanation_2 = push_scrollable_text(main_font);
		controls_explanation_2->set_position({
			static_cast<float>(230),
			static_cast<float>(150)
		});
		controls_explanation_2->push_strings("LEFT CLICK", " to paint patterns", "\nRIGHT CLICK", " to erase a 10x10 area");

		animation_matrix.push_and_create(animation::animation_idle_shake(controls_explanation_2->get_segment(0)));

		animation_matrix.push_and_create(animation::animation_idle(controls_explanation_2->get_segment(2), 2.5f));
		animation_matrix.push_current(animation::animation_idle_shake(controls_explanation_2->get_segment(2)));

		auto controls_explanation_3 = push_scrollable_text(main_font);
		controls_explanation_3->set_position({
			static_cast<float>(70),
			static_cast<float>(200)
		});
		controls_explanation_3->push_strings("Left and right ", "ARROW KEYS", " to change selected pattern");

		animation_matrix.push_and_create(animation::animation_idle_shake(controls_explanation_3->get_segment(1)));

		auto controls_explanation_4 = push_scrollable_text(main_font);
		controls_explanation_4->set_position({
			static_cast<float>(200),
			static_cast<float>(250)
		});
		controls_explanation_4->push_strings("Press ", "C", " to toggle the ", "COLOR SELECTOR");

		animation_matrix.push_and_create(animation::animation_idle_shake(controls_explanation_4->get_segment(1)));

		animation_matrix.push_and_create(animation::animation_idle(controls_explanation_4->get_segment(3), 2.5f));
		animation_matrix.push_current(animation::animation_idle_pop(controls_explanation_4->get_segment(3)));

		auto space_advance = push_scrollable_text(main_font);
		space_advance->set_position({
			static_cast<float>(WINDOW_WIDTH / 2.f - 200.f),
			static_cast<float>(WINDOW_HEIGHT / 2.f - 100.f)
			});
		space_advance->push_strings("Hold ", "SPACE", " to advance the simulation");

		animation_matrix.push_and_create(animation::animation_idle_shake(space_advance->get_segment(1)));

		auto have_fun = push_scrollable_text(main_font);
		have_fun->set_position({
			static_cast<float>(WINDOW_WIDTH / 2.f - 50),
			static_cast<float>(WINDOW_HEIGHT / 2.f - 100.f)
		});
		have_fun->push_strings("Have fun", ":) !!!");

		animation_matrix.push_and_create(animation::animation_idle_pop(have_fun->get_segment(0)));
	}

	void init_sprites(void) {
		sf::Texture malaise_logo_texture;

		if (malaise_logo_texture.loadFromFile(util::RESOURCE_DIRECTORY + "sprites/malaise_logo.png")) {
			textures.emplace("malaise_logo", malaise_logo_texture);
			sprites.emplace("malaise_logo", textures.at("malaise_logo"));

			auto &malaise_sprite = sprites.at("malaise_logo");
			malaise_sprite.setPosition(WINDOW_WIDTH / 2.f - 128, WINDOW_HEIGHT / 2.f - 128);
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
				sprites[sprite_name].setScale(util::SPRITE_SCALE, util::SPRITE_SCALE);

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
				sf::Sound sound;
				sound.setVolume(50.f);
				sound.setBuffer(sound_buffers[sound_name]);
				sounds.emplace(sound_name, sound);
			}
		}

		player.init_sounds();
	}

	void init_tiles(void) {
		tileset.init_tile_definitions(textures);

		tilemap.set(2, 0, TileType::FLOOR);
		tilemap.set(2, 1, TileType::FLOOR_UNDER);
		tilemap.set(3, 0, TileType::GLASS);
		tilemap.set(4, 0, TileType::GOAL);
		tilemap.set(5, 0, TileType::WALL_CORNER_TOP_LEFT);
		tilemap.set(6, 0, TileType::WALL_TOP);
		tilemap.set(7, 0, TileType::WALL_CORNER_TOP_RIGHT);

		tilemap.load_from_file("br_002.txt");
		player.set_position_grid(tilemap.get_player_start_pos());
	}

	void init_animations(void) {
		// typewriters.emplace_back(*welcome_text, "Welcome to Cellbi! :)", 10.f, "");
	}

	void init_patterns(void) {
		// patterns = malaise::Pattern::load_patterns_from_folder(util::RESOURCE_DIRECTORY + "patterns/");

		// pattern_selected = patterns.at("dot");
	}

	void init_cursor(void) {
	}

	void init_events(void) {
		// event_manager.emplace_event(3.7f, [&]() {
		// 	if (scrollable_text_objects.empty() || scrollable_text_objects.size() < 6) return;
		// 	auto txt = scrollable_text_objects.front();
		// 	txt->push_strings("\n\n(press Enter to continue)");
		// });

		// Fallback in case someone gets stuck on the second text box
		// event_manager.emplace_event(20.f, [&]() {
		// 	if (scrollable_text_objects.size() != 5) return;
		// 	auto txt = scrollable_text_objects.front();
		// 	txt->push_strings("\n", "\n", "(press ", "Enter", " to scroll text)");
		// 	animation_matrix.push_and_create(animation::animation_idle(txt->get_segment(8), .3f));
		// 	animation_matrix.push_current(animation::animation_idle_pop(txt->get_segment(8)));
		// });

		// event_manager.emplace_event(2.f, [&]() {
		// 	inputs_locked = false;
		// });
	}

	void update_simulation(const float delta) {
		while (physics_accumulator >= physics_timestep) { // Limit framerate to physics tickrate
			if (physics_ticking) {
				step_towards_goal();
				// simulation.step();
			}
			physics_accumulator -= physics_timestep;
		}

		player.update(delta);
	}

	void draw_ui_elements(void) {
		window.setView(ui_view);

		for (const auto &btn : buttons) {
			btn.render(window);
		}

		for (const auto txt : text_boxes) {
			window.draw(*txt);
		}

		for (auto &txt : dynamic_text_objects) {
			txt.draw(window);
		}

		if (!scrollable_text_objects.empty())
			scrollable_text_objects.front()->draw(window);

		window.setView(world_view);

		std::string health = std::format("HP{:02}", player.get_health());
		alkhemikal.draw_text(window, health, {
			util::TILE_SIZE * util::SPRITE_SCALE,
			WINDOW_HEIGHT - util::TILE_SIZE * util::SPRITE_SCALE});

		std::string braine = tilemap.get_braine() > 255 ? "B???" : std::format("B{:03}", tilemap.get_braine());
		alkhemikal.draw_text(window, braine, {
			WINDOW_WIDTH - 2 * util::TILE_SIZE * util::SPRITE_SCALE,
			WINDOW_HEIGHT - util::TILE_SIZE * util::SPRITE_SCALE
		});

		player.draw_items(window);

		if (debug_mode) {
			// painting tile
			sf::Sprite sprite;
			sprite.setScale(util::SPRITE_SCALE, util::SPRITE_SCALE);

			const sf::Texture *texture = tileset.texture(current_tile);
			if (!texture) return;
			sprite.setTexture(*texture);
			sprite.setTextureRect(tileset.rect_for(current_tile));
			sprite.setPosition(0, WINDOW_HEIGHT - util::TILE_SIZE * util::SPRITE_SCALE);

			window.draw(sprite);

			sf::Vector2i hover_mouse_pos = sf::Mouse::getPosition(window);
			sf::Vector2f hover_world_pos = window.mapPixelToCoords(hover_mouse_pos);

			cursor.render(window, util::float_vector_to_integer(hover_world_pos));
		}

		room_transition.draw(window);
	}

	void draw_world_elements(void) {
		window.setView(world_view);

		draw_tiles();
		player.draw(window);
		// auto &gray = sprites["spr_player_down_0"];
		// gray.setPosition(200, 200);
		// gray.setScale(4, 4);
		// window.draw(gray);

		// Cell::draw_cells(window, simulation.get_active_cells());
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

		for (auto &animation_queue : animation_matrix.get_animations()) {
			if (animation_queue.front().is_finished()) // Play animations from the queue in sequence, popping when finished
				animation_queue.pop();

			if (animation_queue.empty()) continue;

			auto &current_animation = animation_queue.front();

			if (current_animation.expired())
				animation_queue.pop();
			else
				current_animation.update(delta_time); // Tick only the currently playing animation

		}

		for (auto &typewriter : typewriters) {
			typewriter.update(delta_time);
		}
	}

	void update_events(const float delta_time) {
		event_manager.update_and_run_events(delta_time);
	}

	void handle_mouse_pan(void) {
		if (!dragging) return;

		sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);

		sf::Vector2f world_pos_before = window.mapPixelToCoords(lastMousePos);
		sf::Vector2f world_pos_current = window.mapPixelToCoords(mouse_pos);

		sf::Vector2f delta_position = world_pos_before - world_pos_current;
		world_view.move(delta_position);

		lastMousePos = mouse_pos;
	}

	void handle_mouse_zoom(const float scroll_delta) {
		if (inputs_locked) return;

		sf::Vector2i pixel = sf::Mouse::getPosition(window);
		sf::Vector2f before = window.mapPixelToCoords(pixel);

		world_view.zoom(scroll_delta > 0.f ? 0.9f : 1.1f);

		sf::Vector2f after = window.mapPixelToCoords(pixel);
		world_view.move(before - after);
	}

	void handle_window_resize(const unsigned int width, const unsigned int height) {
		float window_ratio =
        static_cast<float>(width) /
        static_cast<float>(height);

		float target_ratio =
			WINDOW_WIDTH / (float)WINDOW_HEIGHT;

		float size_x = 1.f;
		float size_y = 1.f;
		float pos_x = 0.f;
		float pos_y = 0.f;

		if (window_ratio > target_ratio)
		{
			// window too wide
			size_x = target_ratio / window_ratio;
			pos_x = (1.f - size_x) / 2.f;
		}
		else
		{
			// window too tall
			size_y = window_ratio / target_ratio;
			pos_y = (1.f - size_y) / 2.f;
		}

		world_view.setViewport(
			sf::FloatRect(pos_x, pos_y, size_x, size_y)
		);

		// ui_view.setSize(width, height);
		// ui_view.setCenter(width / 2.f, height / 2.f);

		// float old_x = world_view.getSize().x; // Attempt to maintain camera center
		// world_view.setSize(width, height);
		// world_view.zoom(old_x / width);
	}

	void handle_realtime_inputs(void) {
		if (inputs_locked) return;

		// -------------------- SIMULATION TOGGLE --------------------;
		physics_ticking = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);

		if (not debug_mode) return;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
			cursor.set_type(Cursor::Type::EYEDROPPER);
			return;
		}

		cursor.set_type(Cursor::Type::PAINT_BRUSH);

		// -------------------- CELL PAINTING --------------------;
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			sf::Vector2i pixel_pos = sf::Mouse::getPosition(window);
			sf::Vector2f world_pos = window.mapPixelToCoords(pixel_pos);

			math::Vec2i center = {
				static_cast<int32_t>(world_pos.x),
				static_cast<int32_t>(world_pos.y)
			};
			math::Vec2i grid_pos = util::world_pos_to_grid(center);

			tilemap.set(grid_pos.x, grid_pos.y, current_tile);

		// -------------------- CELL ERASING --------------------;
		} else if (!physics_ticking && sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
			sf::Vector2i pixel_pos = sf::Mouse::getPosition(window);
			sf::Vector2f world_pos = window.mapPixelToCoords(pixel_pos);

			cursor.set_type(Cursor::Type::ERASER);

			math::Vec2i center = {
				static_cast<int32_t>(std::floor(world_pos.x)),
				static_cast<int32_t>(std::floor(world_pos.y))
			};
			math::Vec2i grid_pos = util::world_pos_to_grid(center);

			tilemap.set(grid_pos.x, grid_pos.y, TileType::VOID);
		}
	}
	
	void handle_single_inputs(sf::Event &event) {
		if (inputs_locked) return;

		switch (event.key.code) {
			case sf::Keyboard::Up:
				try_move_player(math::UP);
				break;
			case sf::Keyboard::Down:
				try_move_player(math::DOWN);
				break;
			case sf::Keyboard::Left:
				try_move_player(math::LEFT);
				break;
			case sf::Keyboard::Right:
				try_move_player(math::RIGHT);
				break;
			case sf::Keyboard::Z: {
				math::Vec2i target = player.vector_facing() * util::TILE_SIZE * util::SPRITE_SCALE;
				math::Vec2i target_world_pos = player.get_position() + target;
				math::Vec2i target_grid_pos = util::world_pos_to_grid(target_world_pos);
				TileType tile = tilemap.get(target_grid_pos.x, target_grid_pos.y);
				tile::TileDefinition target_tile = tileset.definition_for(tile);

				sf::Sprite &void_rod_sprite = sprites.at("spr_void_rod_0");

				switch (player.get_facing()) {
					case Facing::RIGHT:
						void_rod_sprite = sprites.at("spr_void_rod_0");
						break;
					case Facing::UP:
						void_rod_sprite = sprites.at("spr_void_rod_1");
						break;
					case Facing::LEFT:
						void_rod_sprite = sprites.at("spr_void_rod_2");
						break;
					case Facing::DOWN:
						void_rod_sprite = sprites.at("spr_void_rod_3");
						break;
					default:
						break;
				}

				void_rod_sprite.setPosition(target_world_pos.x, target_world_pos.y);
				window.draw(void_rod_sprite);

				if (!player.has_tile()) {
					if (target_tile.is_pickable) {
						player.pick_up_place_tile(tile);
						tilemap.set(target_grid_pos.x, target_grid_pos.y, TileType::VOID);
						sounds.at("snd_voidrod_store").play();

					}
				} else if (tile == TileType::VOID) {
					TileType place = player.pick_up_place_tile(tile);
					tilemap.set(target_grid_pos.x, target_grid_pos.y, place);
					sounds.at("snd_voidrod_place").play();
				}
				break;
			}
			case sf::Keyboard::D:
				debug_mode = not debug_mode;
				break;
			case sf::Keyboard::L:
				tilemap.export_to_file(tilemap.get_name() + ".txt", player.get_position_grid());
				break;
			case sf::Keyboard::X: {
				path_to_goal = path_through_tiles();
				break;
			}
			default:
				break;
		}
	}

	void handle_scroll_tile_switching(const float scroll_delta) {
		scroll_tiles(scroll_delta > 0.f ? 1 : -1);
	}

	void handle_sfml_events(void) {
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
				case sf::Event::Closed:
					stop();
					break;
				case sf::Event::Resized: { // Fix stretching on resize
					handle_window_resize(event.size.width, event.size.height);
					break;
				}
				case sf::Event::KeyPressed:
					switch (event.key.code) {
						case sf::Keyboard::Escape:
						case sf::Keyboard::F8: // Godot muscle memory...
							stop();
							break;
						case sf::Keyboard::Enter:
							advance_scrollable_text();
							break;
						default:
							handle_single_inputs(event);
							break;
					}
					break;
				case sf::Event::MouseWheelScrolled: {
					handle_scroll_tile_switching(event.mouseWheelScroll.delta);
					break;
				}
				case sf::Event::MouseButtonPressed:
					if (inputs_locked) break;

					switch (event.mouseButton.button) {
						case sf::Mouse::Left:
							for (auto &btn : buttons) {
								if (btn.hovered)
									btn.pressed = true;
							}

							// Eyedropper logic
							if (cursor.get_type() == Cursor::Type::EYEDROPPER) {
								sf::Vector2i hover_mouse_pos = sf::Mouse::getPosition(window);
								sf::Vector2f hover_world_pos = window.mapPixelToCoords(hover_mouse_pos);
								math::Vec2i hover_grid_pos = util::world_pos_to_grid(hover_world_pos);

								current_tile = tilemap.get(hover_grid_pos.x, hover_grid_pos.y);
							}

							break;
						case sf::Mouse::Middle:
							dragging = true;
							lastMousePos = sf::Mouse::getPosition(window);
							break;
						default:
							break;
					}
					break;
				case sf::Event::MouseButtonReleased:
					if (inputs_locked) break;

					if (event.mouseButton.button == sf::Mouse::Middle) {
						dragging = false;
					} if (event.mouseButton.button == sf::Mouse::Left) {
						for (auto &btn : buttons) {
							if (btn.pressed) {
								if (btn.hovered)
									btn.push();
								btn.pressed = false;
							}
						}
					}
					break;
				case sf::Event::MouseMoved:
					if (inputs_locked) break;

					for (auto &btn : buttons) {
						btn.hovered = btn.point_overlaps_screen(ui_view, sf::Mouse::getPosition(window));
					}

					break;
				default:
					break;
			}
		}
	}

	void update_window_title(const sf::Time &time_elapsed) {
		// Update only twice per second for readability
		constexpr unsigned int UPDATES_PER_SECOND = 2;
		if (render_accumulator.asSeconds() >= 1.f / UPDATES_PER_SECOND) {
			float fps = 1.f / time_elapsed.asSeconds();

			window.setTitle(WINDOW_TITLE + " | " + std::to_string((int)(fps + 0.5f)) + " FPS");

			render_accumulator = sf::Time::Zero;
		}
	}

	void print_controls_help(void) {
		std::stringstream help_menu{};

		help_menu << "Usage: spa-dz-02 [SEED]\n\n";

		help_menu << "Controls:\n\n";

		help_menu << "  Enter \t\t\t\t advance to next text box or close text\n\n";
		help_menu << "  Hold Middle Mouse and Drag \t\t move view\n";
		help_menu << "  Mouse Wheel Scroll \t\t\t zoom in / out\n\n";

		help_menu << "  Left Mouse Click \t\t\t paint with selected pattern\n";
		help_menu << "  Right Mouse Click \t\t\t erase a 10x10 area\n";
		help_menu << "  Left & Right Arrow Keys \t\t cycle selected pattern\n\n";

		help_menu << "  C \t\t\t\t\t toggle the color selector UI\n";
		help_menu << "  X \t\t\t\t\t swap current color\n";
		help_menu << "  LCtrl + Left Mouse Click \t\t use the color picker";
		help_menu << "\n";

		std::cout << help_menu.str();
	}

	inline void advance_scrollable_text(void) {
		if (scrollable_text_objects.empty()) return;

		scrollable_text_objects.pop();
		if (scrollable_text_objects.empty())
			inputs_locked = false;
	}

	/*
	 * Helper function to push scrollable text to the screen
	 * and lock input until it's finished. Also returns the pushed
	 * malaise::text::TextDynamic object for convenience.
	 */
	inline std::shared_ptr<text::TextDynamic> push_scrollable_text(const sf::Font &font) {
		auto scrollable = std::make_shared<text::TextDynamic>(font);
		scrollable_text_objects.push(scrollable);
		inputs_locked = true; // lock inputs because scrollable text is displayed
		return scrollable;
	}

	inline sf::Color get_screen_pixel(const sf::Vector2i pos) {
		static std::unique_ptr<sf::Texture> screen_pixels = nullptr;
		if (!screen_pixels) {
			screen_pixels = std::make_unique<sf::Texture>();
			screen_pixels->create(window.getSize().x, window.getSize().y);
		}
		screen_pixels->update(window);
		sf::Image img = screen_pixels->copyToImage();
		return img.getPixel(pos.x, pos.y);
	}

	inline math::Vec2i world_pos_to_grid(const math::Vec2i world_pos) {
		return {
			static_cast<int32_t>(world_pos.x / util::TILE_SIZE / util::SPRITE_SCALE),
			static_cast<int32_t>(world_pos.y / util::TILE_SIZE / util::SPRITE_SCALE)
		};
	}

	inline void try_move_player(const math::Vec2i direction) {
		math::Vec2i target = direction * util::TILE_SIZE * util::SPRITE_SCALE;
		math::Vec2i target_grid_pos = util::world_pos_to_grid(player.get_position() + target);

		tile::TileDefinition target_tile = tileset.definition_for(tilemap.get(target_grid_pos.x, target_grid_pos.y));
		player.move(target, target_tile);
		if (target_tile.is_stairs and not tilemap.get_level_next().empty()) {
			inputs_locked = true;
			sounds["snd_stairs"].play();
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

	inline std::vector<math::Vec2i> path_through_tiles() {
		std::vector<math::Vec2i> walkable_tiles;
		walkable_tiles.reserve(tilemap.get_size());

		math::Vec2i goal{};

		for (int32_t y = 0; y < tilemap.get_height(); y++) {
			for (int32_t x = 0; x < tilemap.get_width(); x++) {
				const TileType tile = tilemap.get(x, y);
				tile::TileDefinition definition = tileset.definition_for(tile);
				math::Vec2i position = {x, y};

				if (!definition.is_collidable and !definition.is_fall)
					walkable_tiles.push_back(position);
				if (definition.is_stairs)
					goal = position;
			}
		}

		return malaise::algorithm::djikstras_algorithm(walkable_tiles, player.get_position_grid(), goal);
	}

	inline void draw_tiles() {
		tilemap_renderer.draw(window, tilemap, tileset);
	}

	inline void step_towards_goal() {
		if (path_to_goal.empty()) return;
		auto next_step = path_to_goal.front();
		path_to_goal.erase(path_to_goal.begin());
		math::Vec2i target = next_step - player.get_position_grid();
		std::cout << "(" << next_step.x << ", " << next_step.y << ") | (" << target.x << ", " << target.y << ")\n";
		try_move_player(target);
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
		size_t tile_count = static_cast<size_t>(TileType::COUNT);

		int new_index = static_cast<int>(current_tile) + step;
		if (new_index < 0)
			new_index += tile_count;
		else if (new_index >= tile_count)
			new_index -= tile_count;

		current_tile = static_cast<TileType>(new_index);
	}
};

}

#endif // !MALAISE_GAME_HPP
