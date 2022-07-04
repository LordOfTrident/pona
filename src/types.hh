#ifndef TYPES_HH__HEADER_GUARD__
#define TYPES_HH__HEADER_GUARD__

#include <cstdlib> // std::size_t

struct Vec2D {
	Vec2D(int p_x = 0, int p_y = 0);

	int x, y;
}; // struct Vec2D

struct Vec2Dw {
	Vec2Dw(std::size_t p_x = 0, std::size_t p_y = 0);

	std::size_t x, y;
}; // struct Vec2D

#endif
