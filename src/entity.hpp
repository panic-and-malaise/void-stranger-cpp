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
	EGG = static_cast<int>(tile::TileType::EGG),
	CHEST = static_cast<int>(tile::TileType::CHEST_CLOSE),
	ATONER = static_cast<int>(tile::TileType::ATONER),
	CHEST_VOID_ROD = static_cast<int>(tile::TileType::CHEST_BIG_CLOSE),
	COUNT
};

class Entity {
public:
	Entity(const math::Vec2i position_) : position(position_) {}
	virtual ~Entity() = default;

	virtual void update(const float delta) {
		if (falling) {
			fall_timer += delta;

			if (fall_timer > 0.2f) {
				dead = true;
			}
		}
	}
	virtual void draw(sf::RenderTarget &target, const tile::TileSet &set) const {}

	virtual bool on_interact(Player &player) { return false; }
	virtual void on_bump(Player &player, const math::Vec2i direction) {}

	virtual EntityType type() const = 0;

	math::Vec2i get_position() const { return position; }

	bool falling = false;
	bool dead = false;

	float fall_timer = 0.f;
	static constexpr float FALL_DURATION = 2.f;
protected:
	math::Vec2i position;
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

class Atoner : public Entity {
public:
	Atoner(const math::Vec2i position_) : Entity(position_) {}

	void draw(sf::RenderTarget &target, const tile::TileSet &set) const override {
		util::render::draw_tile(target, tile::TileType::ATONER, set, position);
	}

	EntityType type() const override {
		return EntityType::ATONER;
	}

	void on_bump(Player &player, const math::Vec2i direction) override {
		position += direction;
	}
};

}

#endif // !MALAISE_ENTITY_HPP
