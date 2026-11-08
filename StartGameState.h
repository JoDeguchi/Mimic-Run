#pragma once
#include "GameState.h"
#include "KeyReader.h"
#include <vector>
#include "Map.h"
#include "Player.h"
#include "EnemyManager.h"
#include "MouseReader.h"
#include "Palette.h"

class StartGameState :public GameState
{
private:
    KeyReader key;
    Map map;
    Player player;
    EnemyManager enemies;
    MouseReader mouse;
    Palette palette;

    float camX = 0, camY = 0;
public:

    StartGameState(GameManager* mgr) : GameState(mgr) {}

    void Init() override;
    void Input()override;
    void Update() override;
    void Draw() override;

};

