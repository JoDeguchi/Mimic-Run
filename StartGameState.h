#pragma once
#include "GameState.h"
#include "KeyReader.h"
#include <vector>
#include "Map.h"
#include "Player.h"
#include "EnemyManager.h"
#include "MouseReader.h"
#include "Palette.h"
#include "LeafShower.h"
#include "Graph.h"

class StartGameState :public GameState
{
private:
    //  ゲーム本編
    enum Phase
    {
        PLAY,        //  プレイ
        CLEAR,       //  クリア
        GAMEOVER,   //　ゲームオーバー  
    };

    KeyReader key;
    Map map;
    Player player;
    EnemyManager enemies;
    MouseReader mouse;
    Palette palette;
    LeafShower shower;
 
    std::vector<GraphDraw> graphs;  //  描画する画像をまとめて

    //  プレイから
    Phase phase = PLAY;
    // 今の場面になってからのフレーム数 
    int   phaseFrame = 0;	
    // ステージ番号 
    int   stage = 1;			
    //  カメラ場所
    float camX = 0, camY = 0;

    int   fontM = -1, fontL = -1;
    // ステージの作り直し(最初も含む)
    void StartStage();			 


public:

    StartGameState(GameManager* mgr, int stage = 1);
    ~StartGameState();

    void Init() override;
    void Input()override;
    void Update() override;
    void Draw() override;

};

