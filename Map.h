#pragma once
#include <vector>
#include "Leaf.h"

/// <summary>
/// マップ（地形・障害物・落ち葉・木）の生成と描画を担当するクラス 
///  - Build(stage) で作成（シード固定なので毎回同じ配置） 
///  - 描画は DrawGround → キャラ → DrawCanopy の順に呼ぶと、 
///    キャラが木の葉の下に隠れる表現になる（Draw() は両方まとめて呼ぶだけ） 
/// </summary>
class Map
{
public:
	Map() {}

	// 画面サイズ（カメラ計算と画面外カリングに使う） 
	void SetScreenSize(int w, int h) { screenW = w; screenH = h; }

	// ステージ作成（1 または 2） 
	void Build(int stage);

	// 描画（camX, camY = 画面左上に来るワールド座標） 
	void Draw(float camX, float camY) const { DrawGround(camX, camY); DrawCanopy(camX, camY); }
	void DrawGround(float camX, float camY) const;	// 地面・落ち葉・切り株・倒木・幹・ゴール 
	void DrawCanopy(float camX, float camY) const;	// 木の枝葉（キャラより手前） 

	// 注目点(tx, ty)を画面中央にしたときのカメラ位置（マップ外が映らないよう制限） 
	void CalcCamera(float tx, float ty, float& camX, float& camY) const;

	// ---- 当たり判定（次のPlayerクラス・敵クラスから使う）---- 
	// 円(x, y, r) が障害物にめり込んでいたら押し戻す。動いたら true 
	bool ResolveCollision(float& x, float& y, float r) const;
	// 円(x, y, r) が障害物と重なっているか（位置は変えない） 
	bool IsInsideObstacle(float x, float y, float r) const;
	// 擬態できる場所か（同じ種類の落ち葉だまり or 同じ葉の木の下） 
	bool IsCamouflaged(float x, float y, LeafType disguise) const;

	// ---- 情報 ---- 
	float GetWidth() const { return mapW; }
	float GetHeight() const { return mapH; }
	float GetStartX() const { return startX; }
	float GetStartY() const { return startY; }
	float GetGoalX() const { return goalX; }
	float GetGoalY() const { return goalY; }
	bool  IsGoal(float x, float y, float r = 44.0f) const;

private:
	struct FloorLeaf { float x, y, size, ang; LeafType type; };
	struct Blob { float x, y, r; unsigned int col; };
	struct Patch { float x, y, r; LeafType type; std::vector<FloorLeaf> leaves; };
	struct Tree { float x, y, trunkR, canopyR; LeafType leaf; std::vector<FloorLeaf> leaves; };
	struct Stump { float x, y, r; };
	struct LogObj { float x, y, hw, hh, ang; };
	struct Obstacle { bool box; float x, y, r, hw, hh, ang; };

	int   screenW = 1280, screenH = 720;
	float mapW = 0, mapH = 0;
	float startX = 0, startY = 0, goalX = 0, goalY = 0;

	std::vector<FloorLeaf> floorLeaves;
	std::vector<Blob>      blobs;
	std::vector<Patch>     patches;
	std::vector<Tree>      trees;
	std::vector<Stump>     stumps;
	std::vector<LogObj>    logs;
	std::vector<Obstacle>  obstacles;

	// 生成用 
	void FillCanopy(Tree& t, int cnt, float smin, float smax);
	void AddCircleObstacle(float x, float y, float r);
	void AddBoxObstacle(float x, float y, float hw, float hh, float ang);
	void AddPatch(float x, float y, float r, LeafType type);

	// 描画用 
	bool OnScreen(float sx, float sy, float margin) const;
	void DrawGoal(float sx, float sy) const;
};