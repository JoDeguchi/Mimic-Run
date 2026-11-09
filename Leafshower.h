#pragma once
#include <vector>
#include "Leaf.h"

/// <summary>
/// 葉っぱが降ってくるお祝いの演出（クリア画面用） 
/// </summary>
class LeafShower
{
private:
	struct Piece
	{
		float x, y;
		float vx, vy;
		float ang, spin, size; 
		LeafType type;
	};
	//	配列
	std::vector<Piece> pieces;
	//	画面横幅
	float w = 1280, h = 720;

public:
	void Start(float screenW, float screenH);
	void Update();
	void Draw() const;


};