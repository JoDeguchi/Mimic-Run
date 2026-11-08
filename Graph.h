#pragma once
#include "DxLib.h"
#include <string>
#include <utility>

// 画像描画用クラス
class GraphDraw
{
protected:
	int graph_hnd = -1;          // 画像ハンドル(-1 = 未読み込み)
	std::string file_name = "";  // ファイル名
	int pos_x = 0;
	int pos_y = 0;

public:
	// コンストラクタ
	GraphDraw() = default;
	GraphDraw(const std::string& arg_file_name, int x, int y)
	{
		Load(arg_file_name);
		SetPos(x, y);
	}

	// ハンドル二重解放を防ぐためコピー禁止
	GraphDraw(const GraphDraw&) = delete;
	GraphDraw& operator=(const GraphDraw&) = delete;

	// ムーブは許可(vector や代入で使える)
	GraphDraw(GraphDraw&& other) noexcept
		: graph_hnd(other.graph_hnd), file_name(std::move(other.file_name)),
		pos_x(other.pos_x), pos_y(other.pos_y)
	{
		other.graph_hnd = -1;   // 移動元は空にする
	}

	//	operatorはムーブ代入　otherが引越し元、thisが引越し先
	//	noeceptは例外を投げない（安全設計）
	GraphDraw& operator=(GraphDraw&& other) noexcept
	{
		if (this != &other) {
			Release();	//	元の画像を先に解放
			graph_hnd = other.graph_hnd;
			file_name = std::move(other.file_name);
			pos_x = other.pos_x; pos_y = other.pos_y;
			other.graph_hnd = -1;
		}
		return *this;
	}

	//	デストラクタ
	~GraphDraw() { Release(); }

	// 画像読み込み(成功したら true)
	bool Load(const std::string& arg_file_name)
	{
		Release();  // 既に読み込み済みなら先に解放
		file_name = arg_file_name;
		graph_hnd = LoadGraph(file_name.c_str());
		return graph_hnd != -1;
	}

	void SetPos(int x, int y) { pos_x = x; pos_y = y; }
	void Move(int mvx, int mvy) { pos_x += mvx; pos_y += mvy; }

	//	一度だけ描画
	void Draw() const
	{
		if (graph_hnd != -1)
		{
			DrawGraph(pos_x, pos_y, graph_hnd, TRUE);
		}
	}

	//	一度でも描画成功したらそれで終了
	void Release()
	{
		if (graph_hnd != -1)
		{
			DeleteGraph(graph_hnd);
			graph_hnd = -1;
		}
	}
};