#ifndef MALAISE_TILEMAP_HPP
#define MALAISE_TILEMAP_HPP

#include <iostream>
#include <string>
#include <vector>

#include <SFML/Graphics/RenderTarget.hpp>

#include "file_wrapper.hpp"
#include "tile.hpp"
#include "util.hpp"

namespace malaise::tile {

class TileMap {
public:
	TileMap(const size_t width_, const size_t height_) : width(width_), height(height_), tiles(width * height, TileType::VOID) {
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

		tiles.clear();

		file.get() >> width >> height;

		char next_tile;
		while (file.get() >> next_tile) {
			if (next_tile == '#') {
				tiles.push_back(TileType::BOUNDS);
				continue;
			}

			TileType index = static_cast<TileType>(next_tile - '0');
			tiles.push_back(index);
		}
	}

	void export_to_file(const std::string &filename) {
		util::FileWrapper file(util::LEVEL_DIRECTORY + filename, std::ios::out);

		if (!file) {
			std::cerr << "Error loading " << filename << "!\n";
			return;
		}

		file.write_line(std::to_string(width) + " " + std::to_string(height));
		for (int32_t y = 0; y < get_height(); y++) {
			std::string line = "";
			for (int32_t x = 0; x < get_width(); x++) {
				TileType tile = get(x, y);
				if (tile == TileType::BOUNDS) {
					line += '#';
					line += ' ';
					continue;
				}
				line += static_cast<size_t>(tile) + '0';
				line += ' ';
			}
			file.write_line(line);
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
private:
	bool is_in_bounds(const size_t x, const size_t y) const {
		return x >= 0 && x < width and
			y >= 0 && y < height;
	}

	size_t width  = 0;
	size_t height = 0;
	std::vector<TileType> tiles;
};

}

#endif // !MALAISE_TILEMAP_HPP
