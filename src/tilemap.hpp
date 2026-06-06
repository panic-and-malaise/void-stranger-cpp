#ifndef MALAISE_TILEMAP_HPP
#define MALAISE_TILEMAP_HPP

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <SFML/Graphics/RenderTarget.hpp>

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

		std::string temp;

		file.get() >> temp >> width >> height;
		file.get() >> temp >> player_start_pos.x >> player_start_pos.y;
		file.get() >> temp >> braine;
		file.get() >> temp >> level_next;

		char next_tile;
		while (file.get() >> next_tile) {
			if (next_tile == '#') {
				tiles.push_back(TileType::BOUNDS);
				continue;
			}

			TileType index = static_cast<TileType>(next_tile - '0');
			tiles.push_back(index);
		}

		fill_ui_elements();
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
