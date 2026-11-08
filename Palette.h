#pragma once

class MouseReader;
class Player;

/// <summary>
/// 画面上部の葉っぱパレット（5種類） 
///  葉っぱをマウスでドラッグして、プレイヤーの上で離すと着替える 
///  - Update : ドラッグの開始・終了の判定 
///  - Draw   : パレットとドラッグ中の葉っぱの描画（一番手前に描く） 
/// </summary>
class Palette
{
public:
	// 毎フレーム（Player の更新とカメラ計算のあとに呼ぶ） 
	void Update(const MouseReader& mouse, Player& player, float camX, float camY);

	// 描画（最後に呼ぶ） 
	void Draw(const Player& player, const MouseReader& mouse, float camX, float camY) const;

	bool IsDragging() const { return dragIdx >= 0; }

private:
	int dragIdx = -1;		// ドラッグ中の葉っぱ番号（-1 = ドラッグしていない） 

	int HitSlot(int mx, int my) const;	// (mx,my) にあるマスの番号。なければ -1 
};
