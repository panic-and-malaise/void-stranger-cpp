#ifndef MALAISE_BITMAP_FONT_HPP
#define MALAISE_BITMAP_FONT_HPP

#include <SFML/Graphics/Color.hpp>
#include <sstream>
#include <string>
#include <unordered_map>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include "file_wrapper.hpp"
#include "util.hpp"

namespace malaise::font {

struct Glyph {
	sf::IntRect texture_rect;
	int advance = 0;
	int offset = 0;
};

class BitmapFont {
public:
    void add_glyph(const int c, const sf::IntRect rect) {
        glyphs[c] = {
			.texture_rect = rect
		};
    }

    const sf::IntRect& glyph_rect(const int c) const {
        return glyphs.at(c).texture_rect;
    }

	bool load_from_files(const std::string &bitmap_filename, const std::string &metadata_filename) {
		util::FileWrapper metadata(util::FONT_DIRECTORY + metadata_filename);

		if (!bitmap_texture.loadFromFile(util::SPRITE_DIRECTORY + bitmap_filename) or !metadata) return false;

		metadata.skip_line();

		std::string line;
		while (metadata.read_line(line)) {
			std::stringstream buffer(line);

			std::string token;

			std::vector<std::string> fields;

			while (std::getline(buffer, token, ';'))
				fields.push_back(token);

			int ascii = std::stoi(fields[0]);

			glyphs.emplace(
				ascii,
				Glyph {{
					std::stoi(fields[1]),
					std::stoi(fields[2]),
					std::stoi(fields[3]),
					std::stoi(fields[4])
					},
					std::stoi(fields[5]),
					std::stoi(fields[6])
				}
			);
		}

		return true;
	}

	void draw_text(sf::RenderTarget &target, const std::string &text, const sf::Vector2f position, const sf::Color color = sf::Color::Black) {
		sf::Sprite sprite(bitmap_texture);
		sprite.setColor(color);

		float cursor_x = position.x;

		for (unsigned char c : text) {
			auto it = glyphs.find(c);

			if (it == glyphs.end())
				continue;

			const Glyph& glyph = it->second;

			sprite.setTextureRect(
				glyph.texture_rect
			);

			sprite.setScale(util::SPRITE_SCALE, util::SPRITE_SCALE);

			sprite.setPosition(
				cursor_x + glyph.offset * util::SPRITE_SCALE,
				position.y
			);

			target.draw(sprite);

			cursor_x += glyph.advance * util::SPRITE_SCALE;
		}
	}

private:
    std::unordered_map<int, Glyph> glyphs;
	sf::Texture bitmap_texture;
};

}

#endif // !MALAISE_BITMAP_FONT_HPP
