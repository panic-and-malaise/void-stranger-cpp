#ifndef MALAISE_IRIS_TRANSITION_HPP
#define MALAISE_IRIS_TRANSITION_HPP

#include "vec2i.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
namespace malaise::animation {

class SquareIrisTransition {
public:
    void start_close(const sf::Vector2f center_) {
        center = center_;

        state = State::Closing;
        half_size = max_half_size;
    }

    void start_open(const sf::Vector2f center_) {
        center = center_;

        state = State::Opening;
        half_size = 0.f;
    }

    void update(float delta) {
        switch (state) {
			case State::Idle:
				break;

			case State::Closing:
				half_size -= speed * delta;

				if (half_size <= 0.f)
				{
					half_size = 0.f;
					state = State::Closed;
				}
				break;

			case State::Opening:
				half_size += speed * delta;

				if (half_size >= max_half_size)
				{
					half_size = max_half_size;
					state = State::Idle;
				}
				break;

			case State::Closed:
				break;
			}
    }

    void draw(sf::RenderTarget& target) {
        if (state == State::Idle)
            return;

        float left   = center.x - half_size;
        float right  = center.x + half_size;
        float top    = center.y - half_size;
        float bottom = center.y + half_size;

        auto size = target.getSize();

        left   = std::max(0.f, left);
        right  = std::min(static_cast<float>(size.x), right);

        top    = std::max(0.f, top);
        bottom = std::min(static_cast<float>(size.y), bottom);

        sf::RectangleShape rect;
        rect.setFillColor(sf::Color::Black);

        // Top
        rect.setPosition(0.f, 0.f);
        rect.setSize({static_cast<float>(size.x), top});
        target.draw(rect);

        // Bottom
        rect.setPosition(0.f, bottom);
        rect.setSize({
            static_cast<float>(size.x),
            static_cast<float>(size.y) - bottom
        });
        target.draw(rect);

        // Left
        rect.setPosition(0.f, top);
        rect.setSize({
            left,
            bottom - top
        });
        target.draw(rect);

        // Right
        rect.setPosition(right, top);
        rect.setSize({
            static_cast<float>(size.x) - right,
            bottom - top
        });
        target.draw(rect);
    }

    bool is_closed() const {
        return state == State::Closed;
    }

    bool is_active() const {
        return state != State::Idle;
    }

private:
    enum class State {
        Idle,
        Closing,
        Closed,
        Opening
    };

    State state = State::Idle;

	math::Vec2i center;

    float half_size = 0.f;

    // Bigger than half the screen diagonal
    float max_half_size = 600.f;

    float speed = 700.f;
};

}

#endif // !MALAISE_IRIS_TRANSITION_HPP
