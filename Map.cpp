#include "Map.h"
#include "DrawUtil.h"

using namespace DrawUtil;

// ------------------------------------------------------------
//  生成 
// ------------------------------------------------------------
void Map::FillCanopy(Tree& t, int cnt, float smin, float smax)
{
	for (int i = 0; i < cnt; i++) {
		float a = Rf(0, PI2), rr = t.canopyR * 0.88f * sqrtf(Rf(0, 1));
		FloorLeaf l;
		l.x = cosf(a) * rr; l.y = sinf(a) * rr;
		l.size = Rf(smin, smax); l.ang = Rf(0, PI2); l.type = t.leaf;
		t.leaves.push_back(l);
	}
}

void Map::AddCircleObstacle(float x, float y, float r)
{
	Obstacle o; o.box = false; o.x = x; o.y = y; o.r = r; o.hw = o.hh = o.ang = 0;
	obstacles.push_back(o);
}

void Map::AddBoxObstacle(float x, float y, float hw, float hh, float ang)
{
	Obstacle o; o.box = true; o.x = x; o.y = y; o.r = 0; o.hw = hw; o.hh = hh; o.ang = ang;
	obstacles.push_back(o);
}

void Map::AddPatch(float x, float y, float r, LeafType type)
{
	Patch p; p.x = x; p.y = y; p.r = r; p.type = type;
	int cnt = (int)(r * r * 3.14f / 650.0f);
	for (int i = 0; i < cnt; i++) {
		float a = Rf(0, PI2), rr = r * sqrtf(Rf(0, 1));
		FloorLeaf l;
		l.x = cosf(a) * rr; l.y = sinf(a) * rr;
		l.size = Rf(24, 34); l.ang = Rf(0, PI2); l.type = type;
		p.leaves.push_back(l);
	}
	patches.push_back(p);
}

void Map::Build(int stage)
{
	SRand(1234 + stage * 977);		// シード固定 → 毎回同じマップ 
	floorLeaves.clear(); blobs.clear(); patches.clear(); trees.clear();
	stumps.clear(); logs.clear(); obstacles.clear();

	int nBig, nSmall, nStump, nLog, nPatch;
	if (stage == 1) {
		mapW = 2600; mapH = 1700;
		startX = 170; startY = mapH - 170.0f;
		goalX = mapW - 190.0f; goalY = 190;
		nBig = 24; nSmall = 14; nStump = 10; nLog = 9; nPatch = 9;
	}
	else {
		mapW = 3400; mapH = 2100;
		startX = 170; startY = 170;
		goalX = mapW - 190.0f; goalY = mapH - 190.0f;
		nBig = 40; nSmall = 24; nStump = 16; nLog = 14; nPatch = 14;
	}

	// ---- 障害物の配置（間隔を保って通れる隙間を確保）---- 
	struct Placed { float x, y, clear; };
	std::vector<Placed> placed;
	auto tryPlace = [&](float clear, float& ox, float& oy) -> bool {
		for (int k = 0; k < 80; k++) {
			float x = Rf(90, mapW - 90.0f), y = Rf(90, mapH - 90.0f);
			if (Dist(x, y, startX, startY) < 190 || Dist(x, y, goalX, goalY) < 190) continue;
			bool ok = true;
			for (size_t i = 0; i < placed.size(); i++)
				if (Dist(x, y, placed[i].x, placed[i].y) < clear + placed[i].clear) { ok = false; break; }
			if (!ok) continue;
			Placed p = { x, y, clear }; placed.push_back(p);
			ox = x; oy = y; return true;
		}
		return false;
		};

	float x, y;
	// 大きな木 
	for (int i = 0; i < nBig; i++) if (tryPlace(62, x, y)) {
		Tree t; t.x = x; t.y = y; t.trunkR = Rf(20, 28); t.canopyR = Rf(95, 125);
		t.leaf = (LeafType)GetRand(LEAF_COUNT - 1);
		FillCanopy(t, 52, 46, 58);
		trees.push_back(t); AddCircleObstacle(x, y, t.trunkR);
	}
	// ちっちゃい木 
	for (int i = 0; i < nSmall; i++) if (tryPlace(55, x, y)) {
		Tree t; t.x = x; t.y = y; t.trunkR = Rf(8, 11); t.canopyR = Rf(45, 60);
		t.leaf = (LeafType)GetRand(LEAF_COUNT - 1);
		FillCanopy(t, 14, 30, 38);
		trees.push_back(t); AddCircleObstacle(x, y, t.trunkR);
	}
	// 切り株 
	for (int i = 0; i < nStump; i++) if (tryPlace(50, x, y)) {
		Stump s; s.x = x; s.y = y; s.r = Rf(18, 28);
		stumps.push_back(s); AddCircleObstacle(x, y, s.r);
	}
	// 倒木（斜めあり） 
	for (int i = 0; i < nLog; i++) if (tryPlace(95, x, y)) {
		LogObj l; l.x = x; l.y = y; l.hw = Rf(55, 85); l.hh = Rf(12, 17); l.ang = Rf(0, PI);
		logs.push_back(l); AddBoxObstacle(x, y, l.hw, l.hh, l.ang);
	}

	// ---- 落ち葉だまり（擬態ポイント）---- 
	// スタート近くに緑＋もう1種類（チュートリアル用） 
	if (stage == 1) {
		AddPatch(470, mapH - 300.0f, 110, LEAF_GREEN);
		AddPatch(820, mapH - 450.0f, 115, LEAF_MAPLE);
	}
	else {
		AddPatch(470, 300, 110, LEAF_GREEN);
		AddPatch(820, 430, 115, LEAF_SAKURA);
	}
	// ゴール近くにも1つ 
	AddPatch(goalX - 260, goalY + (stage == 1 ? 200.0f : -200.0f), 110, (LeafType)GetRand(LEAF_COUNT - 1));
	// 残りはランダム配置 
	for (int i = 0; i < nPatch; i++) {
		for (int k = 0; k < 60; k++) {
			float px = Rf(150, mapW - 150.0f), py = Rf(150, mapH - 150.0f);
			bool ok = true;
			for (size_t j = 0; j < patches.size(); j++)
				if (Dist(px, py, patches[j].x, patches[j].y) < 250) { ok = false; break; }
			if (!ok) continue;
			AddPatch(px, py, Rf(95, 135), (LeafType)GetRand(LEAF_COUNT - 1));
			break;
		}
	}

	// ---- 地面の装飾 ---- 
	for (int i = 0; i < 160; i++) {
		Blob b; b.x = Rf(0, mapW); b.y = Rf(0, mapH); b.r = Rf(30, 90);
		b.col = (GetRand(1) == 0) ? GetColor(18, 34, 18) : GetColor(52, 80, 40);
		blobs.push_back(b);
	}
	int nFloor = (int)(mapW * mapH / 11000);
	for (int i = 0; i < nFloor; i++) {
		FloorLeaf l; l.x = Rf(0, mapW); l.y = Rf(0, mapH);
		l.size = Rf(20, 30); l.ang = Rf(0, PI2); l.type = (LeafType)GetRand(LEAF_COUNT - 1);
		floorLeaves.push_back(l);
	}

	SRand(GetNowCount());			// 以降のゲーム中の乱数はランダムに戻す 
}

// ------------------------------------------------------------
//  カメラ・判定 
// ------------------------------------------------------------
void Map::CalcCamera(float tx, float ty, float& camX, float& camY) const
{
	camX = Clampf(tx - screenW * 0.5f, 0, fmaxf(0.0f, mapW - screenW));
	camY = Clampf(ty - screenH * 0.5f, 0, fmaxf(0.0f, mapH - screenH));
}

bool Map::IsGoal(float x, float y, float r) const
{
	return Dist(x, y, goalX, goalY) < r;
}

bool Map::ResolveCollision(float& x, float& y, float r) const
{
	float x0 = x, y0 = y;
	for (int it = 0; it < 3; it++) {
		for (size_t i = 0; i < obstacles.size(); i++) {
			const Obstacle& o = obstacles[i];
			if (!o.box) {
				float dx = x - o.x, dy = y - o.y;
				float minD = o.r + r;
				if (fabsf(dx) > minD || fabsf(dy) > minD) continue;
				float d = sqrtf(dx * dx + dy * dy);
				if (d < minD) {
					if (d < 0.001f) { dx = 1; dy = 0; d = 1; }
					x = o.x + dx / d * minD;
					y = o.y + dy / d * minD;
				}
			}
			else {
				float dx = x - o.x, dy = y - o.y;
				float reach = o.hw + o.hh + r;
				if (fabsf(dx) > reach || fabsf(dy) > reach) continue;
				float c = cosf(o.ang), s = sinf(o.ang);
				float lx = dx * c + dy * s;
				float ly = -dx * s + dy * c;
				float cx = Clampf(lx, -o.hw, o.hw);
				float cy = Clampf(ly, -o.hh, o.hh);
				float ddx = lx - cx, ddy = ly - cy;
				float d2 = ddx * ddx + ddy * ddy;
				if (d2 < r * r) {
					if (d2 > 1e-6f) {
						float d = sqrtf(d2);
						lx = cx + ddx / d * r;
						ly = cy + ddy / d * r;
					}
					else {
						float pxv = o.hw - fabsf(lx), pyv = o.hh - fabsf(ly);
						if (pxv < pyv) lx = (lx >= 0 ? o.hw + r : -o.hw - r);
						else           ly = (ly >= 0 ? o.hh + r : -o.hh - r);
					}
					x = o.x + lx * c - ly * s;
					y = o.y + lx * s + ly * c;
				}
			}
		}
		x = Clampf(x, r, mapW - r);
		y = Clampf(y, r, mapH - r);
	}
	float dx = x - x0, dy = y - y0;
	return dx * dx + dy * dy > 1e-6f;
}

bool Map::IsInsideObstacle(float x, float y, float r) const
{
	float ox = x, oy = y;
	ResolveCollision(ox, oy, r);
	return Dist(ox, oy, x, y) > 0.01f;
}

bool Map::IsCamouflaged(float x, float y, LeafType disguise) const
{
	for (size_t i = 0; i < patches.size(); i++) {
		const Patch& p = patches[i];
		if (p.type == disguise && Dist(x, y, p.x, p.y) < p.r) return true;
	}
	for (size_t i = 0; i < trees.size(); i++) {
		const Tree& t = trees[i];
		if (t.leaf == disguise && Dist(x, y, t.x, t.y) < t.canopyR * 0.75f) return true;
	}
	return false;
}

// ------------------------------------------------------------
//  描画 
// ------------------------------------------------------------
bool Map::OnScreen(float sx, float sy, float m) const
{
	return sx > -m && sx < screenW + m && sy > -m && sy < screenH + m;
}

void Map::DrawGoal(float sx, float sy) const
{
	float t = GetNowCount() / 1000.0f;
	float pulse = 0.5f + 0.5f * sinf(t * 3.0f);
	for (int i = 4; i >= 1; i--) {
		Blend(30 + i * 8);
		DrawCircleAA(sx, sy, 28.0f + i * 14.0f + pulse * 6.0f, 40, GetColor(255, 235, 120), TRUE);
	}
	NoBlend();
	DrawCircleAA(sx, sy, 38.0f + pulse * 4.0f, 40, GetColor(255, 250, 200), FALSE, 3.0f);
	DrawLineAA(sx, sy + 20, sx, sy - 6, GetColor(60, 140, 50), 4.0f);
	Leaf::Draw(LEAF_GREEN, sx - 14, sy - 10, 30, -0.9f);
	Leaf::Draw(LEAF_GREEN, sx + 14, sy - 10, 30, 0.9f);
}

void Map::DrawGround(float cx, float cy) const
{
	DrawBox(0, 0, screenW, screenH, GetColor(30, 52, 30), TRUE);

	// 土のムラ 
	Blend(70);
	for (size_t i = 0; i < blobs.size(); i++) {
		const Blob& b = blobs[i];
		float sx = b.x - cx, sy = b.y - cy;
		if (!OnScreen(sx, sy, b.r)) continue;
		DrawCircleAA(sx, sy, b.r, 32, b.col, TRUE);
	}
	NoBlend();

	// 地面の落ち葉 
	for (size_t i = 0; i < floorLeaves.size(); i++) {
		const FloorLeaf& l = floorLeaves[i];
		float sx = l.x - cx, sy = l.y - cy;
		if (!OnScreen(sx, sy, 40)) continue;
		Leaf::Draw(l.type, sx, sy, l.size, l.ang, 255, false);
	}
	// 落ち葉だまり 
	for (size_t i = 0; i < patches.size(); i++) {
		const Patch& p = patches[i];
		float sx = p.x - cx, sy = p.y - cy;
		if (!OnScreen(sx, sy, p.r + 40)) continue;
		Blend(60);
		DrawCircleAA(sx, sy, p.r, 48, Leaf::FillColor(p.type), TRUE);
		NoBlend();
		for (size_t j = 0; j < p.leaves.size(); j++) {
			const FloorLeaf& l = p.leaves[j];
			Leaf::Draw(l.type, sx + l.x, sy + l.y, l.size, l.ang, 255, false);
		}
	}

	// 木の影 
	Blend(45);
	for (size_t i = 0; i < trees.size(); i++) {
		const Tree& t = trees[i];
		float sx = t.x - cx, sy = t.y - cy;
		if (!OnScreen(sx, sy, t.canopyR + 40)) continue;
		DrawCircleAA(sx + 8, sy + 10, t.canopyR * 0.95f, 40, GetColor(0, 0, 0), TRUE);
	}
	NoBlend();

	DrawGoal(goalX - cx, goalY - cy);

	// 切り株 
	for (size_t i = 0; i < stumps.size(); i++) {
		const Stump& s = stumps[i];
		float sx = s.x - cx, sy = s.y - cy;
		if (!OnScreen(sx, sy, s.r + 20)) continue;
		Blend(70); DrawCircleAA(sx + 4, sy + 5, s.r, 24, GetColor(0, 0, 0), TRUE); NoBlend();
		DrawCircleAA(sx, sy, s.r, 28, GetColor(96, 66, 40), TRUE);
		DrawCircleAA(sx, sy, s.r * 0.78f, 28, GetColor(170, 128, 82), TRUE);
		DrawCircleAA(sx, sy, s.r * 0.78f, 28, GetColor(120, 84, 50), FALSE, 1.5f);
		DrawCircleAA(sx, sy, s.r * 0.50f, 24, GetColor(120, 84, 50), FALSE, 1.5f);
		DrawCircleAA(sx, sy, s.r * 0.20f, 16, GetColor(120, 84, 50), FALSE, 1.5f);
	}
	// 倒木（回転ボックス） 
	for (size_t i = 0; i < logs.size(); i++) {
		const LogObj& l = logs[i];
		float sx = l.x - cx, sy = l.y - cy;
		if (!OnScreen(sx, sy, l.hw + 30)) continue;
		Transform f = MakeXf(sx, sy, l.ang);
		Transform sh = MakeXf(sx + 5, sy + 6, l.ang);
		float q[8], w[8];
		sh.pt(-l.hw, -l.hh, w[0], w[1]); sh.pt(l.hw, -l.hh, w[2], w[3]);
		sh.pt(l.hw, l.hh, w[4], w[5]);   sh.pt(-l.hw, l.hh, w[6], w[7]);
		Blend(70); DrawQuadrangleAA(w[0], w[1], w[2], w[3], w[4], w[5], w[6], w[7], GetColor(0, 0, 0), TRUE); NoBlend();
		f.pt(-l.hw, -l.hh, q[0], q[1]); f.pt(l.hw, -l.hh, q[2], q[3]);
		f.pt(l.hw, l.hh, q[4], q[5]);   f.pt(-l.hw, l.hh, q[6], q[7]);
		DrawQuadrangleAA(q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7], GetColor(98, 66, 38), TRUE);
		for (int k = -1; k <= 1; k++)
			LineL(f, -l.hw + 4, k * l.hh * 0.55f, l.hw - 4, k * l.hh * 0.55f, GetColor(70, 46, 26), 1.5f);
		// こけ 
		LineL(f, -l.hw * 0.5f, -l.hh * 0.9f, l.hw * 0.2f, -l.hh * 0.9f, GetColor(70, 130, 60), 3.0f);
		// 切り口 
		FillEllipse(f, -l.hw, 0, l.hh * 0.55f, l.hh, GetColor(172, 130, 84));
		FillEllipse(f, l.hw, 0, l.hh * 0.55f, l.hh, GetColor(172, 130, 84));
		FillEllipse(f, l.hw, 0, l.hh * 0.30f, l.hh * 0.55f, GetColor(140, 100, 62));
		FillEllipse(f, -l.hw, 0, l.hh * 0.30f, l.hh * 0.55f, GetColor(140, 100, 62));
	}
	// 木の幹 
	for (size_t i = 0; i < trees.size(); i++) {
		const Tree& t = trees[i];
		float sx = t.x - cx, sy = t.y - cy;
		if (!OnScreen(sx, sy, t.trunkR + 20)) continue;
		Blend(70); DrawCircleAA(sx + 5, sy + 6, t.trunkR, 24, GetColor(0, 0, 0), TRUE); NoBlend();
		DrawCircleAA(sx, sy, t.trunkR, 28, GetColor(84, 56, 34), TRUE);
		DrawCircleAA(sx - t.trunkR * 0.2f, sy - t.trunkR * 0.2f, t.trunkR * 0.55f, 20, GetColor(112, 78, 46), TRUE);
		DrawCircleAA(sx, sy, t.trunkR, 28, GetColor(52, 34, 20), FALSE, 2.0f);
	}
}

void Map::DrawCanopy(float cx, float cy) const
{
	// 木の葉（半透明なので下のキャラが透けて見える） 
	for (size_t i = 0; i < trees.size(); i++) {
		const Tree& t = trees[i];
		float sx = t.x - cx, sy = t.y - cy;
		if (!OnScreen(sx, sy, t.canopyR + 60)) continue;
		for (size_t j = 0; j < t.leaves.size(); j++) {
			const FloorLeaf& l = t.leaves[j];
			Leaf::Draw(t.leaf, sx + l.x, sy + l.y, l.size, l.ang, 200, false);
		}
	}
}