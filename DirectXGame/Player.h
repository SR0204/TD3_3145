#ifndef PLAYER_H
#define PLAYER_H

#include "Map.h"
#include "Rectangle.h"

class Player {
private:
	class Rectangle hitbox;
	const Map& map;

public:
	Player(float startX, float startZ, float size, const Map& map);

	DirectX::XMFLOAT3 GetPosition() const;
	bool CanMoveTo(float newX, float newZ);
	void Move(float deltaX, float deltaZ);
};

#endif // PLAYER_H
