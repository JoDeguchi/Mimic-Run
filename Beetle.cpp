#include "Beetle.h"
#include "Map.h"
#include "DrawUtil.h"

using namespace DrawUtil;

namespace
{
	const float PATROL_SPEED = 100.0f / 60.0f;
	const float CHASE_SPEED  = 190.0f / 60.0f;
	const float RETURN_SPEED = 150.0f / 60.0f;
	const int   LOST_FRAME   = 90;				// 1.5秒見失ったら戻る 
}

Beetle::Beetle(bool horizontal, float fixedPos, float minV, float maxV, float dir)
	: Enemy(0, 0, 14, 170), horiz(horizontal), fixedPos(fixedPos), minV(minV), maxV(maxV), dir(dir)
{
	// 往復範囲のランダムな位置からスタート 
	float v = Randomfloat(minV, maxV);
	if (horiz) { x = v; y = fixedPos; }
	else       { x = fixedPos; y = v; }
}

void Beetle::Update(const Map& map, float px, float py, bool hidden)
{
	anim += 14.0f / 60.0f;
	bool seen = CanSee(px, py, hidden);

	if (state == PATROL) {
		if (horiz) {
			x += dir * PATROL_SPEED;
			if (x > maxV) { x = maxV; dir = -1; }
			if (x < minV) { x = minV; dir = 1; }
			angle = dir > 0 ? PI * 0.5f : -PI * 0.5f;
		} else {
			y += dir * PATROL_SPEED;
			if (y > maxV) { y = maxV; dir = -1; }
			if (y < minV) { y = minV; dir = 1; }
			angle = dir > 0 ? PI : 0.0f;
		}
		if (seen) { state = CHASE; lostFrame = 0; }
	}
	else if (state == CHASE) {
		MoveToward(px, py, CHASE_SPEED);		// 飛んでいるので障害物は無視 
		if (seen) lostFrame = 0;
		else if (++lostFrame > LOST_FRAME) state = RETURN;
	}
	else {	// RETURN：往復する直線へ戻る 
		float tx = horiz ? Clampf(x, minV, maxV) : fixedPos;
		float ty = horiz ? fixedPos : Clampf(y, minV, maxV);
		float rem = MoveToward(tx, ty, RETURN_SPEED);
		if (rem < 3.0f) state = PATROL;
		if (seen) { state = CHASE; lostFrame = 0; }
	}

	x = Clampf(x, 0, map.GetWidth());
	y = Clampf(y, 0, map.GetHeight());
}

void Beetle::DrawShadow(float camX, float camY) const
{
	Blend(55);
	FillEllipse(MakeXf(x - camX + 6, y - camY + 22, angle), 0, 0, 9, 13, GetColor(0, 0, 0));
	NoBlend();
}

void Beetle::Draw(float camX, float camY) const
{
	Transform f = MakeXf(x - camX, y - camY, angle);
	float flap = sinf(anim * 2.5f) * 0.5f + 0.5f;
	unsigned int dark = GetColor(20, 50, 30);

	// あし 
	for (int k = 0; k < 3; k++) {
		float ly = -6.0f + 6.0f * k;
		LineL(f, -7, ly, -14, ly + (k - 1) * 3.0f, dark, 1.5f);
		LineL(f, 7, ly, 14, ly + (k - 1) * 3.0f, dark, 1.5f);
	}
	// はね（はばたき） 
	Blend(120);
	FillEllipse(f, -11, 1, 3.0f + flap * 3.0f, 12, GetColor(220, 235, 240));
	FillEllipse(f, 11, 1, 3.0f + flap * 3.0f, 12, GetColor(220, 235, 240));
	NoBlend();
	// からだ・あたま・つの 
	FillEllipse(f, 0, 2, 9.5f, 14, GetColor(35, 130, 70));
	FillEllipse(f, -2.5f, 0, 3.0f, 8, GetColor(110, 200, 120));
	LineL(f, 0, -9, 0, 16, GetColor(20, 80, 40), 1.5f);
	FillEllipse(f, 0, -14, 6, 5, GetColor(25, 60, 35));
	LineL(f, -3, -17, -7, -25, dark, 1.5f);
	LineL(f, 3, -17, 7, -25, dark, 1.5f);
}
