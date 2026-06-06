#ifndef MALAISE_CHEST_ENTITY_HPP
#define MALAISE_CHEST_ENTITY_HPP

#include "entity.hpp"
#include "vec2i.hpp"

namespace malaise::entity {

class Chest : public Entity {
public:
	Chest(const math::Vec2i position_) : Entity(position_) {}

	void update(const float delta) override;
	void draw(sf::RenderTarget &target, const tile::TileSet &set) const override;

	EntityType type() const override {
		return EntityType::CHEST;
	}

	bool on_interact(Player &player) override;
	void on_bump(Player &player, const math::Vec2i direction) override;
private:
	bool opened = false;
	bool lucky = false;
	int knocks = 0;
	float knock_timer = -1.f;
};

}

#endif // !MALAISE_CHEST_ENTITY_HPP
