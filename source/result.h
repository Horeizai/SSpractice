#pragma once
#include "load.h"

void result_main(image_ image, sound_ sound);
void sendScoreToResult(char title_[260], int score_, int comboCount[6], int mark_, bool notUseAutoMode_, bool useSeparatejudge_);
void getFilePath(char path[260]);
