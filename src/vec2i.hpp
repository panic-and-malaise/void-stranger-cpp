#ifndef MALAISE_VEC2I_HPP
#define MALAISE_VEC2I_HPP

#include <cstddef>
#include <cstdint>

#include <SFML/System/Vector2.hpp>

namespace malaise::math {

struct Vec2i {
	Vec2i() = default;
	Vec2i(const int32_t x_, const int32_t y_) : x(x_), y(y_) {}
	Vec2i(const sf::Vector2i vec) : x(vec.x), y(vec.y) {}
	Vec2i(const sf::Vector2f vec) : x(vec.x), y(vec.y) {}

	int32_t x{}, y{};

	Vec2i operator+(const Vec2i& other) const;
	Vec2i operator-(const Vec2i& other) const;
	void operator+=(const Vec2i& other);
	Vec2i operator*(const int scalar) const;
	Vec2i operator*(const size_t scalar) const;

	bool operator==(const Vec2i& v) const;
	bool operator<(const Vec2i& other) const;

	void normalize();

	float length() const;
	float length_squared() const;

	float distance(const Vec2i &b);
	float distance_squared(const Vec2i &b);

};
struct Vec2iHash {
	size_t operator()(const Vec2i& v) const;
};

static const Vec2i ZERO  = { 0, 0 };
static const Vec2i UP 	 = { 0, -1 };
static const Vec2i DOWN  = { 0, 1 };
static const Vec2i LEFT  = { -1, 0 };
static const Vec2i RIGHT = { 1, 0 };

}

#endif // !MALAISE_VEC2I_HPP
