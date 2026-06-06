#ifndef MALAISE_ENTITY_HPP
#define MALAISE_ENTITY_HPP

#include <SFML/Graphics/RenderTarget.hpp>

namespace malaise {

class Entity {
public:
	virtual void update(const float delta) {}
	virtual void draw(sf::RenderTarget &target) {}

	virtual ~Entity() = default;
private:
};

class Chest : public Entity {
public:
	bool opened = false;
private:
};

}

#endif // !MALAISE_ENTITY_HPP
