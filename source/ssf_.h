#pragma once
#include "songSelect.h"

#define NOTES_NUM 1000 //notes�̍ő吔
typedef struct {
	double times_L[NOTES_NUM];
	double times_R[NOTES_NUM];
	int connect_L[NOTES_NUM];
	int connect_R[NOTES_NUM];
}hold_;

void loadGenreIni(GENREINFO* genreInfo, const char* file_name); 
void loadSongData(SSFINFO* ssfInfo, const char* file_name);

class ssf_
{
public:
	char title[260], subTitle[260];
	int level;
	double bpm;
	char wavePath[260];
	char jacketPath[260];
	char bgPath[260];
	char moviePath[260];
	int songVolume;
	int seVolume;
	double offset;
	double demoStart;
	double perfect_times_L[NOTES_NUM];
	double perfect_times_R[NOTES_NUM];
	int perfect_timeSize_L;
	int perfect_timeSize_R;
	int notesType_L[NOTES_NUM];
	int notesType_R[NOTES_NUM];
	hold_ hold;
	double notesSpeed_L[NOTES_NUM];
	double notesSpeed_R[NOTES_NUM];
	int cnt_measure;
	double measureTime[NOTES_NUM];
	bool loadHumenOptions(ssf_* ssf, FILE* fp);
	void loadHumen(ssf_* ssf, FILE* fp);
	bool loadHumenData(ssf_* ssf, const char* file_name);

	ssf_() {
		title[0] = 0;
		subTitle[0] = 0;
		level = 0;
		bpm = 120.0;
		wavePath[0] = 0;
		jacketPath[0] = 0;
		bgPath[0] = 0;
		moviePath[0] = 0;
		songVolume = 100;
		seVolume = 100;
		offset = 0.0;
		demoStart = 0.0;
		perfect_times_L[0] = 0;
		perfect_times_R[0] = 0;
		perfect_timeSize_L = 0;
		perfect_timeSize_R = 0;
		notesType_L[0] = 0;
		notesType_R[0] = 0;
		hold.times_L[0] = 0;
		hold.times_R[0] = 0;
		hold.connect_L[0] = 0;
		hold.connect_R[0] = 0;
		notesSpeed_L[0] = 0;
		notesSpeed_R[0] = 0;
		cnt_measure = 0;
		measureTime[0] = 0;
	}
};
