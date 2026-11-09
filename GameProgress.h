#pragma once

/// <summary>
/// 持ち越す記録（かかった時間・捕まった回数） 
/// どのステートからでも GameProgress::Get() で同じものを使える 
/// </summary>
class GameProgress
{
private:
	GameProgress() {}
	int frames = 0;	//	かかった時間
	int deaths = 0;	//	捕まった回数

public:
	static GameProgress& Get();

	void Reset() { frames = 0; deaths = 0; }	// 最初のステージを始めるときに呼ぶ 
	void AddFrame() { frames++; }				// プレイ中は毎フレーム呼ぶ 
	void AddDeath() { deaths++; }

	int GetSeconds() const { return frames / 60; }
	int GetDeaths() const { return deaths; }

};