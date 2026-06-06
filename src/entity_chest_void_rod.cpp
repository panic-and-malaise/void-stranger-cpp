#include "entity_chest_void_rod.hpp"
#include "player.hpp"
#include "util.hpp"
#include "vec2i.hpp"

using namespace malaise;

void entity::ChestVoidRod::draw(sf::RenderTarget &target, const tile::TileSet &set) const {
	util::render::draw_tile(
		target,
		opened ? tile::TileType::CHEST_BIG_OPEN : tile::TileType::CHEST_BIG_CLOSE,
		set,
		position
	);
}

bool entity::ChestVoidRod::on_interact(Player &player)  {
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

		player.give_void_rod();

		return true;
	}

	return false;
}
