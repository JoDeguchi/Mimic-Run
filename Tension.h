#pragma once

class EnemyManager;

/// <summary>
/// 隠れているときに敵が近づいてくる「ドキドキ」の演出 
///  - 画面のふちが赤くなる（敵が近いほど濃く、鼓動に合わせて脈打つ） 
///  - プレイヤーの足元で、心臓のような輪がドクン・ドクンと広がる 
///  - 敵が近いほど鼓動が速くなる 
///  - 隠れていないとき（追われているとき）は出ない 
/// Update で強さ（0〜1）を計算して、Draw で描く。強さは Player にも渡せる 
/// </summary>
class TensionEffect
{
public:
	void Reset();	// ステージの開始・やり直しで呼ぶ 

	// hidden = 擬態中か。最も近い敵までの距離から、強さを決める 
	void Update(const EnemyManager& enemies, float px, float py, bool hidden);

	// 画面に描く（ゲーム画面を描いたあと、パレットなどの前に呼ぶ） 
	void Draw(float playerX, float playerY, float camX, float camY) const;

	// 今の強さ（0 = 何も起きていない 〜 1 = すぐそばに敵がいる） 
	float GetTension() const { return level; }

private:
	float level = 0;	// 強さ（なめらかに増減する） 
	float phase = 0;	// 鼓動の位置（0〜1 をくり返す） 

	float Beat() const;	// 今の鼓動の大きさ（0〜1）。ドクン・ドクンの形 
};