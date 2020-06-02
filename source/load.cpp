#include "main.h"
#include "load.h"
#include "ssf_.h"
#include "songSelect.h"

void image_::loadImage(image_* image) {
	LoadDivGraph("IMAGE/number_combo.png", 10, 10, 1, 43, 58, image->number_combo);
	LoadDivGraph("IMAGE/number_score.png", 10, 10, 1, 30, 40, image->number_score);
	LoadDivGraph("IMAGE/number_score_gray.png", 10, 10, 1, 30, 40, image->number_score_gray);
	image->combo = LoadGraph("IMAGE/combo.png");
	image->level[0] = LoadGraph("IMAGE/level_beginner.png");
	image->level[1] = LoadGraph("IMAGE/level_normal.png");
	image->level[2] = LoadGraph("IMAGE/level_advanced.png");
	image->level[3] = LoadGraph("IMAGE/level_expert.png");
	image->level[4] = LoadGraph("IMAGE/level_master.png");
	//live
	image->notes_tap = LoadGraph("IMAGE/notes_tap.png");
	image->notes_tap_EX = LoadGraph("IMAGE/notes_tap_EX.png");
	image->notes_hold = LoadGraph("IMAGE/notes_hold.png");
	image->notes_holdStrut = LoadGraph("IMAGE/notes_holdStrut.png");
	image->notes_flick_L = LoadGraph("IMAGE/notes_flick_L.png");
	image->notes_flick_R = LoadGraph("IMAGE/notes_flick_R.png");
	image->notes_flick_U = LoadGraph("IMAGE/notes_flick_U.png");
	image->notes_flick_D = LoadGraph("IMAGE/notes_flick_D.png");
	image->judgeFrame = LoadGraph("IMAGE/judgeFrame.png");
	image->spGage = LoadGraph("IMAGE/notes_tap.png");
	image->judgeString_wonderful = LoadGraph("IMAGE/judgeString_wonderful.png");
	image->judgeString_great = LoadGraph("IMAGE/judgeString_great.png");
	image->judgeString_nice = LoadGraph("IMAGE/judgeString_nice.png");
	image->judgeString_bad = LoadGraph("IMAGE/judgeString_bad.png");
	image->judgeString_miss = LoadGraph("IMAGE/judgeString_miss.png");
	image->bg_live = LoadGraph("IMAGE/bg_live.png");
	//songSelect
	image->newMark = LoadGraph("IMAGE/newMark.png");
	LoadDivGraph("IMAGE/trophy.png", 5, 5, 1, 40, 40, image->trophy);
	image->noimage_j = LoadGraph("IMAGE/noimage_j.png");
	image->noimage_b = LoadGraph("IMAGE/noimage_b.png");
	image->bg_songSelect = LoadGraph("IMAGE/bg_songSelect.png");
	//result
	LoadDivGraph("IMAGE/clearMark.png", 5, 1, 5, 200, 40, image->clearMark);
	image->score = LoadGraph("IMAGE/score.png");
	image->bestScore = LoadGraph("IMAGE/bestScore.png");
	image->newRecord = LoadGraph("IMAGE/newRecord.png");
	image->life = LoadGraph("IMAGE/life.png");
	image->maxCombo = LoadGraph("IMAGE/maxCombo.png");
	image->bg_result = LoadGraph("IMAGE/bg_result.png");
}

void sound_::loadSound(sound_* sound) {
	sound->tap_wonderful = LoadSoundMem("SOUND/Notes_Tap_wonderful.wav");
	sound->tap_great = LoadSoundMem("SOUND/Notes_Tap_great.wav");
	sound->tap_nice = LoadSoundMem("SOUND/Notes_Tap_nice.wav");
	sound->tap_bad = LoadSoundMem("SOUND/Notes_Tap_bad.wav");
	sound->tap_Ex = LoadSoundMem("SOUND/Notes_Tap_ex.wav");
	sound->tap_none = LoadSoundMem("SOUND/Notes_Tap_none.wav");
	sound->hold_end = LoadSoundMem("SOUND/Notes_Hold_end.wav");
	sound->hold_strut = LoadSoundMem("SOUND/Notes_Hold_strut.wav");
	sound->flick = LoadSoundMem("SOUND/Notes_Flick.wav");
}

void config_::loadConfig(config_* config, const char* file_name) {
	FILE* fp;

	if ((fopen_s(&fp, file_name, "r")) != 0 || fp == 0) { // "fp‚ª0‚Å‚ ‚é‰Â”\«‚ª‚ ‚è‚Ü‚·"‚Æ‚¢‚¤ƒGƒ‰[‘Îô
		return; // bool‚ÍC++
	}

	char str[260], * next_token = NULL;
	while ((fgets(str, 260, fp)) != NULL) {

		char* first = strtok_s(str, "=", &next_token);
		if (first == NULL) continue;
		char* second = strtok_s(NULL, "=", &next_token);
		if (second == NULL) continue;

		if (strcmp(first, "WindowWidth") == 0) {
			config->windowWidth = atoi(second);
		}
		else if (strcmp(first, "V_SYNC") == 0) {
			config->isV_SYNC = atoi(second);
		}
		else if (strcmp(first, "AutoPlay") == 0) {
			config->AutoPlay = atoi(second);
		}
		else if (strcmp(first, "HighSpeed") == 0) {
			config->HighSpeed = float(atof(second));
		}
		else if (strcmp(first, "HighSpeedStep") == 0) {
			config->HighSpeedStep = float(atof(second));
		}
		else if (strcmp(first, "SeparateJudge") == 0) {
			config->SeparateJudge = atoi(second);
		}
		else if (strcmp(first, "Darkness") == 0) {
			config->Darkness = atoi(second);
		}
	}
	fclose(fp);
	return;
}
