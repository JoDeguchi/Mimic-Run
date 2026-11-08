#pragma once
#include <memory>
#include <ctime>

class GameState;

/// <summary>
/// 所有者　マネージャークラス
/// </summary>
class GameManager
{
private:

	static GameManager* instance;
	std::unique_ptr<GameState> currentState;	//	所有権を持たせるからunique_ptr

	GameManager() {}

public:

	static GameManager* GetInstance()
	{
		if (!instance) {

			instance = new GameManager();
		}
		return instance;
	}
	
	void ChangeState(std::unique_ptr<GameState> next);

	//	ゲームループ
	void GameLoop();

};