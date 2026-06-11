#ifndef MALAISE_TILEMAP_HPP
#define MALAISE_TILEMAP_HPP

#include <algorithm>
#include <cctype>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <SFML/Graphics/RenderTarget.hpp>

#include "entity_chest.hpp"
#include "entity_chest_void_rod.hpp"
#include "file_wrapper.hpp"
#include "tile_type.hpp"
#include "util.hpp"
#include "vec2i.hpp"
#include "entity.hpp"

namespace malaise::tile {

class TileMap {
public:
	TileMap(const size_t width_, const size_t height_)
		: width(width_), height(height_),
		tiles(width * height, TileType::VOID) {
		// init side boundaries
		for (size_t y = 0; y < 9; y++) {
			for (size_t x = 0; x < 2; x++) {
				set(x, y, TileType::BOUNDS);
			}
		}

		for (size_t y = 0; y < 9; y++) {
			for (size_t x = 16; x < 18; x++) {
				set(x, y, TileType::BOUNDS);
			}
		}

	}

	void load_from_file(const std::string &filename) {
		util::FileWrapper file(util::LEVEL_DIRECTORY + filename);

		if (!file) {
			std::cerr << "Error loading " << filename << "!\n";
			return;
		}

		name = std::filesystem::path(filename).filename().replace_extension("").string();

		tiles.clear();
		entities.clear();

		std::string temp;

		file.get() >> temp >> width >> height;
		file.get() >> temp >> player_start_pos.x >> player_start_pos.y;
		file.get() >> temp >> braine;
		file.get() >> temp >> level_next;

		std::string line;
		bool has_entities = false;

		while (file.read_line(line)) {
			std::stringstream buffer(line);
			char next_tile;

			if (line == "ENTITIES") {
				has_entities = true;
				break;
			}
				
			while (buffer >> next_tile) {
				if (next_tile == '#') {
					tiles.push_back(TileType::BOUNDS);
					continue;
				}

				TileType index = static_cast<TileType>(next_tile - '0');
				tiles.push_back(index);
			}
		}

		fill_ui_elements();

		if (not has_entities) return;

		while (file.read_line(line)) {
			std::stringstream buffer(line);

			std::string name;
			math::Vec2i pos;

			buffer >> name >> pos.x >> pos.y;

			std::transform(name.begin(), name.end(), name.begin(), ::tolower);

			if (name == "egg") {
				add_entity<Egg>(pos);
			} else if (name == "chest") {
				add_entity<entity::Chest>(pos);
			} else if (name == "atoner") {
				add_entity<Atoner>(pos);
			} else if (name == "void_rod_chest") {
				add_entity<entity::ChestVoidRod>(pos);
			}
		}
	}

	void export_to_file(const std::string &filename, const math::Vec2i player_pos = {0, 0}) {
		util::FileWrapper file(util::LEVEL_DIRECTORY + filename, std::ios::out);

		if (!file) {
			std::cerr << "Error loading " << filename << "!\n";
			return;
		}

		file.write_line("size " + std::to_string(width) + " " + std::to_string(height));

		if (player_pos.x and player_pos.y)
			file.write_line("player " + std::to_string(player_pos.x) + " " + std::to_string(player_pos.y));
		else
			file.write_line("player " + std::to_string(player_start_pos.x) + " " + std::to_string(player_start_pos.y));

		file.write_line("br " + std::to_string(braine));
		file.write_line("level_next " + level_next);

		for (int32_t y = 0; y < get_height(); y++) {
			std::string line = "";
			for (int32_t x = 0; x < get_width(); x++) {
				TileType tile = get(x, y);
				if (tile == TileType::BOUNDS) {
					line += '#';
					continue;
				}
				line += static_cast<size_t>(tile) + '0';
				line += ' ';
			}
			file.write_line(line);
		}

		if (not entities.empty()) {
			file.write_line("ENTITIES");

			for (auto &entity : entities) {
				EntityType type = entity->type();
				std::string position_string = std::to_string(entity->get_position().x) + " " + std::to_string(entity->get_position().y);

				switch (type) {
					case EntityType::EGG:
						file.write_line("EGG " + position_string);
						break;
					case EntityType::CHEST:
						file.write_line("CHEST " + position_string);
						break;
					case malaise::EntityType::ATONER:
						file.write_line("ATONER " + position_string);
						break;
					case malaise::EntityType::CHEST_VOID_ROD:
						file.write_line("VOID_ROD_CHEST " + position_string);
						break;
					default:
						break;
				}
			}
		}
	}

	void update(const float delta) {
		for (auto it = entities.begin(); it != entities.end(); ) {
			auto &entity = *it;

			entity->update(delta);

			const math::Vec2i entity_pos = entity->get_position();

			if (!entity->falling) {
				const auto pos = entity->get_position();

				if (get(pos.x, pos.y) == TileType::VOID) {
					entity->falling = true;
				}
			}

			if (entity->dead)
				it = entities.erase(it);
			else
				++it;
		}
	}

	void fill_ui_elements() {
		size_t y = height - 1;
		for (size_t x = 0; x < width; x++) {
			set(x, y, TileType::BLANK_WHITE);
		}
	}

	TileType get(const size_t x, const size_t y) const {
		if (!is_in_bounds(x, y)) return TileType::BOUNDS; // out of bounds

		return tiles[y * width + x];
	}

	void set(const size_t x, const size_t y, TileType type) {
		if (is_in_bounds(x, y))
			tiles[y * width + x] = type;
	}

	Entity* get_entity(const int32_t x, const int32_t y) const {
		math::Vec2i pos(x, y);
		for (auto &entity : entities) {
			if (entity->get_position() == pos)
				return entity.get();
		}

		return nullptr;
	}

	template<typename T, typename... Args>
    T& add_entity(Args&&... args) {
        auto entity =
            std::make_unique<T>(
                std::forward<Args>(args)...
            );

        T &ref = *entity;

        entities.push_back(
            std::move(entity)
        );

        return ref;
    }

	const std::vector<std::unique_ptr<Entity>>& get_entities() const {
		return entities;
	}

	std::vector<TileType> get_tiles() const {
		return tiles;
	}

	size_t get_width() const {
		return width;
	}

	size_t get_height() const {
		return height;
	}

	size_t get_size() const {
		return width * height;
	}

	math::Vec2i get_player_start_pos() const {
		return player_start_pos;
	}

	std::string get_level_next() const {
		return level_next;
	}

	std::string get_name() const {
		return name;
	}

	int get_braine() const {
		return braine;
	}
private:
	bool is_in_bounds(const size_t x, const size_t y) const {
		return x >= 0 && x < width and
			y >= 0 && y < height;
	}

	size_t width  = 0;
	size_t height = 0;

	std::string name{};
	int braine = 0;

	std::vector<TileType> tiles;
	std::vector<std::unique_ptr<Entity>> entities;

	math::Vec2i player_start_pos{};
	std::string level_next{};
};

}

#endif // !MALAISE_TILEMAP_HPP
