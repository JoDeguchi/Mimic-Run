#include "Tension.h"
#include "EnemyManager.h"
#include "DrawUtil.h"

using namespace DrawUtil;

namespace
{
	const int   SCREEN_W = 1280;
	const int   SCREEN_H = 720;

	const float START_DIST = 300.0f;	// この距離まで敵が近づくと、演出が始まる 
	const float MAX_DIST = 60.0f;		// この距離まで近づくと、最大になる 
	const float RISE = 0.06f;		// 強くなる速さ（1フレームあたり） 
	const float FALL = 0.03f;		// 弱くなる速さ（1フレームあたり） 

	const float BEAT_SLOW = 60.0f;		// 鼓動の間隔（フレーム）：敵が遠いとき（1秒に1回） 
	const float BEAT_FAST = 24.0f;		// 鼓動の間隔（フレーム）：敵がすぐそばのとき（1秒に2.5回） 

	const int   BAND_COUNT = 10;		// 画面のふちの赤い帯の段数 
	const int   BAND_WIDTH = 16;		// 1段の幅（px） 
}

void TensionEffect::Reset()
{
	level = 0;
	phase = 0;
}

void TensionEffect::Update(const EnemyManager& enemies, float px, float py, bool hidden)
{
	// 隠れているときだけ、いちばん近い敵までの距離から目標の強さを決める 
	float target = 0;
	if (hidden) {
		for (size_t i = 0; i < enemies.Count(); i++) {
			const Enemy& e = enemies.Get(i);
			float d = Dist(px, py, e.GetX(), e.GetY());
			float v = Clampf((START_DIST - d) / (START_DIST - MAX_DIST), 0.0f, 1.0f);
			if (v > target) target = v;
		}
	}

	// 目標へなめらかに近づける（急に出たり消えたりしない） 
	if (target > level) level = fminf(target, level + RISE);
	else                level = fmaxf(target, level - FALL);

	// 強いほど鼓動が速い 
	float period = BEAT_SLOW - (BEAT_SLOW - BEAT_FAST) * level;
	phase += 1.0f / period;
	if (phase >= 1.0f) phase -= 1.0f;
}

float TensionEffect::Beat() const
{
	// ドクン（強）・ドクン（弱）の2回の山。phase が 0 と 0.2 のあたりで盛り上がる 
	float lub = expf(-(phase * phase) / 0.004f);
	float dub = 0.7f * expf(-((phase - 0.2f) * (phase - 0.2f)) / 0.004f);
	return fminf(1.0f, lub + dub);
}

void TensionEffect::Draw(float playerX, float playerY, float camX, float camY) const
{
	if (level < 0.01f) return;

	float beat = Beat();
	unsigned int red = GetColor(210, 20, 20);

	// ---- ① 画面のふちが赤くなる（外側ほど濃い・鼓動に合わせて脈打つ）---- 
	float power = level * (0.5f + 0.5f * beat);
	for (int i = 0; i < BAND_COUNT; i++) {
		int a = (int)(power * 110.0f * (1.0f - (float)i / BAND_COUNT));
		if (a <= 0) continue;
		int o = i * BAND_WIDTH;
		Blend(a);
		DrawBox(o, o, SCREEN_W - o, o + BAND_WIDTH, red, TRUE);								// 上 
		DrawBox(o, SCREEN_H - o - BAND_WIDTH, SCREEN_W - o, SCREEN_H - o, red, TRUE);		// 下 
		DrawBox(o, o + BAND_WIDTH, o + BAND_WIDTH, SCREEN_H - o - BAND_WIDTH, red, TRUE);	// 左 
		DrawBox(SCREEN_W - o - BAND_WIDTH, o + BAND_WIDTH, SCREEN_W - o, SCREEN_H - o - BAND_WIDTH, red, TRUE);	// 右 
	}
	NoBlend();

	// ---- ② 足元の心臓の輪（鼓動のたびに広がる）---- 
	float sx = playerX - camX, sy = playerY - camY;
	Blend((int)(220.0f * level * beat));
	DrawCircleAA(sx, sy, 20.0f + beat * 18.0f, 32, GetColor(255, 70, 60), FALSE, 2.0f + 3.0f * beat);
	NoBlend();
}