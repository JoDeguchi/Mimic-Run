#pragma once
#include "GameState.h"
#include "KeyReader.h"
#include "Graph.h"

class EndingState : public GameState
{
private:

    KeyReader key;

public:

    EndingState(GameManager* manager) : GameState(manager) {}

    void Init() override;
    void Input()override;
    void Update() override;
    void Draw() override;

};