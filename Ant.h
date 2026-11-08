#pragma once
#include "Enemy.h"

/// <summary>
/// アリ：ランダムな方向に歩き回る。たまに立ち止まる。障害物はよけて歩く 
/// </summary>
class Ant : public Enemy
{
public:
	Ant(float x, float y);

	void Update(const Map& map, float px, float py, bool hidden) override;
	void Draw(float camX, float camY) const override;

private:
	float dx = 0, dy = 0;	// 歩く向き（長さ1） 
	float timer = 0;		// 今の動きを続けるフレーム数 
};
