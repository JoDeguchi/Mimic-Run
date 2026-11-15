#pragma once
#include "Leaf.h"

class MouseReader;
class Player;

/// <summary>
/// 画面上部の葉っぱパレット（5種類） 
///  葉っぱをマウスでドラッグして、プレイヤーの上で離すと着替える 
/// </summary>
class Palette
{
private:

	int dragIdx = -1;		// ドラッグ中の葉っぱ番号

	int labelImg[LEAF_COUNT];	// 名前札の画像ハンドル（読み込み失敗なら -1） 
	int labelW[LEAF_COUNT];		// 画像の元の幅 
	int labelH[LEAF_COUNT];		// 画像の元の高さ 

	int frameNormal = -1;		// 枠の画像（通常）
	int frameLight = -1;		// 枠の画像（選択中・光る）

	int  HitSlot(int mx, int my) const;							// (mx,my) にあるマスの番号。なければ -1 
	void DrawName(int i, int slotX, int slotY) const;			// 名前札を描く 
	void DrawFrame(int slotX, int slotY, bool selected) const;	// マスの枠を描く 

public:

	Palette();		// 名前札の画像を読み込む 
	~Palette();		// 画像を解放する 

	// 画像のハンドルを持つので、コピーは禁止 
	//	operatorは代入を意味する
	Palette(const Palette&) = delete;
	Palette& operator=(const Palette&) = delete;

	// 毎フレーム（Player の更新とカメラ計算のあとに呼ぶ） 
	void Update(const MouseReader& mouse, Player& player, float camX, float camY);

	// 描画（最後に呼ぶ） 
	void Draw(const Player& player, const MouseReader& mouse, float camX, float camY) const;

	bool IsDragging() const { return dragIdx >= 0; }

};