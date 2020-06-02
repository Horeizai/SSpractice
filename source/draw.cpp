#include "main.h"
#include "notes.h"
#include "load.h"
#include "draw.h"

//private関数定義
void drawTimeBar(double currentTime);

//ライブ関連
double waveLength_;
double percentage,barLength;
void drawLive(double currentTime, int darkLevel, int bgHandle, image_ image) { //判定ライン描画
	if (bgHandle == -1) DrawExtendGraph(0, 0, 1280, 720, image.bg_live, true);
	else DrawExtendGraph(0, 0, 1280, 720, bgHandle, true);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(25.6 * darkLevel));//暗さ
	DrawBox(0, 0, 1280, 720, GetColor(0, 0, 0), 1);

	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 60);
	DrawQuadrangle(640, 640, 1140, 640, 1162, 712, 640, 712, GetColor(120, 140, 170), 1);//score
	DrawBox(0, 480, 400, 665, GetColor(120, 140, 170), 1);//musicData
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	draw_notes(currentTime, image);
	DrawCircleAA(640.0, 720.0, 188.0, 128, GetColor(255, 255, 255), TRUE);//SPゲージ
	drawTimeBar(currentTime);
}

void drawTimeBar(double currentTime) {
	percentage = currentTime / waveLength_;
	barLength = 773 * percentage;
	DrawBoxAA(253, 8, 1026, 12, GetColor(118, 118, 118), true);
	if (0.0 >= percentage) {
		DrawCircleAA(253, 10, 4, 32, GetColor(255, 255, 255), true);
	}
	if (0.0 <= percentage && percentage <= 1.0) {
		DrawBoxAA(253, 8, 253 + float(barLength), 12, GetColor(3, 224, 255), true);
		DrawCircleAA(253 + float(barLength), 10, 4, 32, GetColor(255, 255, 255), true);
	}
	if (percentage >= 1.0) {
		DrawBoxAA(253, 8, 1026, 12, GetColor(3, 224, 255), true);
		DrawCircleAA(1026, 10, 4, 32, GetColor(255, 255, 255), true);
	}
}
void getWaveLength(double waveLength) { waveLength_ = waveLength; }

void drawStatus(int liveScene, double nowTime_status, char statusMessage[260]){
	if (liveScene == 0) DrawFormatString((1280 - GetDrawStringWidth("NOW LOADING...", -1)) / 2, 340, GetColor(255, 255, 255), "NOW LOADING...");
	if (liveScene == 1) DrawFormatString((1280 - GetDrawStringWidth("PRESS SPACE KEY", -1)) / 2, 340, GetColor(255, 255, 255), "PRESS SPACE KEY");
	if (nowTime_status < 1.0) {
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
		DrawBox(3, 1, 7 + GetDrawStringWidth(statusMessage, -1), 35, GetColor(0, 0, 0), 1);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		DrawFormatString(5, 5, GetColor(255, 255, 255), statusMessage);
	}
}

void drawEndMessage(void) {
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBox(850, 600, 1155, 635, GetColor(0, 0, 0), 1);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	DrawString(855, 605, "Press Ctrl + SpaceKey", GetColor(255, 255, 255));
}

//songselect
void drawErrowWindow(double nowTime_error, char errorMessage[260]) {
	int darkLevel = 0;
	//濃さを変えるやつ
	{
		if (0.50 <= nowTime_error && nowTime_error <= 1.50) darkLevel = 10;
		else if (0.45 <= nowTime_error && nowTime_error <= 1.55) darkLevel = 9;
		else if (0.40 <= nowTime_error && nowTime_error <= 1.60) darkLevel = 8;
		else if (0.35 <= nowTime_error && nowTime_error <= 1.65) darkLevel = 7;
		else if (0.30 <= nowTime_error && nowTime_error <= 1.70) darkLevel = 6;
		else if (0.25 <= nowTime_error && nowTime_error <= 1.75) darkLevel = 5;
		else if (0.20 <= nowTime_error && nowTime_error <= 1.80) darkLevel = 4;
		else if (0.15 <= nowTime_error && nowTime_error <= 1.85) darkLevel = 3;
		else if (0.10 <= nowTime_error && nowTime_error <= 1.90) darkLevel = 2;
		else if (0.05 <= nowTime_error && nowTime_error <= 1.95) darkLevel = 1;
		//else if (0.00 <= nowTime_error && nowTime_error <= 2.00) darkLevel = 0;
	}
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(25.6 * 0.8 * darkLevel));
	DrawBox(400, 220, 880, 440, GetColor(0, 0, 0), 1);
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(25.6 * darkLevel));
	DrawFormatString(405, 315, GetColor(255, 255, 255), errorMessage);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
}


void drawMusicData(image_ image, char title[260], char subTitle[260],int level, int imageHandle) {
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawExtendGraph(225, 490, 390, 655, imageHandle, 1);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	DrawFormatString(5, 590, GetColor(255, 255, 255), title);
	DrawFormatString(5, 630, GetColor(255, 255, 255), subTitle);
	DrawGraph(10, 490, image.level[min(level,4)], 1);
	/*
	char levelString[16];
	convertLevelNumToLevelString(level, levelString);
	DrawFormatString(10, 490, GetColor(255, 255, 255), levelString);
	*/
}


void drawCombo(int x, int y, int combo, image_ image, bool flag) {
	int cnt;
	int div;
	cnt = 0;
	div = combo;
	int digit[5] = {0,0,0,0,0};
	while (div != 0) {
		if (flag) DrawRotaGraph(x, y + 80, 1, 0, image.combo, 1);
		digit[cnt] = div % 10;
		div = int(div / 10);
		cnt++;
	}
	switch (cnt) {
	case 0:
		if (!flag) DrawGraph(x - 21, y, image.number_combo[digit[0]], 1);
		break;
	case 1:
		DrawGraph(x - 21, y, image.number_combo[digit[0]], 1);
		break;
	case 2:
		DrawGraph(x - 43, y, image.number_combo[digit[1]], 1);
		DrawGraph(x     , y, image.number_combo[digit[0]], 1);
		break;
	case 3:
		DrawGraph(x - 64, y, image.number_combo[digit[2]], 1);
		DrawGraph(x - 21, y, image.number_combo[digit[1]], 1);
		DrawGraph(x + 22, y, image.number_combo[digit[0]], 1);
		break;
	case 4:
		DrawGraph(x - 86, y, image.number_combo[digit[1]], 1);
		DrawGraph(x - 43, y, image.number_combo[digit[1]], 1);
		DrawGraph(x     , y, image.number_combo[digit[0]], 1);
		DrawGraph(x + 43, y, image.number_combo[digit[0]], 1);
		break;
	default:
		break;
	}
}
void drawScore(int x, int y, double totalScore, image_ image, bool drawGray) {
	int cnt;
	int div;
	cnt = 0;
	div = int(round(totalScore));
	int digit[8] = { 0,0,0,0,0,0,0,0 };
	while (div != 0) {
		digit[cnt] = div % 10;
		div = int(div / 10);
		cnt++;
	}
	for (int i = 0; i < cnt; i++) {
		DrawGraph(x - 30 * i, y, image.number_score[digit[i]], 1);
	}
	if (drawGray) {
		for (int i = cnt; i < 8; i++) {
			DrawGraph(x - 30 * i, y, image.number_score_gray[digit[i]], 1);
		}
	}
	if (int(totalScore) == 0) DrawGraph(x, y, image.number_score[0], 1);
}
