#include "LeafShower.h"
#include "DrawUtil.h"

using namespace DrawUtil;

//	開始状態
void LeafShower::Start(float screenW, float screenH)
{
	w = screenW; h = screenH;

	pieces.clear();	//	初期化

	for (int i = 0; i < 60; i++) {
		Piece p;
		// 上にずらして並べ、順番に降らせる
		p.x = Randomfloat(0, w);
		p.y = Randomfloat(-h, 0);						 
		p.vx = Randomfloat (-0.5f, 0.5f);
		p.vy = Randomfloat(1.5f, 3.5f);
		p.ang = Randomfloat(0, PI2);	//	２πラジアン（360°）
		p.spin = Randomfloat(-0.05f, 0.05f);	//	回転
		p.size = Randomfloat(30, 56);	//	大きさ
		p.type = (LeafType)GetRand(LEAF_COUNT - 1);	//	いろんな種類の葉
		pieces.push_back(p);	//	６０枚の葉
	}
}
//	ふらせる処理
void LeafShower::Update()
{
	for (size_t i = 0; i < pieces.size(); i++) {
		Piece& p = pieces[i];
		p.y += p.vy;
		p.x += p.vx + sinf(p.y * 0.02f) * 0.6f;		// ひらひら揺れる 
		p.ang += p.spin;
		if (p.y > h + 50) {		// 下まで行ったら上へ戻す 
			p.y = -50;
			p.x = Randomfloat(0, w);
		}
	}
}
//	描画
void LeafShower::Draw() const
{
	for (size_t i = 0; i < pieces.size(); i++) {
		const Piece& p = pieces[i];
		Leaf::Draw(p.type, p.x, p.y, p.size, p.ang, 230, true);
	}
}