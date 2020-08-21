#pragma once
#include "load.h"
#include "ssf_.h"

#define BUTTON_NUM 7 //”»’èƒ{ƒ^ƒ“‚Ì”
#define NOTES_NUM 1000 //notes‚ÌÅ‘å”

void copy_ssf(ssf_* ssf_L, sound_ sound);
void getScoreFromNotes(int* score, int comboCount[6], bool* notUseAutoMode_);
int getClearMark(void);

void updateNotes(double currentTime, double speed);
void calcThroughComboAndScore(double currentTime, bool autoMode);
void setup_notes();
void judge_notes(double currentTime, bool autoMode);
bool checkChartPlaying(void);
void draw_notes(double currentTime, image_ image);
