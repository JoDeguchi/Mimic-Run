#include "EnemyManager.h"
#include "Map.h"
#include "Beetle.h"
#include "Ant.h"
#include "Spider.h"
#include "DrawUtil.h"

using namespace DrawUtil;

void EnemyManager::Build(int stage, const Map& map)
{
	enemies.clear();
	SRand(5678 + stage * 131);		// 敵の配置も毎回同じにする 

	int nBeetle, nAnt, nSpider, nGuard;		// nGuard = ゴール付近を守るクモの数 
	if (stage == 1) { nBeetle = 3; nAnt = 5; nSpider = 2; nGuard = 1; }
	else            { nBeetle = 5; nAnt = 8; nSpider = 4; nGuard = 2; }

	float mapW = map.GetWidth(), mapH = map.GetHeight();
	float startX = map.GetStartX(), startY = map.GetStartY();
	float goalX = map.GetGoalX(), goalY = map.GetGoalY();

	// ---- カナブン：縦横の直線を往復（スタート直後に当たらない位置のみ）---- 
	for (int i = 0; i < nBeetle; i++) {
		for (int k = 0; k < 100; k++) {
			bool horiz = (GetRand(1) == 0);
			float dir = (GetRand(1) == 0) ? 1.0f : -1.0f;
			float cx = Randomfloat(200, mapW - 200.0f), cy = Randomfloat(200, mapH - 200.0f), half = Randomfloat(150, 320);
			float fixedPos, minV, maxV, rx, ry;
			if (horiz) {
				fixedPos = cy;
				minV = Clampf(cx - half, 100, mapW - 100.0f); maxV = Clampf(cx + half, 100, mapW - 100.0f);
				rx = Clampf(startX, minV, maxV); ry = cy;
			} else {
				fixedPos = cx;
				minV = Clampf(cy - half, 100, mapH - 100.0f); maxV = Clampf(cy + half, 100, mapH - 100.0f);
				rx = cx; ry = Clampf(startY, minV, maxV);
			}
			if (Dist(rx, ry, startX, startY) < 380) continue;	// スタートに近すぎる 
			enemies.push_back(std::unique_ptr<Enemy>(new Beetle(horiz, fixedPos, minV, maxV, dir)));
			break;
		}
	}
	// ---- アリ：障害物の中・スタート付近を避けて配置 ---- 
	for (int i = 0; i < nAnt; i++) {
		for (int k = 0; k < 100; k++) {
			float ex = Randomfloat(150, mapW - 150.0f), ey = Randomfloat(150, mapH - 150.0f);
			if (Dist(ex, ey, startX, startY) < 380 || map.IsInsideObstacle(ex, ey, 10)) continue;
			enemies.push_back(std::unique_ptr<Enemy>(new Ant(ex, ey)));
			break;
		}
	}
	// ---- クモ：一部はゴール付近を守る ---- 
	for (int i = 0; i < nSpider; i++) {
		for (int k = 0; k < 120; k++) {
			float ex, ey;
			if (i < nGuard) {
				float a = Randomfloat(0, PI2), d = Randomfloat(200, 320);
				ex = goalX + cosf(a) * d; ey = goalY + sinf(a) * d;
				if (ex < 120 || ey < 120 || ex > mapW - 120.0f || ey > mapH - 120.0f) continue;
			} else {
				ex = Randomfloat(150, mapW - 150.0f); ey = Randomfloat(150, mapH - 150.0f);
				if (Dist(ex, ey, startX, startY) < 450 || Dist(ex, ey, goalX, goalY) < 100) continue;
			}
			if (map.IsInsideObstacle(ex, ey, 16)) continue;
			enemies.push_back(std::unique_ptr<Enemy>(new Spider(ex, ey)));
			break;
		}
	}

	SRand(GetNowCount());			// ゲーム中の乱数はランダムに戻す 
}

void EnemyManager::Update(const Map& map, float px, float py, bool hidden)
{
	for (size_t i = 0; i < enemies.size(); i++) enemies[i]->Update(map, px, py, hidden);
}

bool EnemyManager::HitPlayer(float px, float py, float pr) const
{
	for (size_t i = 0; i < enemies.size(); i++)
		if (enemies[i]->HitPlayer(px, py, pr)) return true;
	return false;
}

void EnemyManager::DrawGround(float camX, float camY) const
{
	for (size_t i = 0; i < enemies.size(); i++) enemies[i]->DrawVision(camX, camY);
	for (size_t i = 0; i < enemies.size(); i++) enemies[i]->DrawShadow(camX, camY);
	for (size_t i = 0; i < enemies.size(); i++)
		if (!enemies[i]->IsFlying()) enemies[i]->Draw(camX, camY);
}

void EnemyManager::DrawAir(float camX, float camY) const
{
	for (size_t i = 0; i < enemies.size(); i++)
		if (enemies[i]->IsFlying()) enemies[i]->Draw(camX, camY);
	for (size_t i = 0; i < enemies.size(); i++) enemies[i]->DrawAlert(camX, camY);
}
