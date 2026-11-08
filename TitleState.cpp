#include "TitleState.h"
#include "GameManager.h"
#include "StartGameState.h"


void TitleState::Init()
{
	//	コンテナを使って背景から順番に描画
	graphs.emplace_back("data/background.jpg", 0, 0);	//	背景
}

void TitleState::Input()
{
	key.Read();	//	キーの読み込み

	//	エンターキーでゲーム画面へ
	if (key.CheckReleaseKey(KEY_INPUT_SPACE)) {

		manager->ChangeState(
			std::make_unique<StartGameState>(manager)
		);
	}
}

void TitleState::Update()
{
	
}

void TitleState::Draw()
{
	for (const auto& g : graphs) {
		g.Draw();
	}

}

