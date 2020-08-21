#include "main.h"
#include "keyboard.h"
#include "draw.h"
#include "songSelect.h"
#include "live.h"
#include "notes.h"
#include "result.h"
#include "ssf_.h"
#include "load.h"

SSFINFO ssfInfo;
LONGLONG start_count; //開始した時刻
LONGLONG now_count; //現在のフレームの時刻
double currentTime; //開始してからの経過時間
double openingDelay = 1.5;
double addTime = 0;

typedef enum {
    reload,
    loading = 0,
    standby,
    waiting,
    playing
}liveScene_e;
int liveScene = loading;
bool isReload = false;

//function
void moveMeasure(void);
void drawMeasure(void);
void goToResultProcess(void);

//status
bool init = true;
static config_ config;
bool autoMode;
float speed;
float speedStep;
bool separateFlag;
int darkLevel;
LONGLONG startCount_status; //ステータス変更を開始した時刻
double nowTime_status;
char statusMessage[260];

//
int jacketHandle;
int bgHandle;
int movieHandle;
int waveHandle;
double waveLength;
char path_ssf[260], path_jacket[260], path_bg[260], path_movie[260], path_wave[260];

//result
bool isSongPlaying;
bool isChartPlaying;
static bool once = true;
double chartEndTime = 0;
int timecount = 0;
static int score_;
static int combo_[6];
static int mark_;
bool notUseAutoMode_, useSeparatejudge_;

int measure = 0;
ssf_ ssf;
//ssf_* ssf = new ssf_;

void live_main(image_ image, sound_ sound) {
    switch (liveScene)
    {
    case loading: {
        //別の譜面のデータが入るの防止
        for (int i = 0; i < 260; i++)
        {
            ssf.songVolume = 100;
            ssf.seVolume = 100;
            ssf.jacketPath[i] = 0;
            ssf.bgPath[i] = 0;
            ssf.moviePath[i] = 0;
            ssf.wavePath[i] = 0;
        }

        sprintfDx(path_ssf, "%s\\%s", ssfInfo.dirPath, ssfInfo.fileName);
        ssf.loadHumenData(&ssf, path_ssf);
        copy_ssf(&ssf, sound);
        setup_notes();
        currentTime = addTime;
        SetSoundCurrentTime(int(addTime * 1000), waveHandle);
        ChangeVolumeSoundMem(255 * ssf.songVolume / 100, waveHandle);
        if (measure != 0) updateNotes(currentTime, speed);
        if (!isReload)
        {
            //画像，動画，音源読み込み
            sprintfDx(path_jacket, "%s\\%s", ssfInfo.dirPath, ssf.jacketPath);
            jacketHandle = LoadGraph(path_jacket);
            sprintfDx(path_bg, "%s\\%s", ssfInfo.dirPath, ssf.bgPath);
            bgHandle = LoadGraph(path_bg);
            sprintfDx(path_wave, "%s\\%s", ssfInfo.dirPath, ssf.wavePath);
            waveHandle = LoadSoundMem(path_wave);
            //曲の長さ取得→drawに送る
            waveLength = GetSoundTotalTime(waveHandle) / 1000.0;
            getWaveLength(waveLength);

            isReload = true;
        }
        liveScene = standby;
        break; }
    case standby: {
        moveMeasure();
        if (checkKey(KEY_INPUT_Q) == 1) {
            liveScene = reload;
        }
        if (checkKey(KEY_INPUT_SPACE) == 1) {
            calcThroughComboAndScore(currentTime, autoMode);
            liveScene = waiting;
            start_count = GetNowHiPerformanceCount();
        }
        break; }
    case waiting: {
        now_count = GetNowHiPerformanceCount();
        currentTime = (now_count - start_count) / 1000000.0 + addTime;
        if (measure == 0) currentTime -= openingDelay;

        if (currentTime >= 0) {
            PlaySoundMem(waveHandle, DX_PLAYTYPE_BACK, false);
            liveScene = playing;
        }}
    case playing: {
        //処理
        now_count = GetNowHiPerformanceCount();
        currentTime = (now_count - start_count) / 1000000.0 + addTime;
        if (measure == 0) currentTime -= openingDelay;
        //        currentTime = 800;
        getSeparateFlag(separateFlag);
        updateNotes(currentTime, speed);
        judge_notes(currentTime, autoMode);

        //曲終了
        isChartPlaying = checkChartPlaying();
        if (!isChartPlaying) {
            isSongPlaying = CheckSoundMem(waveHandle);
            if (once) {
                chartEndTime = currentTime;
                once = false;
            }
            if (!once) timecount = int(currentTime - chartEndTime);
            if (!isSongPlaying) {
                goToResultProcess();
            }
            if ((checkKey(KEY_INPUT_LCONTROL) || checkKey(KEY_INPUT_RCONTROL)) && checkKey(KEY_INPUT_SPACE) == 1) {
                StopSoundMem(waveHandle);
                goToResultProcess();
            }
        }

        //リロード
        if (CheckHitKey(KEY_INPUT_Q) == 1) {
            StopSoundMem(waveHandle);
            timecount = 0;
            liveScene = reload;
        }
        break; }
    default:
        break;
    }

    //Fのあれこれ
    changeStatus();

    //描画
    drawLive(currentTime, darkLevel, bgHandle, image);
    drawStatus(liveScene, nowTime_status, statusMessage);
    drawMusicData(image, ssf.title, ssf.subTitle, ssf.level, jacketHandle);
    if (liveScene == standby) drawMeasure();
    if (timecount >= 1) drawEndMessage();

    //終了
    if (checkKey(KEY_INPUT_ESCAPE) == 1) {
        StopSoundMem(waveHandle);
        DeleteSoundMem(waveHandle);
        liveScene = 0;
        timecount = 0;
        isReload = false;
        resetMeasure();
        setScene(scene_songSelect);
    }

    //デバッグ
#if 0
    DrawFormatString(10, 50, GetColor(255, 255, 255), "t:%f", currentTime);
    DrawFormatString(10, 80, GetColor(255, 255, 255), "m:%d", measure);
    DrawFormatString(10, 110, GetColor(255, 255, 255), "m:%d", int(addTime * 1000));

    //    DrawFormatString(10, 50, GetColor(255, 255, 255), "songlen:%f", waveLength);
        //DrawFormatString(10, 50, GetColor(255, 255, 255), "l:%d", ssf.perfect_timeSize_L);
        //DrawFormatString(10, 30, GetColor(255, 255, 255), "r:%d", ssf.perfect_timeSize_R);
    //    DrawFormatString(50, 100, GetColor(0, 255, 0), "BPM=%f\nOFFSET=%f\nLEVEL=%d\n%s\n%s",
    //        ssf.bpm, ssf.offset, ssf.level, ssf.title, ssf.wavePath);
    //    for (int row = 0; row < 20; row++) {
    //            DrawFormatString(50 + 150, 50 + 30 * row, GetColor(0, 255, 0), "%d,%.3f", ssf.notesType_L[row],ssf.hold.times_L[row]);
    //            DrawFormatString(50 + 300, 50 + 30 * row, GetColor(0, 255, 0), "%d,%.3f", ssf.notesType_R[row],ssf.hold.times_R[row]);
    //    }
#endif
}

/////////////////////////////////////////////////////////////////////
void get_ssfInfo(SSFINFO ssfInfo_) {
    ssfInfo = ssfInfo_;
}
void setStatus(config_ config) {
    autoMode = config.AutoPlay;
    speed = config.HighSpeed;
    speedStep = config.HighSpeedStep;
    separateFlag = config.SeparateJudge;
    darkLevel = config.Darkness;
}
void getLiveScene(int liveScene_) {
    liveScene = liveScene_;
}

void goToResultProcess(void) {
    DeleteSoundMem(waveHandle);
    getScoreFromNotes(&score_, combo_, &notUseAutoMode_);
    mark_ = getClearMark();
    sendScoreToResult(ssf.title, score_, combo_, mark_, notUseAutoMode_, useSeparatejudge_);
    once = true;
    timecount = 0;
    liveScene = 0;
    isReload = false;
    setScene(scene_result);
}

void changeStatus(void) {
    if (checkKey(KEY_INPUT_F1) == 1) {
        startCount_status = GetNowHiPerformanceCount();
        autoMode = !autoMode;
        if (autoMode) sprintfDx(statusMessage, "AutoMode : ON");        
        else sprintfDx(statusMessage, "AutoMode : OFF");
    }
    if (checkKey(KEY_INPUT_F2) == 1) {
        if (speed > 1) speed -= speedStep;
        startCount_status = GetNowHiPerformanceCount();
        sprintfDx(statusMessage, "Speed : %.1f", speed);
    }
    if (checkKey(KEY_INPUT_F3) == 1) {
        if (speed < 10) speed += speedStep;
        startCount_status = GetNowHiPerformanceCount();
        sprintfDx(statusMessage, "Speed : %.1f", speed);
    }
    if (checkKey(KEY_INPUT_F4) == 1) {
        startCount_status = GetNowHiPerformanceCount();
        separateFlag = !separateFlag;
        if (separateFlag) sprintfDx(statusMessage, "SeparateLR : ON");
        else sprintfDx(statusMessage, "SeparateLR : OFF");
    }
    if (checkKey(KEY_INPUT_F5) == 1) {
        if (darkLevel > 0) darkLevel--;
        startCount_status = GetNowHiPerformanceCount();
        sprintfDx(statusMessage, "Darkness : %d", darkLevel);
    }
    if (checkKey(KEY_INPUT_F6) == 1) {
        if (darkLevel < 10) darkLevel++;
        startCount_status = GetNowHiPerformanceCount();
        sprintfDx(statusMessage, "Darkness : %d", darkLevel);
    }

    //譜面制作者向け
    //そんなものはない

    nowTime_status = double((GetNowHiPerformanceCount() - startCount_status) / 1000000);
}

void moveMeasure(void) {
    bool flag = false;
    if (checkKey(KEY_INPUT_PGUP) == 1 || checkKey(KEY_INPUT_PGUP) >= 200 && checkKey(KEY_INPUT_PGUP) % 100 == 1) {
        if (measure < ssf.cnt_measure) {
            measure++;
            flag = true;
        }
    }
    if (checkKey(KEY_INPUT_PGDN) == 1 || checkKey(KEY_INPUT_PGDN) >= 200 && checkKey(KEY_INPUT_PGDN) % 100 == 1) {
        if (measure > 0) {
            measure--;
            flag = true;
        }
    }
    if (checkKey(KEY_INPUT_END) == 1) {
        measure = ssf.cnt_measure;
        flag = true;
    }
    if (checkKey(KEY_INPUT_HOME) == 1) {
        measure = 0;
        flag = true;
    }
    if (flag) {
        addTime = ssf.measureTime[measure];
        currentTime = addTime;
        updateNotes(currentTime, speed);
        if (measure == 0) updateNotes(-10, 10);
        SetSoundCurrentTime(int(addTime * 1000), waveHandle);
        flag = false;
    }
}
void drawMeasure(void) {
    if (measure > 0) {
        DrawFormatString(158, 452, GetColor(255, 255, 255), "Measure : %03d/%03d", measure, ssf.cnt_measure);
    }
}
void resetMeasure(void) {
    measure = 0;
    addTime = 0;
}
void getStatusMessage(char message[260], double* nowTime) { 
    strcpyDx(message, statusMessage);
    *nowTime = nowTime_status;
}
