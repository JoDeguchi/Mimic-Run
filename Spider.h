#pragma once
#include "Enemy.h"

/// <summary>
/// クモ：持ち場でじっと待ち、視界に入ると素早く飛びかかる。 
///  追いかけるのは短い間だけで、そのあとゆっくり持ち場へ戻る 
///  （PATROL = 待ち伏せ、CHASE = 飛びかかり、RETURN = 戻り） 
/// </summary>
class Spider : public Enemy
{
public:
	Spider(float x, float y);

	void Update(const Map& map, float px, float py, bool hidden) override;
	void Draw(float camX, float camY) const override;

private:
	float homeX, homeY;		// 待ち伏せする場所 
	int   timer = 0;		// 追跡・戻りの残りフレーム 
	int   cool = 0;			// 待機に戻ってから、次に反応するまでの休み 
};
