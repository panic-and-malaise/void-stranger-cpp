#include "entity_chest.hpp"
#include "player.hpp"
#include "util.hpp"
#include "vec2i.hpp"

using namespace malaise;

void entity::Chest::update(const float delta) {
	if (knock_timer >= 0.f) {
		knock_timer += delta;
	}
	if (knock_timer >= 3.f) {
		if (knocks == 3)
			lucky = true;
		knock_timer = -1.f;
		knocks = 0;
	}
}

void entity::Chest::draw(sf::RenderTarget &target, const tile::TileSet &set) const {
	util::render::draw_tile(
		target,
		opened ? tile::TileType::CHEST_OPEN : tile::TileType::CHEST_CLOSE,
		set,
		position
	);
}

bool entity::Chest::on_interact(Player &player)  {
	if (player.get_position_grid() != position + math::DOWN) return false;
	if (not opened) {
		opened = true;
		player.set_facing(math::DOWN);

		player.play_animation("idle_down");
		player.play_animation("item_get");

		player.play_effect("soul_glow", {
			static_cast<int32_t>(-util::TILE_SIZE_SCALED / 4),
			static_cast<int32_t>(-util::TILE_SIZE_SCALED * 1.5)
		});
		player.play_effect("locust", {
			static_cast<int32_t>(-util::TILE_SIZE_SCALED / 4 + 12),
			static_cast<int32_t>(-util::TILE_SIZE_SCALED * 1.5 + 12)
		});

		player.add_locusts(lucky ? 3 : 1);

		return true;
	}

	return false;
}

void entity::Chest::on_bump(Player &player, const math::Vec2i direction) {
	if (player.get_position_grid() != position + math::UP) return;

	knocks++;
	knock_timer = 0.f;
}
