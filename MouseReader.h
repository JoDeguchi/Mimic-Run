#pragma once
#pragma once
#include "Dxlib.h"
/// <summary>
/// マウスクラス（汎用できる実装に）
/// </summary>
class MouseReader
{
private:
    int curMouse;     // 今フレームのマウス状態
    int oldMouse;     // 前フレームのマウス状態
    int mouseX;       // マウスX座標
    int mouseY;       // マウスY座標
public:
    //  コンストラクタ
    MouseReader()
        : curMouse(0), oldMouse(0), mouseX(0), mouseY(0)
    {
    }

    // マウス読み込み
    void Read()
    {
        oldMouse = curMouse;
        curMouse = GetMouseInput();
        GetMousePoint(&mouseX, &mouseY);
    }

    // 座標取得
    int GetX() const { return mouseX; }
    int GetY() const { return mouseY; }

    //----------------------------------------------------------------------------
    // 押した瞬間 それぞれ
    bool PressLeft() const
    {
        return (curMouse & MOUSE_INPUT_LEFT) && !(oldMouse & MOUSE_INPUT_LEFT);
    }

    bool PressRight() const
    {
        return (curMouse & MOUSE_INPUT_RIGHT) && !(oldMouse & MOUSE_INPUT_RIGHT);
    }

    bool PressMiddle() const
    {
        return (curMouse & MOUSE_INPUT_MIDDLE) && !(oldMouse & MOUSE_INPUT_MIDDLE);
    }
    //-----------------------------------------------------------------------------
    // 離した瞬間 それぞれ
    bool ReleaseLeft() const
    {
        return !(curMouse & MOUSE_INPUT_LEFT) && (oldMouse & MOUSE_INPUT_LEFT);
    }

    bool ReleaseRight() const
    {
        return !(curMouse & MOUSE_INPUT_RIGHT) && (oldMouse & MOUSE_INPUT_RIGHT);
    }

    bool ReleaseMiddle() const
    {
        return !(curMouse & MOUSE_INPUT_MIDDLE) && (oldMouse & MOUSE_INPUT_MIDDLE);
    }

};