#pragma once
#include "Leaf.h"

class KeyReader;
class Map;

/// <summary>
/// プレイヤー（コノハムシ） 
///  - WASD で8方向に移動（斜めでも速さは同じ） 
///  - 障害物との当たり判定は Map に任せる 
///  - 着替え（SetDisguise）で葉っぱの姿が変わる 
/// 1フレーム = 1/60秒 の固定ループ前提で、速度は「1フレームあたり」で持つ 
/// </summary>
class Player
{
public:
	Player();		// 「隠れ中」アイコンの画像を読み込む 
	~Player();		// 画像を解放する 

	// 画像のハンドルを持つので、コピーは禁止 
	Player(const Player&) = delete;
	Player& operator=(const Player&) = delete;

	// 位置を決めて、見た目を最初の状態（緑の葉）に戻す 
	void Init(float startX, float startY);

	// 入力と移動（毎フレーム呼ぶ） 
	void Update(const KeyReader& key, const Map& map);

	// 描画：Map::DrawGround のあと、Map::DrawCanopy の前に呼ぶ 
	void Draw(float camX, float camY) const;
	// 位置が分かる目印（頭上の矢印・足元の輪・隠れ中アイコン）。Map::DrawCanopy のあとに呼ぶ 
	void DrawMarker(float camX, float camY) const;

	// ---- 着替え ---- 
	void SetDisguise(LeafType type);
	LeafType GetDisguise() const { return disguise; }

	// 擬態中かどうか（Mapで判定した結果を渡すと、光る表示になる） 
	void SetCamouflaged(bool on) { camouflaged = on; }

	// 敵が近づいている強さ（0〜1）。隠れ中アイコンが、赤くなって震える 
	void SetTension(float t) { tension = t; }

	// ---- 情報 ---- 
	float GetX() const { return x; }
	float GetY() const { return y; }
	float GetRadius() const;
	// 画面上の点(sx, sy)がプレイヤーの近くか（ドラッグ着替えの判定用） 
	bool IsPointOver(int sx, int sy, float camX, float camY, float range = 40.0f) const;

private:
	float x = 0, y = 0;				// ワールド座標 
	float angle = 0;				// 向き（ラジアン） 
	LeafType disguise = LEAF_GREEN;	// 今の葉っぱ 
	float walkT = 0;				// 足のアニメ用 
	int   morphFrame = 0;			// 着替えエフェクトの残りフレーム 
	bool  camouflaged = false;
	int   animFrame = 0;			// 目印のアニメ用（毎フレーム増える） 

	int   hideIcon = -1;			// 「隠れ中」アイコンの画像ハンドル（読み込み失敗なら -1） 
	int   hideIconW = 0, hideIconH = 0;
	float tension = 0;				// 敵が近づいている強さ（0〜1） 
	float hideIconT = 0;			// アイコンの出方（0 = 消えている 〜 1 = 完全に出ている） 

	void DrawArrow(float sx, float sy) const;		// 頭上の矢印（DrawMarker から呼ぶ） 
	void DrawHideIcon(float sx, float sy) const;	// 右上の隠れ中アイコン（DrawMarker から呼ぶ） 
};