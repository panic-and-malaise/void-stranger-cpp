#ifndef MALAISE_CHEST_VOID_ROD_ENTITY_HPP
#define MALAISE_CHEST_VOID_ROD_ENTITY_HPP

#include "entity.hpp"
#include "vec2i.hpp"

namespace malaise::entity {

class ChestVoidRod : public Entity {
public:
	ChestVoidRod(const math::Vec2i position_) : Entity(position_) {}

	void draw(sf::RenderTarget &target, const tile::TileSet &set) const override;

	EntityType type() const override {
		return EntityType::CHEST_VOID_ROD;
	}

	bool on_interact(Player &player) override;
private:
	bool opened = false;
};

}

#endif // !MALAISE_CHEST_VOID_ROD_ENTITY_HPP
