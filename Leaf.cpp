#include "Leaf.h"
#include "DrawUtil.h"

using namespace DrawUtil;

namespace
{
	bool inited = false;	//	すでに初期化したかどうか

	unsigned int fillCol[LEAF_COUNT];	//	塗りつぶし色
	unsigned int veinCol[LEAF_COUNT];	//	ふちや葉脈の色

	int imgHandle  [LEAF_COUNT];		//	葉っぱ画像のハンドル
	int imgH [LEAF_COUNT];				//	画像の高さ

	const char* names[LEAF_COUNT] = { "イチョウ", "緑の葉", "もみじ", "桜", "枯れ葉" };

	//	LeafType の順番：イチョウ・緑の葉・もみじ・桜・枯れ葉
	const char* FILE_NAME[LEAF_COUNT] = {
		"data/ityou.png",
		"data/leaf.png",
		"data/momiji.png",
		"data/sakura.png",
		"data/kareha.png"
	};

	//	種類ごとのサイズ
	const float SCALE [LEAF_COUNT] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };

	//	初期化（色と画像の読み込み）
	void EnsureInit()
	{
		if (inited) return; //	できていたら関数を終了する
		inited = true;

		fillCol[LEAF_GINKGO] = GetColor(242, 208, 0);		veinCol[LEAF_GINKGO] = GetColor(196, 152, 18);
		fillCol[LEAF_GREEN] = GetColor(74, 172, 62);		veinCol[LEAF_GREEN] = GetColor(34, 112, 36);
		fillCol[LEAF_MAPLE] = GetColor(214, 56, 34);		veinCol[LEAF_MAPLE] = GetColor(150, 30, 22);
		fillCol[LEAF_SAKURA] = GetColor(250, 184, 204);		veinCol[LEAF_SAKURA] = GetColor(222, 122, 152);
		fillCol[LEAF_BROWN] = GetColor(146, 98, 52);		veinCol[LEAF_BROWN] = GetColor(92, 58, 28);

		for (int i = 0; i < LEAF_COUNT; i++) {

			imgH[i] = 120;	//	120px
			imgHandle[i] = LoadGraph(FILE_NAME[i]);	//	失敗したら -1

			if (imgHandle[i] !=-1) {
				int w = 0, h = 0;
				GetGraphSize(imgHandle[i], &w, &h);
				if (h > 0) imgH[i] = h;
			}
		}
	}
}

//	staticで読んでいるので関数外で初期化
//	それぞれの色を返す
unsigned int Leaf::FillColor(LeafType type) { EnsureInit(); return fillCol[type]; }
unsigned int Leaf::VeinColor(LeafType type) { EnsureInit(); return veinCol[type]; }
//	名前を返す
const char* Leaf::Name(LeafType type) { return names[type]; }

//	描画
void Leaf::Draw(LeafType type, float cx, float cy, float size, float ang, int alpha, bool detail)
{
	(void)detail;	//	使わない
	EnsureInit();	//	初期化呼び出し

	//	もし透明度が最大ではないなら透明をON
	if (alpha < 255) Blend(alpha);

	if (imgHandle[type] != -1) {
		//	描画拡大倍率計算
		double rate = (double)(size * SCALE[type] / (float)imgH[type]);
		//	バイリニア方で拡大縮小・回転をなめらかにする
		SetDrawMode(DX_DRAWMODE_BILINEAR);		
		DrawRotaGraphF(cx, cy, rate, (double)ang, imgHandle[type], TRUE);	//	画像描画
		SetDrawMode(DX_DRAWMODE_NEAREST);
	}
	else {
		//	画像が読み込めなかったときの代わり：色つきの丸
		DrawCircleAA(cx, cy, size * 0.4f, 24, fillCol[type], TRUE);
	}
	//	透明設定を元に戻す
	if (alpha < 255) NoBlend();
}