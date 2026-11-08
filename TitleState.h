#pragma once

#include "GameState.h"
#include "KeyReader.h"
#include "Graph.h"
#include <vector>

class TitleState :public GameState
{
private:
	KeyReader key;
	
	std::vector<GraphDraw> graphs;   // 描画する画像をまとめて管理
public:

	TitleState(GameManager* mgr) : GameState(mgr) {}

	void Init() override;
	void Input() override;
	void Update()override;
	void Draw()override;
};