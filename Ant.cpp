#include "Ant.h"
#include "Map.h"
#include "DrawUtil.h"

using namespace DrawUtil;

namespace
{
	const float PATROL_SPEED = 70.0f / 60.0f;
	const float CHASE_SPEED  = 115.0f / 60.0f;
	const int   LOST_FRAME   = 72;				// 1.2秒見失ったら巡回に戻る 
}

Ant::Ant(float x, float y) : Enemy(x, y, 10, 125) {}

void Ant::Update(const Map& map, float px, float py, bool hidden)
{
	float ox = x, oy = y;
	bool seen = CanSee(px, py, hidden);

	if (state == PATROL) {
		if (--timer <= 0) {
			if (GetRand(4) == 0) {			// 5回に1回は立ち止まる 
				dx = dy = 0;
				timer = Rf(30, 72);
			} else {						// ランダムな向きへ歩く 
				float a = Rf(0, PI2);
				dx = cosf(a); dy = sinf(a);
				timer = Rf(60, 150);
			}
		}
		if (dx != 0 || dy != 0) {
			x += dx * PATROL_SPEED;
			y += dy * PATROL_SPEED;
			angle = atan2f(dx, -dy);
			if (map.ResolveCollision(x, y, r)) timer = 0;	// ぶつかったら向きを選び直す 
		}
		if (seen) { state = CHASE; lostFrame = 0; }
	}
	else {	// CHASE
		MoveToward(px, py, CHASE_SPEED);
		map.ResolveCollision(x, y, r);
		if (seen) lostFrame = 0;
		else if (++lostFrame > LOST_FRAME) { state = PATROL; timer = 0; }
	}

	if (Dist(ox, oy, x, y) > 0.05f) anim += 14.0f / 60.0f;	// 動いたときだけ足を動かす 
}

void Ant::Draw(float camX, float camY) const
{
	float sx = x - camX, sy = y - camY;

	// 影 
	Blend(60);
	FillEllipse(MakeXf(sx + 2, sy + 3, angle), 0, 0, 6, 10, GetColor(0, 0, 0));
	NoBlend();

	Transform f = MakeXf(sx, sy, angle);
	unsigned int col = GetColor(28, 22, 22);
	// あし 
	for (int k = 0; k < 3; k++) {
		float ly = -2.0f + 4.0f * k;
		float sw = sinf(anim + k * 2.0f) * 3.0f;
		LineL(f, -3, ly, -11, ly + sw, col, 1.5f);
		LineL(f, 3, ly, 11, ly - sw, col, 1.5f);
	}
	// おしり・むね・あたま 
	FillEllipse(f, 0, 9, 5.5f, 7.5f, GetColor(40, 30, 30));
	FillEllipse(f, 0, -1, 3.5f, 4.5f, GetColor(62, 46, 44));
	FillEllipse(f, 0, -8.5f, 4.5f, 4.5f, GetColor(52, 40, 40));
	// しょっかく 
	LineL(f, -2, -11, -6, -17, col, 1.2f);
	LineL(f, 2, -11, 6, -17, col, 1.2f);
}
