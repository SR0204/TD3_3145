#include "Rectangle.h"

using namespace DirectX;

Rectangle::Rectangle(float x, float z, float width, float depth) : position(x, 0.0f, z), width(width), depth(depth) {}

bool Rectangle::IsColliding(const Rectangle& other) const {
	return (position.x + width > other.position.x && position.x < other.position.x + other.width && position.z + depth > other.position.z && position.z < other.position.z + other.depth);
}
