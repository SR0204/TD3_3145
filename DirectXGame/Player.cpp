#include "Player.h"

using namespace DirectX;

Player::Player(float startX, float startZ, float size, const Map& map) : hitbox(startX, startZ, size, size), map(map) {}

XMFLOAT3 Player::GetPosition() const { return hitbox.position; }

bool Player::CanMoveTo(float newX, float newZ) {
	float halfSize = hitbox.width / 2.0f;
	return !map.IsWall(newX - halfSize, newZ - halfSize) && !map.IsWall(newX + halfSize, newZ - halfSize) && !map.IsWall(newX - halfSize, newZ + halfSize) &&
	       !map.IsWall(newX + halfSize, newZ + halfSize);
}

void Player::Move(float deltaX, float deltaZ) {
	float newX = hitbox.position.x + deltaX;
	float newZ = hitbox.position.z + deltaZ;

	if (CanMoveTo(newX, hitbox.position.z)) {
		hitbox.position.x = newX;
	}
	if (CanMoveTo(hitbox.position.x, newZ)) {
		hitbox.position.z = newZ;
	}
}
