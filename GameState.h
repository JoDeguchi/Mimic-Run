#pragma once
#include <string>
#include "Dxlib.h"

class GameManager;

class GameState
{
protected:
    //  GameManager側が所有者だからこっちはポインタのみでいい
    GameManager* manager;
public:

    GameState(GameManager* mgr):manager(mgr)
    { 
    }

    virtual ~GameState() = default;
    virtual void Init() = 0;
    virtual void Input() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
  
};