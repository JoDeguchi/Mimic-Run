#include "Palette.h"
#include "MouseReader.h"
#include "Player.h"
#include "Leaf.h"
#include "DrawUtil.h"
#include <cstring>

using namespace DrawUtil;

namespace
{
	const int SCREEN_W = 1280;		// 中央ぞろえに使う画面の幅 
	const int SLOT = 72;		// マスの大きさ 
	const int GAP = 24;		// マスの間隔（名前札が重ならない幅） 
	const int TOP = 12;		// 上の余白 

	const float LABEL_W = 100.0f;	// 画面に表示する名前札の幅（画像の大きさに関係なくこの幅になる） 

	// 名前札の画像ファイル（LeafType の順番：イチョウ・緑の葉・もみじ・桜・枯れ葉） 
	const char* LABEL_FILE[LEAF_COUNT] = {
		"label_ginkgo.png",
		"label_green.png",
		"label_maple.png",
		"label_sakura.png",
		"label_brown.png"
	};

	// i 番目のマスの左上座標（5つを画面上部の中央に並べる） 
	void SlotPos(int i, int& x, int& y)
	{
		int total = LEAF_COUNT * SLOT + (LEAF_COUNT - 1) * GAP;
		x = (SCREEN_W - total) / 2 + i * (SLOT + GAP);
		y = TOP;
	}

	// 画像が読み込めなかったときの代わり：文字を黒フチつきで中央ぞろえに描く 
	void DrawLabel(int cx, int y, const char* s)
	{
		int x = cx - GetDrawStringWidth(s, (int)strlen(s)) / 2;
		DrawString(x + 1, y + 1, s, GetColor(0, 0, 0));
		DrawString(x, y, s, GetColor(235, 245, 225));
	}
}

Palette::Palette()
{
	for (int i = 0; i < LEAF_COUNT; i++) {
		labelW[i] = labelH[i] = 0;
		labelImg[i] = LoadGraph(LABEL_FILE[i]);		// 失敗したら -1 
		if (labelImg[i] >= 0) GetGraphSize(labelImg[i], &labelW[i], &labelH[i]);
	}
}

Palette::~Palette()
{
	for (int i = 0; i < LEAF_COUNT; i++) {
		if (labelImg[i] >= 0) DeleteGraph(labelImg[i]);
	}
}

int Palette::HitSlot(int mx, int my) const
{
	for (int i = 0; i < LEAF_COUNT; i++) {
		int x, y; SlotPos(i, x, y);
		if (mx >= x && mx < x + SLOT && my >= y && my < y + SLOT) return i;
	}
	return -1;
}

void Palette::Update(const MouseReader& mouse, Player& player, float camX, float camY)
{
	// 押した瞬間：マスの上なら、その葉っぱを持つ 
	if (mouse.PressLeft()) {
		dragIdx = HitSlot(mouse.GetX(), mouse.GetY());
	}
	// 離した瞬間：プレイヤーの上なら着替える（どこで離しても、持っているのは終わる） 
	if (mouse.ReleaseLeft()) {
		if (dragIdx >= 0 && player.IsPointOver(mouse.GetX(), mouse.GetY(), camX, camY)) {
			player.SetDisguise((LeafType)dragIdx);
		}
		dragIdx = -1;
	}
}

void Palette::DrawName(int i, int slotX, int slotY) const
{
	float cx = slotX + SLOT * 0.5f;

	// 画像がなければ文字で代用 
	if (labelImg[i] < 0) {
		DrawLabel((int)cx, slotY + SLOT + 2, Leaf::Name((LeafType)i));
		return;
	}

	// 幅が LABEL_W になる倍率で、マスの真下に中央ぞろえで描く 
	float rate = LABEL_W / (float)labelW[i];
	float cy = slotY + SLOT + 2 + labelH[i] * rate * 0.5f;

	SetDrawMode(DX_DRAWMODE_BILINEAR);			// 縮小をなめらかに 
	DrawRotaGraphF(cx, cy, rate, 0.0, labelImg[i], TRUE);
	SetDrawMode(DX_DRAWMODE_NEAREST);
}

void Palette::Draw(const Player& player, const MouseReader& mouse, float camX, float camY) const
{
	// ---- マス ---- 
	for (int i = 0; i < LEAF_COUNT; i++) {
		int x, y; SlotPos(i, x, y);
		bool selected = (player.GetDisguise() == (LeafType)i);

		Blend(150);
		DrawBox(x, y, x + SLOT, y + SLOT, GetColor(10, 24, 10), TRUE);
		NoBlend();

		unsigned int frame = selected ? GetColor(255, 220, 80) : GetColor(150, 180, 150);
		DrawBox(x, y, x + SLOT, y + SLOT, frame, FALSE);
		if (selected) DrawBox(x + 1, y + 1, x + SLOT - 1, y + SLOT - 1, frame, FALSE);	// 選択中は枠を太く 

		// ドラッグ中の葉っぱは、元の場所をうすくする 
		Leaf::Draw((LeafType)i, x + SLOT * 0.5f, y + SLOT * 0.5f, 50, 0.0f, (dragIdx == i) ? 90 : 255, true);
		DrawName(i, x, y);
	}

	// ---- ドラッグ中 ---- 
	if (dragIdx >= 0) {
		int mx = mouse.GetX(), my = mouse.GetY();

		// プレイヤーを囲む輪（近づけると黄色く太くなる） 
		float px = player.GetX() - camX, py = player.GetY() - camY;
		bool hover = player.IsPointOver(mx, my, camX, camY);
		float t = GetNowCount() / 1000.0f;
		float ringR = 34.0f + sinf(t * 10.0f) * 3.0f;
		DrawCircleAA(px, py, ringR, 32,
			hover ? GetColor(255, 255, 120) : GetColor(255, 255, 255), FALSE, hover ? 3.0f : 1.5f);

		// マウスの位置に、持っている葉っぱを描く 
		Leaf::Draw((LeafType)dragIdx, (float)mx, (float)my, 56, 0.4f, 235, true);
	}
}