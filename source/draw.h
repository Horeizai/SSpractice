#pragma once
#include "notes.h"
#include "load.h"

void drawLive(double currentTime, int darkLevel, int bgHandle, image_ image);
void drawStatus(int liveScene, double nowTime_status, char statusMessage[260]);
void drawEndMessage(void);
void drawErrowWindow(double nowTime_error, char errorMessage[260]);
void getWaveLength(double waveLength_);
void drawMusicData(image_ image, char title[260], char subTitle[260],int level, int imageHandle);
void drawScore(int x, int y, double totalScore, image_ image, bool drawGray);
void drawCombo(int x, int y, int combo, image_ image, bool flag);
