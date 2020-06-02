#pragma once
#include "load.h"

void song_select_main(image_ image,sound_ sound);
void update_songBlock(void);

typedef struct {
	char dirPath[260];
	char fileName[260];
	char datName[260];
	char title[260];
	char subTitle[260];
	char designer[260];
	char bpm[130];
	int level;
	char wavePath[260];
	char jacketPath[260];
	char bgPath[260];
	int waveHandle, jacketHandle, bgHandle;
	int songVolime;
	double demoStart;
}SSFINFO;

typedef struct {
	char pass[260];
	char genreName[260];
	unsigned int genreColor;
	unsigned int fontColor;
}GENREINFO;

typedef struct {
	int bestScore;
	int mark;
}SAVEDATA;

typedef struct {
	SSFINFO info;
	SAVEDATA dat;
	int x;
	int y;
	int genreNumber;
	char title[260];
}SONGBLOCK;
