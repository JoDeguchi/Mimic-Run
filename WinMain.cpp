// =======================================
//  ライブラリ読込
// =======================================
		// ゲームクラス
#include "GameManager.h"
#include "TitleState.h"

//	定数の宣言
#define WINDOW_W 1280	// 画面サイズ　横
#define WINDOW_H 720	// 画面サイズ　縦

// ------------------------------
// WinMain
// ------------------------------
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	// =======================================
	//  初期化処理
	// =======================================
	// ログの書き出しを無効化
	SetOutApplicationLogValidFlag(FALSE);	// ログの書き出しを無効化

	// ウィンドウモードの設定
	ChangeWindowMode(TRUE);					// ウインドウモードを有効化

	// ウィンドウサイズとカラービットの設定
	SetGraphMode(WINDOW_W, WINDOW_H, 32);

	// ウィンドウのタイトル
	SetMainWindowText("神戸TECH Dxライブラリー");

	// 背景色の設定
	SetBackgroundColor(128, 128, 128);

	// ＤＸライブラリ初期化処理
	if (DxLib_Init() == -1)
	{
		return -1;							// エラーが起きたら直ちに終了
	}


	// ウィンドウ描画モード設定
	SetDrawScreen(DX_SCREEN_BACK);			// 裏画面に描画する


	//	ゲーム管理呼び出し
	GameManager::GetInstance()->ChangeState(
		std::make_unique<TitleState>(GameManager::GetInstance())
	);
	GameManager::GetInstance()->GameLoop();

	// =======================================
	//  終了処理
	// =======================================
	// ＤＸライブラリ使用の終了処理
	DxLib_End();							// ＤＸライブラリ使用の終了処理

	// ソフトの終了
	return 0;
}