#include "main.h"
#include "ssf_.h"
#include "songSelect.h"

typedef enum {
    numer,
    denom
}fraction;

unsigned int convertColorCode(char colorCode[8]) {
    if (colorCode[0] != '#') return GetColor(0, 255, 0);
    char str16[16], * next_token = NULL;

    char* str = strtok_s(colorCode, "#", &next_token);
    if (str == NULL) return GetColor(255, 0, 0);

    sprintfDx(str16, "0x%s", str);
    long colorCodeToLong = strtol(str16, NULL,0);

    int red   = int(colorCodeToLong / 256 / 256);
    int green = int(colorCodeToLong / 256 % 256);
    int blue  = int(colorCodeToLong % 256 % 256);
    return GetColor(red, green, blue);
}

void loadGenreIni(GENREINFO* genreInfo, const char* file_name) {
    FILE* fp;

    if ((fopen_s(&fp, file_name, "r")) != 0 || fp == 0) { // "fpが0である可能性があります"というエラー対策
        return; // boolはC++
    }

    char str[260], * next_token = NULL;
    while ((fgets(str, 260, fp)) != NULL) {
 
        char* first = strtok_s(str, "=", &next_token);
        if (first == NULL) continue;

        if (strcmp(first, "GenreName") == 0) {
            char* second = strtok_s(NULL, "=", &next_token);
            strcpyDx(genreInfo->genreName, second);
        }
        else if (strcmp(first, "GenreColor") == 0) {
            char* second = strtok_s(NULL, "=", &next_token);
            genreInfo->genreColor = convertColorCode(second);
        }
        else if (strcmp(first, "FontColor") == 0) {
            char* second = strtok_s(NULL, "=", &next_token);
            genreInfo->fontColor = convertColorCode(second);
        }
    }
    fclose(fp);
    return;
}

void loadSongData(SSFINFO* ssfInfo, const char* file_name) {
    FILE* fp;

    if ((fopen_s(&fp, file_name, "r")) != 0 || fp == 0) { // "fpが0である可能性があります"というエラー対策
        return; // boolはC++
    }
    ssfInfo->songVolime = 100;

    char str[260], * next_token = NULL, tstr[260];
    while ((fgets(str, 260, fp)) != NULL) {
        strncpy_s(tstr, 260, str, 6);
        if (strcmp(tstr, "#START") == 0) {
            fclose(fp);
            return;
        }

        char* first = strtok_s(str, ":", &next_token);
        if (first == NULL) continue;
        char* second = strtok_s(NULL, ":", &next_token);
        if (second == NULL) continue;
        char* char_cut = strtok_s(second, "\n", &next_token);
        if (char_cut == NULL) continue;

        if (strcmp(first, "BPM") == 0) {
            strcpy_s(ssfInfo->bpm, char_cut);
        }
        else if (strcmp(first, "LEVEL") == 0) {
            ssfInfo->level = atoi(char_cut);
        }
        else if (strcmp(first, "SONGVOL") == 0) {
            ssfInfo->songVolime = atoi(char_cut);
        }
        else if (strcmp(first, "DEMOSTART") == 0) {
            ssfInfo->demoStart = atof(char_cut);
        }
        else if (strcmp(first, "TITLE") == 0) {
            strcpy_s(ssfInfo->title, char_cut);
        }
        else if (strcmp(first, "SUBTITLE") == 0) {
            strcpy_s(ssfInfo->subTitle, char_cut);
        }
        else if (strcmp(first, "DESIGNER") == 0) {
            strcpy_s(ssfInfo->designer, char_cut);
        }
        else if (strcmp(first, "WAVE") == 0) {
            strcpy_s(ssfInfo->wavePath, char_cut);
        }
        else if (strcmp(first, "JACKET") == 0) {
            strcpy_s(ssfInfo->jacketPath, char_cut);
        }
        else if (strcmp(first, "BACKGROUND") == 0) {
            sprintfDx(ssfInfo->bgPath, char_cut);
        }
    }
    fclose(fp);
    return;
}

bool ssf_::loadHumenOptions(ssf_* ssf, FILE* fp) { // 引数に余計なものが入りすぎ
    char str[260], * next_token = NULL, tstr[260];
    while ((fgets(str, 260, fp)) != NULL) {
        strncpy_s(tstr, 260, str, 6);
        if (strcmp(tstr, "#START") == 0) {
            return true;
        }

        char* first = strtok_s(str, ":", &next_token);
        if (first == NULL) continue;
        char* second = strtok_s(NULL, ":", &next_token);
        if (second == NULL) continue;
        char* char_cut = strtok_s(second, "\n", &next_token);
        if (char_cut == NULL) continue;

        if (strcmp(first, "BPM") == 0) {
            ssf->bpm = atof(char_cut);
        }
        else if (strcmp(first, "OFFSET") == 0) {
            ssf->offset = atof(char_cut);
        }
        else if (strcmp(first, "LEVEL") == 0) {
            ssf->level = atoi(char_cut);
        }
        else if (strcmp(first, "SONGVOL") == 0) {
            ssf->songVolume = atoi(char_cut);
        }
        else if (strcmp(first, "SEVOL") == 0) {
            ssf->seVolume = atoi(char_cut);
        }
        else if (strcmp(first, "DEMOSTART") == 0) {
            ssf->demoStart = atof(char_cut);
        }
        else if (strcmp(first, "TITLE") == 0) {
            strcpy_s(ssf->title, char_cut);
        }
        else if (strcmp(first, "SUBTITLE") == 0) {
            strcpy_s(ssf->subTitle, char_cut);
        }
        else if (strcmp(first, "WAVE") == 0) {
            strcpy_s(ssf->wavePath, char_cut);
        }
        else if (strcmp(first, "JACKET") == 0) {
            sprintfDx(ssf->jacketPath, char_cut);
        }
        else if (strcmp(first, "BACKGROUND") == 0) {
            sprintfDx(ssf->bgPath, char_cut);
        }
        else if (strcmp(first, "MOVIE") == 0) {
            strcpy_s(ssf->moviePath, char_cut);
        }
    }
    return false;
}

void ssf_::loadHumen(ssf_* ssf, FILE* fp) {
    char line[260], * next_token = NULL;

    //ssfの使うやつを初期化
    ssf->perfect_timeSize_L = 0;
    ssf->perfect_timeSize_R = 0;
    ssf->cnt_measure = 0;
    for (int i = 0; i < NOTES_NUM; i++)
    {
        ssf->measureTime[i] = 0;
    }

    bool isReadingLeft = true;
    bool flag_p = 0, flag_R = 0, flag_L = 0;
    int humen_length = 0;
    double measure[2] = { 4,4 };
    //    double scrollChange = 1.0;
    double scrollChange_L = 1.0;
    double scrollChange_R = 1.0;
//    double bpmChange = ssf->bpm;
    double bpmChange_L = ssf->bpm;
    double bpmChange_R = ssf->bpm;
    double passedTime_total = 0;
    double passedTime_measure = 0;

    int holdNumber_L, holdNumber_R;
    bool holdFlag_L = 0, holdFlag_R = 0;
    double scroll_sub[260] = { 0 };
    double bpm_sub[260] = { 0 };

    //1小節ごとの内容をssfに記述
    for (int row = 0; (fgets(line, 260, fp)) != NULL; row++) {
        char* str = strtok_s(line, "\n", &next_token);
        char humen[260];
        if (str == NULL) continue;

        //譜面中の命令
        if (str[0] == '#') {
            char* first = strtok_s(str, " \n", &next_token);
#pragma region BPMCHANGE
            if (strcmp(first, "#BPMCHANGE") == 0) {
                char* second = strtok_s(NULL, " ", &next_token);
                bpmChange_L = atof(second);
                bpmChange_R = atof(second);
            }
            else if (strcmp(first, "#BPMCHANGE_L") == 0) {
                char* second = strtok_s(NULL, " ", &next_token);
                bpmChange_L = atof(second);
            }
            else if (strcmp(first, "#BPMCHANGE_R") == 0) {
                char* second = strtok_s(NULL, " ", &next_token);
                bpmChange_R = atof(second);
            }
#pragma endregion
            else if (strcmp(first, "#MEASURE") == 0) {
                char* second = strtok_s(NULL, " ", &next_token);
                char* measure_c = strtok_s(second, "/", &next_token);
                char* measure_m = strtok_s(NULL, "/", &next_token);
                measure[numer] = atof(measure_c);
                measure[denom] = atof(measure_m);
            }
#pragma region SCROLL
            else if (strcmp(first, "#SCROLL") == 0) {
                char* second = strtok_s(NULL, " ", &next_token);
                scrollChange_L = atof(second);
                scrollChange_R = atof(second);
            }
            else if (strcmp(first, "#SCROLL_L") == 0) {
                char* second = strtok_s(NULL, " ", &next_token);
                scrollChange_L = atof(second);
            }
            else if (strcmp(first, "#SCROLL_R") == 0) {
                char* second = strtok_s(NULL, " ", &next_token);
                scrollChange_R = atof(second);
            }
#pragma endregion

            else if (strcmp(first, "#END") == 0) {
                break;
            }
        }

        //数字
        for (unsigned int i=0; i<strlen(str); i++){
            if('0' <= str[i] && str[i] <= '9'){
                humen[humen_length] = str[i];
                if (isReadingLeft) {
                    scroll_sub[humen_length] = scrollChange_L;
                    bpm_sub[humen_length] = bpmChange_L;
                }
                else {
                    scroll_sub[humen_length] = scrollChange_R;
                    bpm_sub[humen_length] = bpmChange_R;
                }
                humen_length++;
            }
            if(str[i] == ','){
                humen[humen_length] = '\0';
                strncpy_s(humen, 260, humen, humen_length);
                flag_p = true;
                break;
            }
            if (str[i] == '/' && str[i + 1] == '/') break;
        }
//        clsDx();
//        printfDx("scroll=[%s]", str);//debug

        if (flag_p) {
            int length = strlen(humen);
            for (int i = 0; i < length; i++) {
                if ('0' <= humen[i] && humen[i] <= '9') {
                    ssf->bpm = bpm_sub[i];

                    if (isReadingLeft) {
                        ssf->notesSpeed_L[ssf->perfect_timeSize_L] = scroll_sub[i];
                        //数字を判別して判定時間とノーツの種類を格納
                        switch (humen[i]) {
                        case '0':
                            break;
                        case '1':
                            ssf->notesType_L[ssf->perfect_timeSize_L] = 1;
                            ssf->perfect_times_L[ssf->perfect_timeSize_L++] = passedTime_total + passedTime_measure - ssf->offset;
                            break;
                        case '2':
                            if (holdFlag_L) break;
                            holdNumber_L = ssf->perfect_timeSize_L;
                            holdFlag_L = true;
                            ssf->hold.times_L[holdNumber_L] = passedTime_total + passedTime_measure - ssf->offset;
                            ssf->perfect_timeSize_L++;
                            break;
                        case '3':
                            if (holdFlag_L) {
                                ssf->notesType_L[holdNumber_L] = 2;
                                ssf->notesType_L[ssf->perfect_timeSize_L] = 3;
                                ssf->perfect_times_L[holdNumber_L] = hold.times_L[holdNumber_L];
                                ssf->perfect_times_L[ssf->perfect_timeSize_L++] = passedTime_total + passedTime_measure - ssf->offset;
                                ssf->hold.times_L[holdNumber_L] = ssf->perfect_times_L[ssf->perfect_timeSize_L - 1];
                                ssf->hold.connect_L[holdNumber_L] = ssf->perfect_timeSize_L - 1;
                                holdFlag_L = false;
                            }
                            break;
                        case '4':
                            ssf->notesType_L[ssf->perfect_timeSize_L] = 4;
                            ssf->perfect_times_L[ssf->perfect_timeSize_L++] = passedTime_total + passedTime_measure - ssf->offset;
                            break;
                        case '5':
                            ssf->notesType_L[ssf->perfect_timeSize_L] = 5;
                            ssf->perfect_times_L[ssf->perfect_timeSize_L++] = passedTime_total + passedTime_measure - ssf->offset;
                            break;
                        case '6':
                            ssf->notesType_L[ssf->perfect_timeSize_L] = 6;
                            ssf->perfect_times_L[ssf->perfect_timeSize_L++] = passedTime_total + passedTime_measure - ssf->offset;
                            break;
                        case '7':
                            ssf->notesType_L[ssf->perfect_timeSize_L] = 7;
                            ssf->perfect_times_L[ssf->perfect_timeSize_L++] = passedTime_total + passedTime_measure - ssf->offset;
                            break;
                        case '8':
                            ssf->notesType_L[ssf->perfect_timeSize_L] = 8;
                            ssf->perfect_times_L[ssf->perfect_timeSize_L++] = passedTime_total + passedTime_measure - ssf->offset;
                            break;
                        default:
                            break;
                        }
                        //小節を切り替えるための左フラグ
                        flag_L = 1;
                    }
                    else {
                        ssf->notesSpeed_R[ssf->perfect_timeSize_R] = scroll_sub[i];
                        //数字を判別して判定時間とノーツの種類を格納
                        switch (humen[i]) {
                        case '0':
                            break;
                        case '1':
                            ssf->notesType_R[ssf->perfect_timeSize_R] = 1;
                            ssf->perfect_times_R[ssf->perfect_timeSize_R++] = passedTime_total + passedTime_measure - ssf->offset;
                            break;
                        case '2':
                            if (!holdFlag_R) {
                                holdNumber_R = ssf->perfect_timeSize_R;
                                holdFlag_R = true;
                                ssf->hold.times_R[holdNumber_R] = passedTime_total + passedTime_measure - ssf->offset;
                                ssf->perfect_timeSize_R++;
                            }
                            break;
                        case '3':
                            if (holdFlag_R) {
                                ssf->notesType_R[holdNumber_R] = 2;
                                ssf->notesType_R[ssf->perfect_timeSize_R] = 3;
                                ssf->perfect_times_R[holdNumber_R] = hold.times_R[holdNumber_R];
                                ssf->perfect_times_R[ssf->perfect_timeSize_R++] = passedTime_total + passedTime_measure - ssf->offset;
                                ssf->hold.times_R[holdNumber_R] = ssf->perfect_times_R[ssf->perfect_timeSize_R - 1];
                                ssf->hold.connect_R[holdNumber_R] = ssf->perfect_timeSize_R - 1;
                                holdFlag_R = false;
                            }
                            break;
                        case '4':
                            ssf->notesType_R[ssf->perfect_timeSize_R] = 4;
                            ssf->perfect_times_R[ssf->perfect_timeSize_R++] = passedTime_total + passedTime_measure - ssf->offset;
                            break;
                        case '5':
                            ssf->notesType_R[ssf->perfect_timeSize_R] = 5;
                            ssf->perfect_times_R[ssf->perfect_timeSize_R++] = passedTime_total + passedTime_measure - ssf->offset;
                            break;
                        case '6':
                            ssf->notesType_R[ssf->perfect_timeSize_R] = 6;
                            ssf->perfect_times_R[ssf->perfect_timeSize_R++] = passedTime_total + passedTime_measure - ssf->offset;
                            break;
                        case '7':
                            ssf->notesType_R[ssf->perfect_timeSize_R] = 7;
                            ssf->perfect_times_R[ssf->perfect_timeSize_R++] = passedTime_total + passedTime_measure - ssf->offset;
                            break;
                        case '8':
                            ssf->notesType_R[ssf->perfect_timeSize_R] = 8;
                            ssf->perfect_times_R[ssf->perfect_timeSize_R++] = passedTime_total + passedTime_measure - ssf->offset;
                            break;
                        default:
                            break;
                        }
                        //小節を切り替えるための右フラグ
                        flag_R = 1;
                    }
                    passedTime_measure += 60.0 * measure[numer] / (measure[denom] / 4) * ((double)1 / length) / ssf->bpm;
                }
            }
            isReadingLeft = !isReadingLeft;
            if (flag_R && flag_L) {
                flag_R = 0;
                flag_L = 0;
                ssf->cnt_measure++;
                ssf->measureTime[cnt_measure] = passedTime_total - ssf->offset;
                passedTime_total += passedTime_measure;
            }
            humen_length = 0;
            passedTime_measure = 0;
            flag_p = false;
        }
//        clsDx();
//        printfDx("[%s]", str);
    }
}

bool ssf_::loadHumenData(ssf_* ssf, const char* file_name) {
    FILE* fp;

    if ((fopen_s(&fp, file_name, "r")) != 0 || fp == 0) { // "fpが0である可能性があります"というエラー対策
        return false; // boolはC++
    }

    bool loadable = loadHumenOptions(ssf, fp);

    if (loadable) loadHumen(ssf, fp);

    fclose(fp);
    return true;
}