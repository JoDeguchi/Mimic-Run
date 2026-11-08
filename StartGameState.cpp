#include "StartGameState.h"
#include "EndingState.h"
#include "GameManager.h"

void StartGameState::Init()
{
	map.SetScreenSize(1280, 720);
	map.Build(1);                      // 1 or 2
	player.Init(map.GetStartX(), map.GetStartY());
	enemies.Build(1, map);

	camX = 0; camY = map.GetHeight() - 720;   // スタート側から表示
}

void StartGameState::Input()
{
	key.Read();	//	キーの読み込み
	mouse.Read();	//	マウス読み込み

	// 確認用：数字キーで着替え
	if (key.CheckPressKey(KEY_INPUT_1)) player.SetDisguise(LEAF_GINKGO);
	if (key.CheckPressKey(KEY_INPUT_2)) player.SetDisguise(LEAF_GREEN);
	if (key.CheckPressKey(KEY_INPUT_3)) player.SetDisguise(LEAF_MAPLE);
	if (key.CheckPressKey(KEY_INPUT_4)) player.SetDisguise(LEAF_SAKURA);
	if (key.CheckPressKey(KEY_INPUT_5)) player.SetDisguise(LEAF_BROWN);

	//	エンターキーでゲーム画面へ
	if (key.CheckReleaseKey(KEY_INPUT_RETURN)) {

		manager->ChangeState(
			std::make_unique<EndingState>(manager)
		);
	}
}

void StartGameState::Update()
{
	player.Update(key, map);
	player.SetCamouflaged(map.IsCamouflaged(player.GetX(), player.GetY(), player.GetDisguise()));
	map.CalcCamera(player.GetX(), player.GetY(), camX, camY);

	palette.Update(mouse, player, camX, camY);

	bool hidden = map.IsCamouflaged(player.GetX(), player.GetY(), player.GetDisguise());
	player.SetCamouflaged(hidden);
	enemies.Update(map, player.GetX(), player.GetY(), hidden);
	if (enemies.HitPlayer(player.GetX(), player.GetY(), player.GetRadius())) {
		Init();    // 捕まったので最初から
	}
	if (map.IsGoal(player.GetX(), player.GetY())) {
		// ゴール処理（あとでクリア画面へ） 
	}
}


void StartGameState::Draw()
{
	map.DrawGround(camX, camY);
	enemies.DrawGround(camX, camY);   // 視界・影・アリ・クモ
	player.Draw(camX, camY);
	map.DrawCanopy(camX, camY);
	player.DrawMarker(camX, camY);
	enemies.DrawAir(camX, camY);      // カナブンと「!」（一番手前）

	palette.Draw(player, mouse, camX, camY);
}










