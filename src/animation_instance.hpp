#ifndef MALAISE_ANIMATION_INSTANCE
#define MALAISE_ANIMATION_INSTANCE

#include "vec2i.hpp"
#include "sprite_animation.hpp"

namespace malaise::animation {

struct AnimationInstance {
	const SpriteAnimation *animation = nullptr;

	size_t current_frame = 0;
	float timer = 0.f;
	float rotation = 0.f;

	bool finished = false;
	math::Vec2i offset{};
};
	
}

#endif // !MALAISE_ANIMATION_INSTANCE
