#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <DirectXMath.h>

class Rectangle {
public:
	DirectX::XMFLOAT3 position;
	float width, depth;

	Rectangle(float x, float z, float width, float depth);

	bool IsColliding(const Rectangle& other) const;
};

#endif // RECTANGLE_H
