#pragma once
#include "Leaf.h"

class MouseReader;
class Player;

/// <summary>
/// 画面上部の葉っぱパレット（5種類） 
///  葉っぱをマウスでドラッグして、プレイヤーの上で離すと着替える 
///  - Update : ドラッグの開始・終了の判定 
///  - Draw   : パレットとドラッグ中の葉っぱの描画（一番手前に描く） 
///  葉っぱの下の名前は画像（名前札）で表示する。画像がなければ文字で代用する 
/// </summary>
class Palette
{
public:
	Palette();		// 名前札の画像を読み込む 
	~Palette();		// 画像を解放する 

	// 画像のハンドルを持つので、コピーは禁止 
	Palette(const Palette&) = delete;
	Palette& operator=(const Palette&) = delete;

	// 毎フレーム（Player の更新とカメラ計算のあとに呼ぶ） 
	void Update(const MouseReader& mouse, Player& player, float camX, float camY);

	// 描画（最後に呼ぶ） 
	void Draw(const Player& player, const MouseReader& mouse, float camX, float camY) const;

	bool IsDragging() const { return dragIdx >= 0; }

private:
	int dragIdx = -1;		// ドラッグ中の葉っぱ番号（-1 = ドラッグしていない） 

	int labelImg[LEAF_COUNT];	// 名前札の画像ハンドル（読み込み失敗なら -1） 
	int labelW[LEAF_COUNT];		// 画像の元の幅 
	int labelH[LEAF_COUNT];		// 画像の元の高さ 

	int  HitSlot(int mx, int my) const;			// (mx,my) にあるマスの番号。なければ -1 
	void DrawName(int i, int slotX, int slotY) const;	// i 番目のマスの下に名前札を描く 
};