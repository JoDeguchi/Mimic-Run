#pragma once

// 葉っぱの種類（パレット・擬態・落ち葉・木で共通） 
enum LeafType {
	LEAF_GINKGO = 0,	// イチョウ 
	LEAF_GREEN,			// 普通の緑の葉 
	LEAF_MAPLE,			// もみじ 
	LEAF_SAKURA,		// 桜 
	LEAF_BROWN,			// 茶色の枯れ葉 
	LEAF_COUNT
};

// 葉っぱ1枚の描画を担当（形・色のデータは最初の使用時に自動初期化） 
class Leaf
{
public:
	// size = 葉の長さ / cx,cy = 葉の中心 / ang = 向き(ラジアン) 
	static void Draw(LeafType type, float cx, float cy, float size, float ang, int alpha = 255, bool detail = true);

	//	マイナスにならないようにunsigned
	static unsigned int FillColor(LeafType type);
	static unsigned int VeinColor(LeafType type);
	static const char* Name(LeafType type);
};