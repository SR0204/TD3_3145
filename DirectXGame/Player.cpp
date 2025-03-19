#include "Player.h"

Player::Player(float x, float z, float size, Map& map) : position_{x, 0.0f, z}, size_(size), map_(map) {}

void Player::Move(float dx, float dz) {
	// 移動先を計算
	float newX = position_.x + dx;
	float newZ = position_.z + dz;

	// 移動先のマスがブロックでなければ移動
	if (CanMoveTo(newX, newZ)) {
		position_.x = newX;
		position_.z = newZ;
	}
}

void Player::SetPosition(const Vector3& position) { position_ = position; }

bool Player::CanMoveTo(float newX, float newZ) {
	// 移動先のマップ上のインデックスを計算
	uint32_t mapX = static_cast<uint32_t>(newX); // 小数点以下切り捨て
	uint32_t mapZ = static_cast<uint32_t>(newZ);

	// マップ範囲外なら移動不可
	if (mapX >= map_.GetWidth() || mapZ >= map_.GetHeight()) {
		return false;
	}

	// 移動先がブロックなら移動不可
	if (map_.GetMapChipTypeByIndex(mapX, mapZ) == MapChipType::kBlock) {
		return false;
	}

	return true; // それ以外なら移動OK
}
