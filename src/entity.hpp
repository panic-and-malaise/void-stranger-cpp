#ifndef MALAISE_ENTITY_HPP
#define MALAISE_ENTITY_HPP

#include <SFML/Graphics/RenderTarget.hpp>

#include "tile_type.hpp"
#include "tileset.hpp"
#include "vec2i.hpp"
#include "render_util.hpp"

namespace malaise {

class Player; // I hate you C++ I hate you I hate you I hate you

enum class EntityType {
	EGG = 0,
	CHEST
};

class Entity {
public:
	Entity(const math::Vec2i position_) : position(position_) {}
	virtual ~Entity() = default;

	virtual void update(const float delta) {}
	virtual void draw(sf::RenderTarget &target, const tile::TileSet &set) const {}

	virtual void on_interact(Player &player) {}
	virtual void on_bump(Player &player, const math::Vec2i direction) {}

	virtual EntityType type() const = 0;
	math::Vec2i get_position() const { return position; }
protected:
	math::Vec2i position;
};

class Chest : public Entity {
public:
	Chest(const math::Vec2i position_) : Entity(position_) {}

	void draw(sf::RenderTarget &target, const tile::TileSet &set) const override {
		util::render::draw_tile(target, tile::TileType::CHEST_CLOSE, set, position);
	}

	EntityType type() const override {
		return EntityType::CHEST;
	}

	bool opened = false;
private:
};

class Egg : public Entity {
public:
	Egg(const math::Vec2i position_) : Entity(position_) {}

	void draw(sf::RenderTarget &target, const tile::TileSet &set) const override {
		util::render::draw_tile(target, tile::TileType::EGG, set, position);
	}

	EntityType type() const override {
		return EntityType::EGG;
	}

	void on_bump(Player &player, const math::Vec2i direction) override {
		position += direction;
	}
};

}

#endif // !MALAISE_ENTITY_HPP
