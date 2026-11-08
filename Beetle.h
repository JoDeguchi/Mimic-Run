#pragma once
#include "Enemy.h"

/// <summary>
/// カナブン：決まった直線を縦または横に往復する。飛んでいるので障害物を越える 
/// </summary>
class Beetle : public Enemy
{
public:
	// horizontal = true なら横に往復（fixedPos は y 座標）、false なら縦（fixedPos は x 座標） 
	// minV〜maxV が往復する範囲、dir は最初の向き（+1 / -1） 
	Beetle(bool horizontal, float fixedPos, float minV, float maxV, float dir);

	void Update(const Map& map, float px, float py, bool hidden) override;
	void Draw(float camX, float camY) const override;
	void DrawShadow(float camX, float camY) const override;
	bool IsFlying() const override { return true; }

private:
	bool  horiz;
	float fixedPos;
	float minV, maxV;
	float dir;
};
