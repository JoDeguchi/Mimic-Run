#pragma once

class Map;
class EnemyManager;
class Player;

/// <summary>
/// ミニマップ（画面右下） 
///  - マップ全体を縮小して表示：落ち葉だまり・木・切り株・倒木 
///  - 敵（赤い点）・プレイヤー（白い点）・今の画面の範囲（白い枠） 
///  - ゴールは目立つ表現：脈打つ金色の星＋広がる輪＋「GOAL」の文字 
///    ＋プレイヤーからゴールへのてんせん 
/// 画面に固定で描くので、毎フレーム Draw を呼ぶだけでよい（一番手前に描く） 
/// </summary>
class Minimap
{
public:
	// 置き場所：false = 右下（初期値） / true = 右上 
	void SetAtTop(bool top) { atTop = top; }
	bool IsAtTop() const { return atTop; }

	// ミニマップの下端の y 座標（ほかの表示を、すぐ下に並べたいときに使う） 
	float GetBottom(const Map& map) const;

	void Draw(const Map& map, const EnemyManager& enemies, const Player& player,
		float camX, float camY) const;

private:
	bool atTop = false;
};