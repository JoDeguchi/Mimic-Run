#pragma once
#include "DxLib.h"
#include <cmath>

// 描画まわりの小さな共通関数（Leaf / Map で共用） 
namespace DrawUtil
{
	const float PI =  3.14159265f;	//	180度
	const float PI2 = 6.28318531f;	//	360度

	//----------------------------------------------------------------------
	//	inlineをつけることで関数を呼び出す処理を省略できる”可能性”がある
	//----------------------------------------------------------------------
	//	角度や配置場所などをランダム範囲指定
	inline float Randomfloat(float a, float b)
	{
		//	最小がaとしてbが最大としたら
		//	b-aがその間　ランダム精度高い10000で0.0f～1.0ｆに収める
		return a + (b - a) * (float)GetRand(10000) / 10000.0f;
	}
	//	点と点の距離を求める
	inline float Dist(float ax, float ay, float bx, float by)
	{
		float dx = ax - bx;
		float dy = ay - by; 
		return sqrtf(dx * dx + dy * dy);
	}
	//	範囲からはみ出さないようにする
	inline float Clampf(float v, float low, float high)
	{
		if (v < low) return low;
		else if (v > high) return high;
		else return v;
	}
	//	透明処理
	inline void Blend(int a) { SetDrawBlendMode(DX_BLENDMODE_ALPHA, a); }
	//	透明を終了させる
	inline void NoBlend() { SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0); }

	// 葉っぱや虫などの回転用
	struct Transform {
		float cx, cy ;  //	物の中心座標
		float c, s;		//	cos sinの代わり
		//	中心を基準として２次元の回転行列の公式でもとめる
		//	X=xcosθ-ysinθ		X=xcosθ+ysinθ
		void Point(float x, float y, float& ox, float& oy) const {
			ox = cx + x * c - y * s;
			oy = cy + x * s + y * c;
		}
	};
	//	Transformの値代入
	inline Transform MakeXf(float cx, float cy, float ang)
	{
		Transform f; 
		f.cx = cx;			f.cy = cy; 
		f.c = cosf(ang);	f.s = sinf(ang); 
		return f;
	}

	//	塗りつぶしの楕円（楕円は半径がちがうからxyが値違う）
	//	中心lx,ly　楕円半径 rx,ry 
	inline void FillEllipse(const Transform& f, float lx, float ly, 
		float rx, float ry, unsigned int col) 
	{
		const int N = 18;	//	18個の三角形でわけて楕円を作る

		//	lxlyを中心座標として回転・移動
		float ox, oy, ax[N], ay[N];
		f.Point(lx, ly, ox, oy);

		for (int i = 0; i < N; i++) {
			float t = PI2 * i / N;	//	２π(360)を18で割る　20度ずつ
			f.Point(lx + rx * cosf(t), ly + ry * sinf(t), ax[i], ay[i]);
		}
		//	三角形の描画
		for (int i = 0; i < N; i++) {
			int j = (i + 1) % N;	//	あまりをもとめてちょっとずつ三角形描画
			DrawTriangleAA(ox, oy, ax[i], ay[i], ax[j], ay[j], col, TRUE);
		}
	}

	//	線
	//	構造体データを参照してコピーする手間を省く
	inline void LineL(const Transform& f, float x1, float y1,
		float x2, float y2, unsigned int col, float thick)
	{
		float a, b, c, d;
		f.Point(x1, y1, a, b);
		f.Point(x2, y2, c, d);
		DrawLineAA(a, b, c, d, col, thick);
	}
}