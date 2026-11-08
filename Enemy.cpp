#include "Enemy.h"
#include "DrawUtil.h"

using namespace DrawUtil;

bool Enemy::CanSee(float px, float py, bool hidden) const
{
	return !hidden && Dist(x, y, px, py) < vision;
}

float Enemy::MoveToward(float tx, float ty, float speed)
{
	float dx = tx - x, dy = ty - y;
	float d = sqrtf(dx * dx + dy * dy);
	if (d < 0.001f) return 0;
	float step = speed < d ? speed : d;		// 行き過ぎない 
	x += dx / d * step;
	y += dy / d * step;
	angle = atan2f(dx, -dy);
	return d - step;
}

bool Enemy::HitPlayer(float px, float py, float pr) const
{
	return Dist(x, y, px, py) < pr + r * 0.8f;
}

void Enemy::DrawVision(float camX, float camY) const
{
	float sx = x - camX, sy = y - camY;
	Blend(state == CHASE ? 40 : 18);
	DrawCircleAA(sx, sy, vision, 48, GetColor(255, 60, 40), TRUE);
	Blend(70);
	DrawCircleAA(sx, sy, vision, 48, GetColor(255, 80, 60), FALSE, 1.0f);
	NoBlend();
}

void Enemy::DrawAlert(float camX, float camY) const
{
	if (state != CHASE) return;
	float sx = x - camX, sy = y - camY;
	unsigned int col = GetColor(255, 70, 50);
	DrawLineAA(sx, sy - 66, sx, sy - 50, col, 5.0f);
	DrawCircleAA(sx, sy - 42, 3.0f, 12, col, TRUE);
}
