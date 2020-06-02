#include "main.h"
#include "load.h"
#include "keyboard.h"
#include "draw.h"
#include "live.h"

//define

//structs
typedef enum {
    maxCombo,
    wonderful,
    great,
    nice,
    bad,
    miss
}judgeRank_er;
static SAVEDATA savedata;

//variables
static config_ config;
static bool once = true;
static bool write = true;
static bool isBestScore = false;
static int score;
static int mark;
int comboInfo[6];
char title[260];
char filePath[260];
static int titleFont;
static int digit_s[8];
static bool notUseAutoMode;
static bool useSeparateJudge;

//functions
void updateDatFile(void);
void sendScoreToResult(char title_[260], int score_, int comboCount[6],int mark_, bool notUseAutoMode_, bool useSeparatejudge_);
void getFilePath(char path[260]);

//--------------------------------------------------------------------------------------------
void result_main(image_ image, sound_ sound)
{
    if (once) {
        titleFont = getFontHandle(resultTitleFont_e);
        config = getConfig();
    }
    if (write) {
        updateDatFile();
        write = false;
    }
	DrawExtendGraph(0, 0, 1280, 720, image.bg_result, 1);

    DrawFormatStringToHandle((1280 - GetDrawStringWidthToHandle(title, -1, titleFont)) / 2, 50, GetColor(255, 255, 255), titleFont, title);
    //タイトル以外は文字画像と数字画像を作る
    DrawGraph(400, 150, image.score, 1);
    drawScore(1100, 150, score, image, false);
    DrawGraph(400, 300, image.bestScore, 1);
    drawScore(1100, 300, savedata.bestScore, image, false);
    if (isBestScore) DrawGraph(970, 277, image.newRecord, 1);

    //DrawGraph(400, 400, image.life, 1);

    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 70);
    DrawBox(390, 520, 1200, 710, GetColor(0, 0, 0), 1);
    DrawBox(50 , 520, 360, 710, GetColor(0, 0, 0), 1);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
    DrawGraph(400, 530, image.judgeString_wonderful, 1);
    DrawGraph(400, 590, image.judgeString_great, 1);
    DrawGraph(400, 650, image.judgeString_nice, 1);
    DrawGraph(850, 530, image.judgeString_bad, 1);
    DrawGraph(850, 590, image.judgeString_miss, 1);
    //drawScreの悪用
    drawScore(810, 535, comboInfo[wonderful], image, false);
    drawScore(810, 595, comboInfo[great], image, false);
    drawScore(810, 655, comboInfo[nice], image, false);
    drawScore(1130, 535, comboInfo[bad], image, false);
    drawScore(1130, 595, comboInfo[miss], image, false);

    //コンボとクリアマーク
    DrawGraph(60, 525, image.maxCombo, 1);
    drawCombo(205, 575, comboInfo[maxCombo], image, false);
    DrawRotaGraph(205, 670, 1, 0,image.clearMark[mark], 1);

    //リロード
    if (CheckHitKey(KEY_INPUT_Q) == 1) {
        write = true;
        isBestScore = false;
        update_songBlock();
        getLiveScene(0);
        setScene(scene_live);
    }
    if (checkKey(KEY_INPUT_ESCAPE) == 1) {
        write = true;
        isBestScore = false;
        update_songBlock();
        resetMeasure();
        setScene(scene_songSelect);
    }
}
//--------------------------------------------------------------------------------------------

void updateDatFile(void) {
    FILE* fp;
    if ((fopen_s(&fp, filePath, "rb")) != 0 || fp == 0) { // "fpが0である可能性があります"というエラー対策
    }
    else {
        fread(&savedata, sizeof(savedata), 1, fp);
        fclose(fp);
    }
    if (!notUseAutoMode) return;//オート使った分際で記録を！？ｗｗｗ
    if (config.isV_SYNC) return;//判定がよくないので
    if (savedata.bestScore < score) {
        savedata.bestScore = score;
        isBestScore = true;
    }
    if(savedata.mark < mark) savedata.mark = mark;
    if ((fopen_s(&fp, filePath, "wb")) != 0 || fp == 0) { // "fpが0である可能性があります"というエラー対策
    }
    else {
        fwrite(&savedata, sizeof(savedata), 1, fp);
        fclose(fp);
    }
}

void sendScoreToResult(char title_[260], int score_, int comboCount[6], int mark_, bool notUseAutoMode_, bool useSeparatejudge_) {
    strcpyDx(title, title_);
    score = score_;
    mark = mark_;
    for (int i = 0; i < 6; i++)
    {
        comboInfo[i] = comboCount[i];
    }
    notUseAutoMode = notUseAutoMode_;
    useSeparateJudge = useSeparatejudge_;
}

void getFilePath(char path[260]) {
    strcpyDx(filePath, path);
}
