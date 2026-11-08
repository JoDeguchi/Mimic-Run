#include "Leaf.h"
#include "DrawUtil.h"
#include <vector>

using namespace DrawUtil;

namespace
{
	struct Pt { float x, y; };

	bool inited = false;
	unsigned int fillCol[LEAF_COUNT], veinCol[LEAF_COUNT];
	std::vector<Pt> shape[LEAF_COUNT];
	Pt pivot[LEAF_COUNT];

	const char* names[LEAF_COUNT] = { "イチョウ", "緑の葉", "もみじ", "桜", "枯れ葉" };

	void EnsureInit()
	{
		if (inited) return;
		inited = true;

		fillCol[LEAF_GINKGO] = GetColor(242, 208, 40);		veinCol[LEAF_GINKGO] = GetColor(196, 152, 18);
		fillCol[LEAF_GREEN] = GetColor(74, 172, 62);		veinCol[LEAF_GREEN] = GetColor(34, 112, 36);
		fillCol[LEAF_MAPLE] = GetColor(214, 56, 34);		veinCol[LEAF_MAPLE] = GetColor(150, 30, 22);
		fillCol[LEAF_SAKURA] = GetColor(250, 184, 204);		veinCol[LEAF_SAKURA] = GetColor(222, 122, 152);
		fillCol[LEAF_BROWN] = GetColor(146, 98, 52);		veinCol[LEAF_BROWN] = GetColor(92, 58, 28);

		// 普通の葉（緑・枯れ葉で共用。枯れ葉は縁を少しうねらせる） 
		for (int kind = 0; kind < 2; kind++) {
			LeafType lt = (kind == 0) ? LEAF_GREEN : LEAF_BROWN;
			std::vector<Pt>& v = shape[lt];
			float wob = (kind == 0) ? 0.0f : 0.10f;
			const int N = 10;
			for (int i = 0; i <= N; i++) {
				float t = (float)i / N;
				float w = 0.34f * sinf(PI * powf(t, 0.75f)) * (1.0f + wob * sinf(t * 18.0f));
				Pt p = { -w, -t }; v.push_back(p);
			}
			for (int i = N - 1; i >= 1; i--) {
				float t = (float)i / N;
				float w = 0.34f * sinf(PI * powf(t, 0.75f)) * (1.0f + wob * sinf(t * 18.0f + 1.7f));
				Pt p = { w, -t }; v.push_back(p);
			}
			pivot[lt].x = 0; pivot[lt].y = -0.5f;
		}
		// もみじ（5つの尖った葉） 
		{
			std::vector<Pt>& v = shape[LEAF_MAPLE];
			for (int k = 0; k < 40; k++) {
				float th = PI2 * k / 40.0f;
				float m = fmaxf(0.0f, 0.5f + 0.5f * cosf(5.0f * th));
				float r = 0.52f * (0.38f + 0.62f * powf(m, 0.7f));
				Pt p = { r * sinf(th), -0.5f - r * cosf(th) }; v.push_back(p);
			}
			pivot[LEAF_MAPLE].x = 0; pivot[LEAF_MAPLE].y = -0.5f;
		}
		// 桜（先端が割れた花びら） 
		{
			static const float d[][2] = {
				{ 0, 0 }, { -0.12f, -0.12f }, { -0.26f, -0.35f }, { -0.34f, -0.60f }, { -0.30f, -0.85f },
				{ -0.12f, -1.00f }, { 0, -0.88f }, { 0.12f, -1.00f }, { 0.30f, -0.85f }, { 0.34f, -0.60f },
				{ 0.26f, -0.35f }, { 0.12f, -0.12f }
			};
			for (int i = 0; i < 12; i++) { Pt p = { d[i][0], d[i][1] }; shape[LEAF_SAKURA].push_back(p); }
			pivot[LEAF_SAKURA].x = 0; pivot[LEAF_SAKURA].y = -0.5f;
		}
		// イチョウ（扇形・中央に切れ込み） 
		{
			std::vector<Pt>& v = shape[LEAF_GINKGO];
			Pt o = { 0, 0 }; v.push_back(o);
			for (int deg = -75; deg <= 75; deg += 10) {
				float rad = deg * PI / 180.0f;
				float R = 0.98f - 0.30f * expf(-((float)deg / 9.0f) * ((float)deg / 9.0f));
				Pt p = { R * sinf(rad) * 0.8f, -R * cosf(rad) }; v.push_back(p);
			}
			pivot[LEAF_GINKGO].x = 0; pivot[LEAF_GINKGO].y = 0;
		}
	}
}

unsigned int Leaf::FillColor(LeafType type) { EnsureInit(); return fillCol[type]; }
unsigned int Leaf::VeinColor(LeafType type) { EnsureInit(); return veinCol[type]; }
const char* Leaf::Name(LeafType type) { return names[type]; }

void Leaf::Draw(LeafType type, float cx, float cy, float size, float ang, int alpha, bool detail)
{
	EnsureInit();
	const std::vector<Pt>& sh = shape[type];
	int n = (int)sh.size();
	Transform f = MakeXf(cx, cy, ang);
	float px[48], py[48];
	for (int i = 0; i < n; i++) f.pt(sh[i].x * size, (sh[i].y + 0.5f) * size, px[i], py[i]);
	float pvx, pvy;
	f.pt(pivot[type].x * size, (pivot[type].y + 0.5f) * size, pvx, pvy);

	if (alpha < 255) Blend(alpha);
	for (int i = 0; i < n; i++) {
		int j = (i + 1) % n;
		DrawTriangleAA(pvx, pvy, px[i], py[i], px[j], py[j], fillCol[type], TRUE);
	}
	if (detail) {
		for (int i = 0; i < n; i++) {
			int j = (i + 1) % n;
			DrawLineAA(px[i], py[i], px[j], py[j], veinCol[type], 1.5f);
		}
	}
	unsigned int vc = veinCol[type];
	float th = detail ? 2.0f : 1.0f;
#define LV(x1, y1, x2, y2, t) LineL(f, (x1) * size, ((y1) + 0.5f) * size, (x2) * size, ((y2) + 0.5f) * size, vc, (t))
	switch (type) {
	case LEAF_GREEN:
	case LEAF_BROWN:
		LV(0, 0.08f, 0, -0.9f, th);
		if (detail) {
			for (int k = 1; k <= 3; k++) {
				float t = 0.2f * k;
				LV(0, -t, 0.2f, -t - 0.15f, 1.2f);
				LV(0, -t, -0.2f, -t - 0.15f, 1.2f);
			}
		}
		break;
	case LEAF_SAKURA:
		LV(0, 0, 0, -0.55f, 1.2f);
		break;
	case LEAF_MAPLE:
		for (int k = 0; k < 5; k++) {
			float a = PI2 * k / 5.0f;
			LV(0, -0.5f, 0.46f * sinf(a), -0.5f - 0.46f * cosf(a), detail ? 1.5f : 1.0f);
		}
		LV(0, -0.5f, 0, 0.04f, th);
		break;
	case LEAF_GINKGO:
		for (int deg = -60; deg <= 60; deg += 30) {
			float rad = deg * PI / 180.0f;
			LV(0, 0, 0.85f * sinf(rad) * 0.8f, -0.85f * cosf(rad), detail ? 1.5f : 1.0f);
		}
		LV(0, 0, 0, 0.08f, th);
		break;
	default: break;
	}
#undef LV
	if (alpha < 255) NoBlend();
}