#pragma once
#include "load.h"
#include "songSelect.h"

typedef enum {
	notPlay,
	played,
	clear,
	fullCombo,
	perfectLive
}mark_e;

void live_main(image_ image, sound_ sound);
void get_ssfInfo(SSFINFO ssfInfo);
void setStatus(config_ config);
void getLiveScene(int liveScene_);
void changeStatus(void);
void resetMeasure(void);
void getStatusMessage(char message[260], double* nowTime);
