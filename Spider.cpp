#include "Spider.h"
#include "Map.h"
#include "DrawUtil.h"

using namespace DrawUtil;

namespace
{
	const float CHASE_SPEED  = 255.0f / 60.0f;
	const float RETURN_SPEED = 95.0f / 60.0f;
	const int   CHASE_FRAME  = 84;		// 飛びかかる時間 1.4秒 
	const int   LOST_FRAME   = 36;		// 0.6秒見失ったらあきらめる 
	const int   RETURN_FRAME = 360;		// 戻る制限時間 6秒 
	const int   COOL_FRAME   = 72;		// 戻ったあとの休み 1.2秒 
}

Spider::Spider(float x, float y) : Enemy(x, y, 16, 210), homeX(x), homeY(y) {}

void Spider::Update(const Map& map, float px, float py, bool hidden)
{
	float ox = x, oy = y;
	bool seen = CanSee(px, py, hidden);

	if (state == PATROL) {			// 待ち伏せ 
		if (cool > 0) cool--;
		else if (seen) { state = CHASE; timer = CHASE_FRAME; lostFrame = 0; }
	}
	else if (state == CHASE) {		// 飛びかかり 
		MoveToward(px, py, CHASE_SPEED);
		map.ResolveCollision(x, y, r);
		timer--;
		if (seen) lostFrame = 0;
		else if (++lostFrame > LOST_FRAME) { state = RETURN; timer = RETURN_FRAME; }
		if (timer <= 0 && state == CHASE) { state = RETURN; timer = RETURN_FRAME; }
	}
	else {							// 戻り 
		float rem = MoveToward(homeX, homeY, RETURN_SPEED);
		map.ResolveCollision(x, y, r);
		timer--;
		if (rem < 4.0f || timer <= 0) {
			if (timer <= 0) { homeX = x; homeY = y; }	// 戻れなければ今の場所を新しい持ち場に 
			state = PATROL;
			cool = COOL_FRAME;
		}
	}

	if (Dist(ox, oy, x, y) > 0.05f) anim += 14.0f / 60.0f;
}

void Spider::Draw(float camX, float camY) const
{
	float sx = x - camX, sy = y - camY;

	// 影 
	Blend(60);
	FillEllipse(MakeXf(sx + 3, sy + 4, angle), 0, 4, 10, 14, GetColor(0, 0, 0));
	NoBlend();

	Transform f = MakeXf(sx, sy, angle);
	unsigned int col = GetColor(36, 28, 44);
	static const float spread[4] = { -10, -3, 5, 12 };
	// あし（左右4本ずつ・ひざで曲がる） 
	for (int side = -1; side <= 1; side += 2) {
		for (int k = 0; k < 4; k++) {
			float baseY = -4.0f + k * 3.0f;
			float sw = sinf(anim + k * 1.6f + (side > 0 ? PI : 0.0f)) * 3.0f;
			float kx = side * 13.0f, ky = baseY - 8.0f + spread[k] * 0.4f + sw * 0.5f;
			float fx = side * 23.0f, fy = baseY + spread[k] + sw;
			LineL(f, side * 3.0f, baseY, kx, ky, col, 2.0f);
			LineL(f, kx, ky, fx, fy, col, 2.0f);
		}
	}
	// おなか（赤い模様つき）・あたま・め 
	FillEllipse(f, 0, 9, 9, 11, GetColor(48, 36, 58));
	FillEllipse(f, 0, 9, 3, 5, GetColor(170, 40, 40));
	FillEllipse(f, 0, -4, 6.5f, 6.5f, GetColor(70, 52, 80));
	FillEllipse(f, -2.2f, -8.5f, 1.4f, 1.4f, GetColor(255, 255, 255));
	FillEllipse(f, 2.2f, -8.5f, 1.4f, 1.4f, GetColor(255, 255, 255));
}
