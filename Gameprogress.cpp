#include "GameProgress.h"

GameProgress& GameProgress::Get()
{
	static GameProgress instance;
	return instance;
}