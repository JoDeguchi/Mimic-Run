#pragma once
#include <vector>
#include <memory>
#include "Enemy.h"

class Map;

/// <summary>
/// 敵をまとめて管理するクラス 
///  - Build でステージに合わせて敵を配置（位置は毎回同じ） 
///  - 更新・当たり判定・描画を全員分まとめて行う 
/// </summary>
class EnemyManager
{
public:
	void Build(int stage, const Map& map);

	// px,py = プレイヤー位置 / hidden = 擬態中（敵に見つからない） 
	void Update(const Map& map, float px, float py, bool hidden);

	// プレイヤーに触れた敵がいるか 
	bool HitPlayer(float px, float py, float pr) const;

	// 描画：map.DrawGround → DrawGround → player.Draw → map.DrawCanopy → DrawAir の順 
	void DrawGround(float camX, float camY) const;	// 視界・影・地上の敵（アリ・クモ） 
	void DrawAir(float camX, float camY) const;		// 飛ぶ敵（カナブン）と「!」 

	size_t Count() const { return enemies.size(); }
	const Enemy& Get(size_t i) const { return *enemies[i]; }

private:
	std::vector<std::unique_ptr<Enemy>> enemies;
};
