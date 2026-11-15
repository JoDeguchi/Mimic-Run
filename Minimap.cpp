#include "Minimap.h"
#include "Map.h"
#include "EnemyManager.h"
#include "Player.h"
#include "DrawUtil.h"
#include <cstring>

using namespace DrawUtil;

namespace
{
	const int   SCREEN_W = 1280;
	const int   SCREEN_H = 720;
	const float MAP_W = 320.0f;		// ミニマップの幅（高さはマップの縦横比から決まる） 
	const float MARGIN = 12.0f;		// 画面の端からの余白 

	// 星を描く（ゴールの目印）。中心 (cx,cy)、外側の半径 R 
	void DrawStar(float cx, float cy, float R, unsigned int fill, unsigned int edge)
	{
		float sx[10], sy[10];
		for (int i = 0; i < 10; i++) {
			float a = -PI * 0.5f + i * PI / 5.0f;			// 真上から時計回りに10点 
			float r = (i % 2 == 0) ? R : R * 0.45f;			// 外側の点と内側の点を交互に 
			sx[i] = cx + cosf(a) * r;
			sy[i] = cy + sinf(a) * r;
		}
		for (int i = 0; i < 10; i++) {
			int j = (i + 1) % 10;
			DrawTriangleAA(cx, cy, sx[i], sy[i], sx[j], sy[j], fill, TRUE);
		}
		for (int i = 0; i < 10; i++) {
			int j = (i + 1) % 10;
			DrawLineAA(sx[i], sy[i], sx[j], sy[j], edge, 1.5f);
		}
	}
}

float Minimap::GetBottom(const Map& map) const
{
	float h = map.GetHeight() * (MAP_W / map.GetWidth());
	float y0 = atTop ? MARGIN : SCREEN_H - h - MARGIN;
	return y0 + h;
}

void Minimap::Draw(const Map& map, const EnemyManager& enemies, const Player& player,
	float camX, float camY) const
{
	float k = MAP_W / map.GetWidth();				// 縮尺（ワールド1pxがミニマップで何pxか） 
	float w = MAP_W;
	float h = map.GetHeight() * k;
	float x0 = SCREEN_W - w - MARGIN;				// 右端にそろえる 
	float y0 = atTop ? MARGIN : SCREEN_H - h - MARGIN;	// 右上 or 右下 

	float t = GetNowCount() / 1000.0f;
	unsigned int gold = GetColor(255, 225, 90);

	// ---- ① 板と地形 ---- 
	Blend(170);
	DrawBox((int)x0, (int)y0, (int)(x0 + w), (int)(y0 + h), GetColor(10, 24, 10), TRUE);
	NoBlend();
	map.DrawMinimapTerrain(x0, y0, k);

	// プレイヤーとゴールのミニマップ上の位置 
	float px = x0 + player.GetX() * k, py = y0 + player.GetY() * k;
	float gx = x0 + map.GetGoalX() * k, gy = y0 + map.GetGoalY() * k;

	// ---- ② プレイヤーからゴールへのてんせん ---- 
	{
		float dx = gx - px, dy = gy - py;
		float len = sqrtf(dx * dx + dy * dy);
		if (len > 14.0f) {
			float ux = dx / len, uy = dy / len;
			Blend(190);
			for (float d = 6.0f; d < len - 10.0f; d += 8.0f) {		// 4px 描いて 4px 空ける 
				float d2 = fminf(d + 4.0f, len - 10.0f);
				DrawLineAA(px + ux * d, py + uy * d, px + ux * d2, py + uy * d2, gold, 1.5f);
			}
			NoBlend();
		}
	}

	// ---- ③ 敵（赤い点。追跡中は大きく明るく）---- 
	for (size_t i = 0; i < enemies.Count(); i++) {
		const Enemy& e = enemies.Get(i);
		float ex = x0 + e.GetX() * k, ey = y0 + e.GetY() * k;
		if (e.GetState() == Enemy::CHASE) DrawCircleAA(ex, ey, 3.5f, 10, GetColor(255, 90, 70), TRUE);
		else                               DrawCircleAA(ex, ey, 2.2f, 8, GetColor(220, 50, 40), TRUE);
	}

	// ---- ④ ゴール（一番目立たせる）---- 
	// 広がる輪を2つ、半周期ずらして出す 
	for (int n = 0; n < 2; n++) {
		float cyc = fmodf(t * 0.9f + n * 0.5f, 1.0f);				// 0→1 
		Blend((int)(220 * (1.0f - cyc)));
		DrawCircleAA(gx, gy, 6.0f + cyc * 9.0f, 24, gold, FALSE, 2.0f);
		NoBlend();
	}
	// 脈打つ星 
	DrawStar(gx, gy, 8.0f + 1.5f * sinf(t * 6.0f), gold, GetColor(60, 40, 0));
	// 「GOAL」の文字（星の左に。ミニマップの外にはみ出さないように位置を調整） 
	{
		const char* label = "GOAL";
		int tw = GetDrawStringWidth(label, (int)strlen(label));
		float lx = gx - 14.0f - tw;
		float ly = gy - 8.0f;
		if (lx < x0 + 2.0f) lx = gx + 14.0f;						// 左に入らなければ右に 
		if (ly < y0 + 2.0f) ly = y0 + 2.0f;
		if (ly > y0 + h - 18.0f) ly = y0 + h - 18.0f;
		DrawString((int)lx + 1, (int)ly + 1, label, GetColor(0, 0, 0));
		DrawString((int)lx, (int)ly, label, gold);
	}

	// ---- ⑤ プレイヤー（白い点＋小さく脈打つ輪）---- 
	Blend(160);
	DrawCircleAA(px, py, 5.0f + sinf(t * 8.0f), 16, GetColor(255, 255, 255), FALSE, 1.0f);
	NoBlend();
	DrawCircleAA(px, py, 3.2f, 12, GetColor(255, 255, 255), TRUE);

	// ---- ⑥ 今の画面の範囲（白い枠）と、板のふち ---- 
	DrawBox((int)(x0 + camX * k), (int)(y0 + camY * k),
		(int)(x0 + (camX + SCREEN_W) * k), (int)(y0 + (camY + SCREEN_H) * k),
		GetColor(255, 255, 255), FALSE);
	DrawBox((int)x0, (int)y0, (int)(x0 + w), (int)(y0 + h), GetColor(200, 220, 200), FALSE);
}