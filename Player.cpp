#include "Player.h"
#include "Map.h"
#include "KeyReader.h"
#include "DrawUtil.h"

using namespace DrawUtil;

namespace
{
	const float RADIUS = 11.0f;			// 当たり判定の半径 
	const float SPEED = 170.0f / 60.0f;	// 1フレームの移動量（秒速170px） 
	const float TURN_RATE = 0.23f;			// 向きが変わる速さ（0〜1） 
	const float WALK_RATE = 14.0f / 60.0f;	// 足の動く速さ 
	const int   MORPH_FRAME = 27;				// 着替えエフェクトの長さ（約0.45秒） 
	const float BODY_SIZE = 50.0f;			// 葉っぱの大きさ 

	const float ARROW_FLIP_Y = 130.0f;			// 画面の上端に近いとき、矢印を下側に出す境目 
	const float PING_FRAME = 72.0f;			// 足元の輪が広がる周期（約1.2秒） 
}

float Player::GetRadius() const { return RADIUS; }

void Player::Init(float startX, float startY)
{
	x = startX; y = startY;
	angle = 0;
	disguise = LEAF_GREEN;
	walkT = 0;
	morphFrame = 0;
	camouflaged = false;
	arrow_anim_frame = 0;
}

void Player::SetDisguise(LeafType type)
{
	if (type == disguise) return;
	disguise = type;
	morphFrame = MORPH_FRAME;
}

bool Player::IsPointOver(int sx, int sy, float camX, float camY, float range) const
{
	return Dist((float)sx, (float)sy, x - camX, y - camY) < range;
}

void Player::Update(const KeyReader& key, const Map& map)
{
	arrow_anim_frame++;
	if (morphFrame > 0) morphFrame--;

	// ---- 入力 ---- 
	float ix = 0, iy = 0;
	if (key.CheckHoldKey(KEY_INPUT_W)) iy -= 1;
	if (key.CheckHoldKey(KEY_INPUT_S)) iy += 1;
	if (key.CheckHoldKey(KEY_INPUT_A)) ix -= 1;
	if (key.CheckHoldKey(KEY_INPUT_D)) ix += 1;
	if (ix == 0 && iy == 0) return;

	// ---- 移動 ---- 
	float len = sqrtf(ix * ix + iy * iy);
	ix /= len; iy /= len;			// 斜めでも速さが変わらないようにする 
	x += ix * SPEED;
	y += iy * SPEED;
	map.ResolveCollision(x, y, RADIUS);	// 障害物・マップ外への押し戻し 

	// ---- 向きをなめらかに進行方向へ ---- 
	float target = atan2f(ix, -iy);		// 「前」は -y 方向 
	float diff = target - angle;
	while (diff > PI)  diff -= PI2;
	while (diff < -PI) diff += PI2;
	angle += diff * TURN_RATE;

	walkT += WALK_RATE;
}

void Player::Draw(float camX, float camY) const
{
	float sx = x - camX, sy = y - camY;

	// 着替え直後は少しふくらむ 
	float pop = 1.0f;
	if (morphFrame > 0) {
		float prog = 1.0f - (float)morphFrame / MORPH_FRAME;	// 0→1 
		pop = 1.0f + 0.3f * sinf(prog * PI);
	}
	float size = BODY_SIZE * pop;

	// 擬態中は足元がうっすら光る 
	if (camouflaged) {
		Blend(70);
		DrawCircleAA(sx, sy, 30, 32, GetColor(160, 255, 160), TRUE);
		NoBlend();
	}

	// 影 
	Blend(60);
	FillEllipse(MakeXf(sx + 4, sy + 6, angle), 0, 0, size * 0.3f, size * 0.5f, GetColor(0, 0, 0));
	NoBlend();

	// あし（3本ずつ左右に） 
	Transform f = MakeXf(sx, sy, angle);
	unsigned int legCol = GetColor(46, 86, 40);
	for (int k = 0; k < 3; k++) {
		float ly = (-0.18f + 0.18f * k) * size;
		float ph = sinf(walkT + k * 2.1f) * 0.07f * size;
		LineL(f, -0.12f * size, ly, -0.42f * size, ly + ph + (k - 1) * 0.04f * size, legCol, 2.0f);
		LineL(f, 0.12f * size, ly, 0.42f * size, ly - ph + (k - 1) * 0.04f * size, legCol, 2.0f);
	}

	// からだ（葉っぱ） 
	Leaf::Draw(disguise, sx, sy, size, angle, 255, true);

	// あたま・め 
	FillEllipse(f, 0, -0.45f * size, 0.08f * size, 0.075f * size, Leaf::VeinColor(disguise));
	FillEllipse(f, -0.035f * size, -0.47f * size, 1.6f, 1.6f, GetColor(15, 15, 15));
	FillEllipse(f, 0.035f * size, -0.47f * size, 1.6f, 1.6f, GetColor(15, 15, 15));

	// 着替えエフェクト（広がる輪） 
	if (morphFrame > 0) {
		float prog = 1.0f - (float)morphFrame / MORPH_FRAME;
		Blend((int)(200 * (1.0f - prog)));
		DrawCircleAA(sx, sy, 14.0f + prog * 60.0f, 40, Leaf::FillColor(disguise), FALSE, 4.0f);
		NoBlend();
	}
}

void Player::DrawMarker(float camX, float camY) const
{
	float sx = x - camX, sy = y - camY;

	// 小さな白い輪（木の葉の下でも位置が分かる） 
	Blend(110);
	DrawCircleAA(sx, sy, 19, 24, GetColor(255, 255, 255), FALSE, 1.5f);
	NoBlend();

	DrawArrow(sx, sy);
}

void Player::DrawArrow(float sx, float sy) const
{
	float t = (float)arrow_anim_frame;
	float wave = sinf(t * 0.18f);					// -1〜1：上下にはずむ 
	float k = (sinf(t * 0.12f) + 1.0f) * 0.5f;		// 0〜1：色のゆっくりした点滅 

	// ① 足元から広がる輪（レーダーのように、一定間隔で出る） 
	float cyc = fmodf(t, PING_FRAME) / PING_FRAME;	// 0→1 
	Blend((int)(200 * (1.0f - cyc)));
	DrawCircleAA(sx, sy, 16.0f + cyc * 36.0f, 40, GetColor(255, 240, 120), FALSE, 1.0f + 3.0f * (1.0f - cyc));
	NoBlend();

	// ② 矢印の位置：頭上でプレイヤーを指す（画面の上端付近では足元側から上向きに） 
	float u = (sy < ARROW_FLIP_Y) ? -1.0f : 1.0f;	// 1 = 頭上 / -1 = 足元 
	float tipY = sy - u * (30.0f + wave * 6.0f);	// 先端（はずむ） 
	float headBase = tipY - u * 18.0f;				// 三角の底 
	float shaftEnd = tipY - u * 42.0f;				// 軸のはし 

	// ③ ぼんやり光る背景（点滅に合わせて明るさが変わる） 
	Blend(50 + (int)(50 * k));
	DrawCircleAA(sx, (tipY + shaftEnd) * 0.5f, 30.0f + 4.0f * k, 32, GetColor(255, 235, 120), TRUE);
	NoBlend();

	// ④ 矢印本体（黄色↔白っぽく点滅） 
	unsigned int fill = GetColor(255, 215 + (int)(40 * k), 60 + (int)(150 * k));
	unsigned int edge = GetColor(60, 40, 0);
	DrawQuadrangleAA(sx - 6, headBase, sx + 6, headBase, sx + 6, shaftEnd, sx - 6, shaftEnd, fill, TRUE);
	DrawTriangleAA(sx, tipY, sx - 15, headBase, sx + 15, headBase, fill, TRUE);

	// ⑤ 黒い縁取り（どんな背景でも見えるように） 
	float ox[7] = { sx, sx + 15, sx + 6, sx + 6, sx - 6, sx - 6, sx - 15 };
	float oy[7] = { tipY, headBase, headBase, shaftEnd, shaftEnd, headBase, headBase };
	for (int i = 0; i < 7; i++) {
		int j = (i + 1) % 7;
		DrawLineAA(ox[i], oy[i], ox[j], oy[j], edge, 2.0f);
	}
}