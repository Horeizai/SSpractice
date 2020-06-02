#pragma once

class image_ {
public:
	int number_combo[10];
	int number_score[10];
	int number_score_gray[10];
	int level[5];
	int combo;
	//live
	int notes_tap;
	int notes_tap_EX;
	int notes_hold;
	int notes_holdStrut;
	int notes_flick_L;
	int notes_flick_R;
	int notes_flick_U;
	int notes_flick_D;
	int judgeFrame;
	int spGage;
	int judgeString_wonderful;
	int judgeString_great;
	int judgeString_nice;
	int judgeString_bad;
	int judgeString_miss;
	int bg_live;
	//song_select
	int newMark;
	int trophy[5];
	int noimage_j;
	int noimage_b;
	int bg_songSelect;
	//result
	int clearMark[5];
	int score;
	int bestScore;
	int newRecord;
	int life;
	int maxCombo;
	int bg_result;
	//function
	void loadImage(image_* image);
};

class sound_ {
public:
	//live
	int tap_wonderful;
	int tap_great;
	int tap_nice;
	int tap_bad;
	int tap_Ex;
	int tap_none;
	int hold_strut;
	int hold_end;
	int flick;
	//other
	int move;
	int select;
	//function
	void loadSound(sound_* sound);
};

class config_ {
public:
	int windowWidth;
	bool isV_SYNC;
	bool AutoPlay;
	float HighSpeed;
	float HighSpeedStep;
	bool SeparateJudge;
	int Darkness;
	typedef struct {
		int L1;
		int L2;
		int R1;
		int R2;
	}key_t;
	//function
	void loadConfig(config_* config, const char* file_name);

	config_() {
		windowWidth = 1280;
		isV_SYNC = 0;
		AutoPlay = 1;
		HighSpeed = 5.0;
		HighSpeedStep = 1.0;
		SeparateJudge = 0;
		Darkness = 0;
	}
};
