#pragma once
#pragma once
#include "DxLib.h"
/// <summary>
/// キー（汎用できる実装多め）
/// </summary>
class KeyReader
{
private:
	char Key[256];		//	今フレーム用キーの配列
	char oldKey[256];	//	前フレーム用キーの配列

	bool keyPress = false;		//	キーが押されたか
public:

	//	コンストラクタ（キーをすべて初期化）
	KeyReader()
	{
		for (int cnt = 0; cnt < 256; cnt++) {
			Key[cnt] = 0;
		}
	}

	//	キーの読み込み
	void Read()
	{
		for (int cnt = 0; cnt < 256; cnt++)
		{
			oldKey[cnt] = Key[cnt];
		}
		//	すべてのキーの押下の読み込み
		GetHitKeyStateAll(Key);

		keyPress = false;	//	いったん初期化しておく

		for (int cnt = 0; cnt < 256; cnt++)
		{
			if (Key[cnt] != 0)
			{
				keyPress = true;
			}
		}

	}

	//	なにかでもキーを"押した瞬間"の結果を返す
	bool AnyKeyDown() const
	{
		return keyPress;
	}
	//	なにかでも"押したキーを離した瞬間"の結果を返す
	bool AnyKeyRelease() const
	{
		for (int i = 0; i < 256; i++) {
			if (Key[i] == 0 && oldKey[i] == 1) {
				return true;
			}
		}
		return false;
	}

	//	なにか特定のキーを"押した瞬間"を取得
	bool CheckPressKey(int arg_key_code) const
	{
		// 指定されたキーの状態をチェック
		if (this->Key[arg_key_code] == 1 && oldKey[arg_key_code] == 0) {
			// 押されていればtrueを返す
			return true;
		}
		// 押されていなければfalseを返す
		return false;
	}

	//	なにか特定のキーを"押している状態から離した瞬間"を取得
	bool CheckReleaseKey(int arg_key_code) const
	{
		// 指定されたキーの状態をチェック
		if (this->Key[arg_key_code] == 0 && oldKey[arg_key_code] == 1) {
			// 押されていればtrueを返す
			return true;
		}
		// 押されていなければfalseを返す
		return false;
	}
	// キーを押している間 true
	bool CheckHoldKey(int arg_key_code) const { return Key[arg_key_code] == 1; }
};