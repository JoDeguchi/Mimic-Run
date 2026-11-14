#include "StartGameState.h"
#include "TitleState.h"
#include "GameManager.h"
#include "GameProgress.h"
#include <cstdio>
#include <cstring>

namespace
{
	//	画面幅
	const int SCREEN_W = 1280;			
	const int SCREEN_H = 720;
	// ステージ数 
	const int MAX_STAGE = 2;	
	// クリア後待ち時間
	const int CLEAR_WAIT = 40;		
	//	捕まったと表示される時間
	const int CAUGHT_FRAME = 78;		 
}

//	コンストラクタ
StartGameState::StartGameState(GameManager* mgr, int stage)
	: GameState(mgr), stage(stage)
{
	fontM = CreateFontToHandle("メイリオ", 24, 3, DX_FONTTYPE_ANTIALIASING_EDGE);
	fontL = CreateFontToHandle("メイリオ", 60, 4, DX_FONTTYPE_ANTIALIASING_EDGE);
	if (stage == 1) GameProgress::Get().Reset();	// 1面から始めるときは記録をリセット 
}
//	デストラクタ
StartGameState::~StartGameState()
{
	DeleteFontToHandle(fontM);
	DeleteFontToHandle(fontL);
}
//	初期化
void StartGameState::Init()
{
	StartStage();
	graphs.emplace_back("operation_explain.png", 0, 550);	//	操作説明画像
	graphs.emplace_back("purpose.png", 0, 70);	//	目的画像
}

//	ステージ配置
void StartGameState::StartStage()
{
	//	画面サイズに合わせる
	map.SetScreenSize(SCREEN_W, SCREEN_H);	
	//	ステージ１から
	map.Build(stage);	
	//	プレイヤーとマップと敵を配置
	player.Init(map.GetStartX(), map.GetStartY());
	enemies.Build(stage, map);
	map.CalcCamera(player.GetX(), player.GetY(), camX, camY);
	//	プレイから
	phase = PLAY;	
	phaseFrame = 0;
}

void StartGameState::Input()
{
	key.Read();
	mouse.Read();

	// クリア画面：Enter で次のステージ or エンディング 
	if (phase == CLEAR && phaseFrame > CLEAR_WAIT && key.CheckPressKey(KEY_INPUT_RETURN)) {
		if (stage < MAX_STAGE) {
			manager->ChangeState(std::make_unique<StartGameState>(manager, stage + 1));
		}
		else {
			manager->ChangeState(std::make_unique<TitleState>(manager));
		}                                                                                                                                                           
		return;		// ChangeState のあとは、このオブジェクトはもう消えているので何も触らない 
	}
}

void StartGameState::Update()
{
	phaseFrame++;

	if (phase == CLEAR) {
		shower.Update();
		return;
	}
	if (phase == GAMEOVER) {
		if (phaseFrame >= CAUGHT_FRAME) StartStage();	// やり直し 
		return;
	}

	// ---- PLAY ----
	GameProgress::Get().AddFrame();

	player.Update(key, map);
	bool hidden = map.IsCamouflaged(player.GetX(), player.GetY(), player.GetDisguise());
	player.SetCamouflaged(hidden);
	enemies.Update(map, player.GetX(), player.GetY(), hidden);
	map.CalcCamera(player.GetX(), player.GetY(), camX, camY);
	palette.Update(mouse, player, camX, camY);

	// 敵に捕まった？ 
	if (enemies.HitPlayer(player.GetX(), player.GetY(), player.GetRadius())) {
		GameProgress::Get().AddDeath();
		phase = GAMEOVER;
		phaseFrame = 0;
		return;
	}
	// ゴールに着いた？ 
	if (map.IsGoal(player.GetX(), player.GetY())) {
		phase = CLEAR;
		phaseFrame = 0;
		shower.Start((float)SCREEN_W, (float)SCREEN_H);
	}
}

namespace
{
	// 画面中央ぞろえで文字を描く 
	void DrawCenter(int y, const char* s, unsigned int col, int font)
	{
		int w = GetDrawStringWidthToHandle(s, (int)strlen(s), font);
		DrawStringToHandle((SCREEN_W - w) / 2, y, s, col, font, GetColor(0, 0, 0));
	}
}

void StartGameState::Draw()
{
	// ---- ゲーム画面（どの場面でも同じ順番で描く）---- 
	map.DrawGround(camX, camY);
	enemies.DrawGround(camX, camY);
	player.Draw(camX, camY);
	map.DrawCanopy(camX, camY);
	player.DrawMarker(camX, camY);
	enemies.DrawAir(camX, camY);

	char buf[128];
	GameProgress& prog = GameProgress::Get();

	if (phase == PLAY) {
		palette.Draw(player, mouse, camX, camY);
		snprintf(buf, sizeof(buf), "STAGE %d / %d", stage, MAX_STAGE);
		DrawStringToHandle(14, 10, buf, GetColor(255, 255, 255), fontM, GetColor(0, 0, 0));
		snprintf(buf, sizeof(buf), "%02d:%02d", prog.GetSeconds() / 60, prog.GetSeconds() % 60);
		DrawStringToHandle(14, 42, buf, GetColor(255, 255, 255), fontM, GetColor(0, 0, 0));
	}
	else if (phase == GAMEOVER) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 90);
		DrawBox(0, 0, SCREEN_W, SCREEN_H, GetColor(120, 0, 0), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		DrawCenter(SCREEN_H / 2 - 40, "見つかった！", GetColor(255, 220, 200), fontL);
	}
	else {	// CLEAR
		// 暗くなっていく（約0.5秒で最大） 
		int a = phaseFrame < 30 ? phaseFrame * 110 / 30 : 110;
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, a);
		DrawBox(0, 0, SCREEN_W, SCREEN_H, GetColor(0, 30, 0), TRUE);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		shower.Draw();

		DrawCenter(SCREEN_H / 2 - 110, "STAGE CLEAR!", GetColor(255, 240, 150), fontL);
		snprintf(buf, sizeof(buf), "ここまでの時間 %02d:%02d　　見つかった回数 %d",
			prog.GetSeconds() / 60, prog.GetSeconds() % 60, prog.GetDeaths());
		DrawCenter(SCREEN_H / 2 + 10, buf, GetColor(255, 255, 255), fontM);
		if (phaseFrame > CLEAR_WAIT && (phaseFrame / 30) % 2 == 0) {
			DrawCenter(SCREEN_H / 2 + 70,
				stage < MAX_STAGE ? "Enter キーで次のステージへ" : "Enter キーでタイトルへ",
				GetColor(255, 255, 255), fontM);
		}
	}

	//	UI画像をまとめて描画
	for (const auto& g : graphs) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 220);
		g.Draw();
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}