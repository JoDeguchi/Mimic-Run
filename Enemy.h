#pragma once

class Map;

/// <summary>
/// 敵の共通の土台クラス（カナブン・アリ・クモがこれを継承する） 
///  共通の仕事：見つける判定、追いかける移動、当たり判定、視界・「!」の表示 
///  種類ごとの動き(Update)と見た目(Draw)は、派生クラスが決める 
/// 1フレーム = 1/60秒 の固定ループ前提で、速度は「1フレームあたり」で持つ 
/// </summary>
class Enemy
{
public:
	enum State {
		PATROL = 0,		// 通常の動き（巡回・待ち伏せ） 
		CHASE,			// プレイヤーを追跡中 
		RETURN			// 持ち場へ戻る 
	};

	Enemy(float x, float y, float r, float vision)
		: x(x), y(y), r(r), vision(vision) {}
	virtual ~Enemy() {}

	// 毎フレームの動き（px,py = プレイヤー位置 / hidden = 擬態中で見えない） 
	virtual void Update(const Map& map, float px, float py, bool hidden) = 0;
	// 本体の描画 
	virtual void Draw(float camX, float camY) const = 0;
	// 地面に落ちる影（飛ぶ敵だけ使う） 
	virtual void DrawShadow(float camX, float camY) const {}
	// 飛んでいるか（true なら木の葉より手前に描く） 
	virtual bool IsFlying() const { return false; }

	void DrawVision(float camX, float camY) const;	// 視界の円 
	void DrawAlert(float camX, float camY) const;	// 追跡中の「!」 

	// プレイヤー(px,py,半径pr)に触れたか 
	bool HitPlayer(float px, float py, float pr) const;

	float GetX() const { return x; }
	float GetY() const { return y; }
	State GetState() const { return state; }

protected:
	bool  CanSee(float px, float py, bool hidden) const;
	// (tx,ty)へ speed だけ近づく。戻り値は残り距離 
	float MoveToward(float tx, float ty, float speed);

	float x, y;				// 位置 
	float r;				// 体の半径 
	float vision;			// 視界の半径 
	float angle = 0;		// 向き 
	float anim = 0;			// 足・羽のアニメ用 
	State state = PATROL;
	int   lostFrame = 0;	// 見失ってからのフレーム数 
};
