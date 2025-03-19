#include<Vector3.h>
#include"Map.h"
class Player {
public:
	Player(float x, float z, float size, Map& map);

	void Move(float dx, float dz);
	Vector3 GetPosition() const { return position_; }

	 // 位置を設定するためのメソッド
	void SetPosition(const Vector3& position);

private:
	Vector3 position_;
	float size_; // プレイヤーの大きさ（ヒットボックス）
	Map& map_;   // マップ情報への参照

	bool CanMoveTo(float newX, float newZ); // 目的地に移動できるか判定
};
