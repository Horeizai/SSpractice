#include "main.h"
#include "keyboard.h"
#include "notes.h"
#include "load.h"
#include "ssf_.h"
#include "draw.h"

#define NOTES_DEG_FALSE 85

typedef struct {
    bool flag;
    int type;
    double x; //X���W
    double y; //Y���W
    double deg; //�p�x
    double extRate;
}NOTES;
typedef struct {
    int combo;
    int count[6];
}COMBO;
typedef enum
{
    tap = 1,
    holdStart,
    holdEnd,
    flickLeft,
    flickRight,
    flickUp,
    flickDown,
    exTap
}notesType_e;
typedef enum {
    maxCombo,
    wonderful,
    great,
    nice,
    bad,
    miss
}judgeRank_e;

NOTES notesR[NOTES_NUM];
NOTES notesL[NOTES_NUM];
COMBO combo;
static double startDeg = 7;

//karioki
double judgeRange_Wonderful = 55;
double judgeRange_Great = 120;
double judgeRange_Nice = 200;
double judgeRange_Bad = 300;

bool hitFlag = false;
int judgeRank = 0;
double startCount_Judge = 0;

bool hitFlag_L, hitFlag_R;
int judgeRank_L, judgeRank_R;
int number_L, number_R;
bool holding_L, holding_R;
bool judgingHold_L, judgingHold_R;
int number_L_hold, number_R_hold;
bool isFlickInvincible = false;

int totalNotes;
double totalScore;
double score_wonderful;
bool notUseAutoMode = true;
static int clearMark;
ssf_* ssf;
sound_ sound;

//�֐��̒�`
void copy_ssf(ssf_* ssf_L, sound_ sound);
void judge_notes(double currentTime, bool autoMode);
int judge_tap(double currentTime, double perfect_times[NOTES_NUM],int num);
int judge_flick(double currentTime, double perfect_times[NOTES_NUM], int num);
void judge_holding(double currentTime, char LR);
void judge_holding_quit(double currentTime, char LR);
void judge_miss(double currentTime);
void judge_endProcess(int notesType);

void copy_ssf(ssf_ *ssf_L, sound_ sound_L) {
    ssf = ssf_L;
    sound = sound_L;
    ChangeVolumeSoundMem(255 * ssf->seVolume / 100, sound.tap_wonderful);
    ChangeVolumeSoundMem(255 * ssf->seVolume / 100, sound.tap_great);
    ChangeVolumeSoundMem(255 * ssf->seVolume / 100, sound.tap_nice);
    ChangeVolumeSoundMem(255 * ssf->seVolume / 100, sound.tap_bad);
    ChangeVolumeSoundMem(255 * ssf->seVolume / 100, sound.tap_Ex);
    ChangeVolumeSoundMem(255 * ssf->seVolume / 100, sound.tap_none);
    ChangeVolumeSoundMem(255 * ssf->seVolume / 100, sound.flick);
    ChangeVolumeSoundMem(255 * ssf->seVolume / 100, sound.hold_end);
    ChangeVolumeSoundMem(255 * ssf->seVolume / 100, sound.hold_strut);
}

#pragma region //�m�[�c�n
//�m�[�c�E�R���{������
void setNotes() {
    for (int i = 0; i < NOTES_NUM; i++) {
        notesL[i].flag = false;
        notesL[i].type = 0;
        notesL[i].deg = 0;
        notesL[i].x = 0;
        notesL[i].y = 0;
        notesL[i].extRate = 1;
        notesR[i].flag = false;
        notesR[i].type = 0;
        notesR[i].deg = 0;
        notesR[i].x = 0;
        notesR[i].y = 0;
        notesR[i].extRate = 1;

        combo.combo = 0;
        combo.count[maxCombo] = 0;
        combo.count[wonderful] = 0;
        combo.count[great] = 0;
        combo.count[nice] = 0;
        combo.count[bad] = 0;
        combo.count[miss] = 0;
    }
}
//notes����
void createNotes() {
    int lane_length_L = ssf->perfect_timeSize_L;
    int lane_length_R = ssf->perfect_timeSize_R;

    //��notes����
    for (int i = 0; i < lane_length_L; i++) {
        notesL[i].flag = true;
        notesL[i].type = ssf->notesType_L[i];
    }
    //�Enotes����
    for (int i = 0; i < lane_length_R; i++) {
        notesR[i].flag = true;
        notesR[i].type = ssf->notesType_R[i];
    }
}
//combo�X�V
void combo_update() {
    switch (judgeRank) {
    case 1:
        combo.combo++;
        combo.count[wonderful]++;
        break;
    case 2:
        combo.combo++;
        combo.count[great]++;
        break;
    case 3:
        combo.combo = 0;
        combo.count[nice]++;
        break;
    case 4:
        combo.combo = 0;
        combo.count[bad]++;
        break;
    case 5:
        combo.combo = 0;
        combo.count[miss]++;
        break;
    default:
        break;
    }
    if (combo.combo > combo.count[maxCombo]) combo.count[maxCombo] = combo.combo;
}
//notes���W�X�V
void updateNotes(double currentTime, double speed) {
    for (int i = 0; i < NOTES_NUM; i++) {
        //��notes
        if (notesL[i].flag) {
            notesL[i].deg = (currentTime - ssf->perfect_times_L[i]) * 11 * speed * ssf->notesSpeed_L[i] + 70;
            notesL[i].x = -20 + 550 * sin(notesL[i].deg * M_PI / 180);
            notesL[i].y = 666 - 550 * cos(notesL[i].deg * M_PI / 180);
            //hold�����Ⴄ����
            if (ssf->notesType_L[i] == 2 && notesL[i].deg >= 70) {
                notesL[i].x = -20 + 550 * sin(70 * M_PI / 180);
                notesL[i].y = 666 - 550 * cos(70 * M_PI / 180);
            }
        }
        //�Enotes
        if (notesR[i].flag) {
            notesR[i].deg = (currentTime - ssf->perfect_times_R[i]) * 11 * speed * ssf->notesSpeed_R[i] + 70;
            notesR[i].x = 1300 - 550 * sin(notesR[i].deg * M_PI / 180);
            notesR[i].y = 666 - 550 * cos(notesR[i].deg * M_PI / 180);
            //hold�����Ⴄ����
            if (ssf->notesType_R[i] == 2 && notesR[i].deg >= 70) {
                notesR[i].x = 1300 - 550 * sin(70 * M_PI / 180);
                notesR[i].y = 666 - 550 * cos(70 * M_PI / 180);
            }
        }
    }
}
#pragma endregion

//�X�R�A�n
void setScore() {
    totalScore = 0;
    totalNotes = ssf->perfect_timeSize_L + ssf->perfect_timeSize_R;
    score_wonderful = 10000000.0 / totalNotes;
}
void calcScore() {
    switch (judgeRank) {
    case 1:
        totalScore += score_wonderful;
        break;
    case 2:
        totalScore += score_wonderful * 0.8;
        break;
    case 3:
        totalScore += score_wonderful * 0.6;
        break;
    case 4:
        totalScore += score_wonderful * 0.4;
        break;
    default:
        break;
    }
    //if (combo.count[maxCombo] == totalNotes) totalScore = 10000000;
}

#pragma region //����n

void setJudge() {
    bool hitFlag_L = 0, hitFlag_R = 0;
    int judgeRank_L = 0, judgeRank_R = 0;
    int number_L = 0, number_R = 0;
    bool holding_L = 0, holding_R = 0;
    bool judgingHold_L = 0, judgingHold_R = 0;
    int number_L_hold = 0, number_R_hold = 0;
    notUseAutoMode = true;
    judgeRank = 0;
}

void judge_decideLR_sub(char LR) {
    switch (LR){
    case 'L'://Left
        notesL[number_L].flag = false;
        judgeRank = judgeRank_L;
        judge_endProcess(ssf->notesType_L[number_L]);
        break;
    case 'R'://Right
        notesR[number_R].flag = false;
        judgeRank = judgeRank_R;
        judge_endProcess(ssf->notesType_R[number_R]);
        break;
    default:
        break;
    }
}
void judge_decideLR(double currentTime) {
    if (hitFlag_L && !hitFlag_R) {
        startCount_Judge = currentTime;
        judge_decideLR_sub('L');
    }
    if (!hitFlag_L && hitFlag_R) {
        startCount_Judge = currentTime;
        judge_decideLR_sub('R');
    }
    //���E�Ƃ��ɔ������Ă���ꍇ
    if (hitFlag_L && hitFlag_R) {
        startCount_Judge = currentTime;
        //���̕�������g����߂��ꍇ
        if (fabs(currentTime - ssf->perfect_times_L[number_L]) < fabs(currentTime - ssf->perfect_times_R[number_R])) {
            judge_decideLR_sub('L');
        }
        //�E�̕�������g����߂��ꍇ
        else if (fabs(currentTime - ssf->perfect_times_L[number_L]) > fabs(currentTime - ssf->perfect_times_R[number_R])) {
            judge_decideLR_sub('R');
        }
        //���������̏ꍇ
        else {
            //�񂩏��ȏ㉟���Ă�����ǂ��������
            if (countNotesKeyPressed('L') >= 1 && countNotesKeyPressed('R') >= 1 && countNotesKeyPressed('B') >= 2) {
                judge_decideLR_sub('L');
                judge_decideLR_sub('R');
            }
            //�ꂩ�����������ĂȂ������獶��������
            else {
                judge_decideLR_sub('L');
            }
        }
    }
    //�I������
    hitFlag_L = 0, hitFlag_R = 0;
    judgeRank_L = 0, judgeRank_R = 0;
    number_L = 0, number_R = 0;
    isFlickInvincible = 0;
}

void judgeNotes(double currentTime) {
    // ������
    if (countNotesKeyPressed('L') >= 1) {
        holding_L = true;
        for (int i = 0; i < NOTES_NUM; i++) {
            if (notesL[i].type >= flickLeft && notesL[i].type <= flickDown) {
                if (!notesL[i].flag && currentTime - ssf->perfect_times_L[i] <= judgeRange_Wonderful * 0.001) {
                    isFlickInvincible = true;
                }
            }
            if (notesL[i].flag) {
                switch (ssf->notesType_L[i])
                {
                case 1: {
                    judgeRank_L = judge_tap(currentTime, ssf->perfect_times_L, i);
                    if(judgeRank_L != 0){
                        number_L = i;
                        hitFlag_L = 1;
                    }
                    break; }
                case 2: {
                    //�n�_����͓����Ȃ̂�
                    judgeRank_L = judge_tap(currentTime, ssf->perfect_times_L, i);
                    if (judgeRank_L != 0) {
                        number_L = i;
                        number_L_hold = i;
                        hitFlag_L = 1;
                        notesL[i].deg = 70;
                        judgingHold_L = true;
                    }
                    break; }
                case 3: {
                    //�������Ȃ�
                    break; }
                //�����͌����_�ōl���Ȃ����Ƃɂ���
                case 4: 
                case 5:
                case 6:
                case 7: {
                    judgeRank_L = judge_flick(currentTime, ssf->perfect_times_L, i);
                    if (judgeRank_L != 0) {
                        number_L = i;
                        hitFlag_L = 1;
                    }
                    break; }
                case 8: {
                    //���ʂ�0�ȊO�Ȃ猋�ʂ�1(wonderful)�ɂ���
                    judgeRank_L = judge_tap(currentTime, ssf->perfect_times_L, i);
                    if (judgeRank_L != 0) {
                        judgeRank_L = 1;
                        number_L = i;
                        hitFlag_L = 1;
                    }
                    break; }
                default:
                    break;
                }
                if (hitFlag_L) break;
            }
        }
        if (!hitFlag_L) PlaySoundMem(sound.tap_none, DX_PLAYTYPE_BACK);//�X�J����炷
    }
    // �E����
    if (countNotesKeyPressed('R') >= 1) {
        holding_R = true;
        for (int i = 0; i < NOTES_NUM; i++) {
            if (notesR[i].type >= flickLeft && notesR[i].type <= flickDown) {
                if (!notesR[i].flag && currentTime - ssf->perfect_times_R[i] <= judgeRange_Wonderful * 0.001) {
                    isFlickInvincible = true;
                }
            }
            if (notesR[i].flag) {
                switch (ssf->notesType_R[i])
                {
                case 1: {
                    judgeRank_R = judge_tap(currentTime, ssf->perfect_times_R, i);
                    if (judgeRank_R != 0) {
                        number_R = i;
                        hitFlag_R = 1;
                    }
                    break; }
                case 2: {
                    //�n�_����͓����Ȃ̂�
                    judgeRank_R = judge_tap(currentTime, ssf->perfect_times_R, i);
                    if (judgeRank_R != 0) {
                        number_R = i;
                        number_R_hold = i;
                        hitFlag_R = 1;
                        notesR[i].deg = 70;
                        judgingHold_R = true;
                    }
                    break; }
                case 3: {
                    //�������Ȃ�
                    break; }
                      //�����͌����_�ōl���Ȃ����Ƃɂ���
                case 4:
                case 5:
                case 6:
                case 7: {
                    judgeRank_R = judge_flick(currentTime, ssf->perfect_times_R, i);
                    if (judgeRank_R != 0) {
                        number_R = i;
                        hitFlag_R = 1;
                    }
                    break; }
                case 8: {
                    //���ʂ�0�ȊO�Ȃ猋�ʂ�1(wonderful)�ɂ���
                    judgeRank_R = judge_tap(currentTime, ssf->perfect_times_R, i);
                    if (judgeRank_R != 0) {
                        judgeRank_R = 1;
                        number_R = i;
                        hitFlag_R = 1;
                    }
                    break; }
                default:
                    break;
                }
                if (hitFlag_R) break;

            }
        }
        if (!hitFlag_R) PlaySoundMem(sound.tap_none, DX_PLAYTYPE_BACK);//�X�J����炷
    }
    judge_decideLR(currentTime);
    judge_miss(currentTime);
}
void judgeNotes_auto(double currentTime) {
    // ������
    holding_L = true;
    for (int i = 0; i < NOTES_NUM; i++) {
        if (notesL[i].flag) {
            switch (ssf->notesType_L[i])
            {
            case 1:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
                if (0.0 <= currentTime - ssf->perfect_times_L[i]) {
                    startCount_Judge = currentTime;
                    notesL[i].flag = false;
                    hitFlag = 1;
                    judgeRank = 1;
                    judge_endProcess(ssf->notesType_L[i]);
                }
                break;
            case 2:
                if (0.0 <= currentTime - ssf->perfect_times_L[i]) {
                    startCount_Judge = currentTime;
                    notesL[i].flag = false;
                    hitFlag = 1;
                    judgeRank = 1;
                    judge_endProcess(ssf->notesType_L[i]);
                }
                if (hitFlag) {
                    judgingHold_L = true;
                    number_L_hold = i;
                }
                break;
            default:
                break;
            }
            if (hitFlag) {
                if (notUseAutoMode) notUseAutoMode = false;
                break;
            }
        }
    }
    hitFlag = 0;
    // �E����
    holding_R = true;
    for (int i = 0; i < NOTES_NUM; i++) {
        if (notesR[i].flag) {
            switch (ssf->notesType_R[i])
            {
            case 1:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
                if (0.0 <= currentTime - ssf->perfect_times_R[i]) {
                    startCount_Judge = currentTime;
                    notesR[i].flag = false;
                    hitFlag = 1;
                    judgeRank = 1;
                    judge_endProcess(ssf->notesType_R[i]);
                }
                break;
            case 2:
                if (0.0 <= currentTime - ssf->perfect_times_R[i]) {
                    startCount_Judge = currentTime;
                    notesR[i].flag = false;
                    hitFlag = 1;
                    judgeRank = 1;
                    judge_endProcess(ssf->notesType_R[i]);
                }
                if (hitFlag) {
                    judgingHold_R = true;
                    number_R_hold = i;
                }
                break;
            default:
                break;
            }
            if (hitFlag) {
                if (notUseAutoMode) notUseAutoMode = false;
                break;
            }
        }
    }
    hitFlag = 0;
}

int judge_tap(double currentTime, double perfect_times[NOTES_NUM], int num) {
    if (-0.001 * judgeRange_Wonderful <= currentTime - perfect_times[num]
        && currentTime - perfect_times[num] <= 0.001 * judgeRange_Wonderful) {
        return 1;
    }
    else if (-0.001 * judgeRange_Great <= currentTime - perfect_times[num]
        && currentTime - perfect_times[num] <= 0.001 * judgeRange_Great) {
        return 2;
    }
    else if (-0.001 * judgeRange_Nice <= currentTime - perfect_times[num]
        && currentTime - perfect_times[num] <= 0.001 * judgeRange_Nice) {
        if (isFlickInvincible) return 0;
        else return 3;
    }
    else if (-0.001 * judgeRange_Bad <= currentTime - perfect_times[num]
        && currentTime - perfect_times[num] <= 0.001 * judgeRange_Bad) {
        if (isFlickInvincible) return 0;
        else return 4;
    }
    return 0;
}
int judge_flick(double currentTime, double perfect_times[NOTES_NUM], int num) {
    if (-0.001 * judgeRange_Wonderful <= currentTime - perfect_times[num]
        && currentTime - perfect_times[num] < 0.001 * judgeRange_Wonderful) {
        return 1;
    }
    else if (0.0 <= currentTime - perfect_times[num]
        && currentTime - perfect_times[num] <= 0.001 * judgeRange_Great) {
        return 2;
    }
    else if (0.0 <= currentTime - perfect_times[num]
        && currentTime - perfect_times[num] <= 0.001 * judgeRange_Nice) {
        return 3;
    }
    else if (0.0 <= currentTime - perfect_times[num]
        && currentTime - perfect_times[num] <= 0.001 * judgeRange_Bad) {
        return 4;
    }
    return 0;
}

//hold���̏���
void judge_holding(double currentTime, char LR) {
    //L
    if (LR == 'L') {
        if (0.0 <= currentTime - ssf->hold.times_L[number_L_hold] && notesL[ssf->hold.connect_L[number_L_hold]].flag && notesL[number_L_hold].type == holdStart) {
            judgingHold_L = false;
            notesL[ssf->hold.connect_L[number_L_hold]].flag = false;
            startCount_Judge = currentTime;
            judgeRank = 1;
            judge_endProcess(ssf->notesType_L[number_L_hold]);
        }
        for (int i = 0; i < NOTES_NUM; i++) {
            //flick��PC�p���ʑ[�u
            if (notesL[i].flag && ssf->notesType_L[i] >= 4 && ssf->notesType_L[i] <= 7 && 0.0 <= currentTime - ssf->perfect_times_L[i]) {
                notesL[i].flag = false;
                startCount_Judge = currentTime;
                judgeRank = 1;
                judge_endProcess(ssf->notesType_L[i]);
            }
        }
    }
    //R
    else {
        if (0.0 <= currentTime - ssf->hold.times_R[number_R_hold] && notesR[ssf->hold.connect_R[number_R_hold]].flag && notesR[number_R_hold].type == holdStart) {
            judgingHold_R = false;
            notesR[ssf->hold.connect_R[number_R_hold]].flag = false;
            startCount_Judge = currentTime;
            judgeRank = 1;
            judge_endProcess(ssf->notesType_R[number_R_hold]);
        }
        for (int i = 0; i < NOTES_NUM; i++) {
            //flick��PC�p���ʑ[�u
            if (notesR[i].flag && ssf->notesType_R[i] >= 4 && ssf->notesType_R[i] <= 7 && 0.0 <= currentTime - ssf->perfect_times_R[i]) {
                notesR[i].flag = false;
                startCount_Judge = currentTime;
                judgeRank = 1;
                judge_endProcess(ssf->notesType_R[i]);
            }
        }

    }    
}
//hold�𗣂������̏���
void judge_holding_quit(double currentTime, char LR) {
    //L
    if (LR == 'L') {
        if (!isNotesKeyPressing('L')) {
            holding_L = false;

            if (!notesL[number_L_hold].flag && notesL[ssf->hold.connect_L[number_L_hold]].flag && ssf->notesType_L[number_L_hold] == holdStart) {
                judgingHold_L = false;
                notesL[ssf->hold.connect_L[number_L_hold]].flag = false;
                startCount_Judge = currentTime;
                //��딻��͕ʂ̊֐��ŏ�����̂�
                judgeRank = judge_tap(currentTime, ssf->hold.times_L, number_L_hold);
                //���������_�ŃX�J=�������ɂ��MISS
                if (judgeRank == 0) judgeRank = 5;

                judge_endProcess(ssf->notesType_L[ssf->hold.connect_L[number_L_hold]]);
            }
        }
    }
    //R
    else {
        if (!isNotesKeyPressing('R')) {
            holding_R = false;

            if (!notesR[number_R_hold].flag && notesR[ssf->hold.connect_R[number_R_hold]].flag && ssf->notesType_R[number_R_hold] == 2) {
                judgingHold_R = false;
                notesR[ssf->hold.connect_R[number_R_hold]].flag = false;
                startCount_Judge = currentTime;
                //��딻��͕ʂ̊֐��ŏ�����̂�
                judgeRank = judge_tap(currentTime, ssf->hold.times_R, number_R_hold);
                //���������_�ŃX�J=�������ɂ��MISS
                if (judgeRank == 0) judgeRank = 5;

                judge_endProcess(ssf->notesType_R[ssf->hold.connect_R[number_R_hold]]);
            }
        }
    }
}

//miss���ǂ�������
void judge_miss(double currentTime) {
    for (int i = 0; i < NOTES_NUM; i++) {
        if (notesL[i].flag) {
            if (0.001 * judgeRange_Bad <= currentTime - ssf->perfect_times_L[i]) {
                notesL[i].flag = false;
                startCount_Judge = currentTime;
                judgeRank = 5;
                judge_endProcess(ssf->notesType_L[i]);

                //holdStart�����Ⴄ����
                if (ssf->notesType_L[i] == holdStart) {
                    notesL[ssf->hold.connect_L[i]].flag = false;
                    judgingHold_L = false;
                    judge_endProcess(ssf->notesType_L[ssf->hold.connect_L[i]]);
                }
            }
        }
        if (notesR[i].flag) {
            if (0.001 * judgeRange_Bad <= currentTime - ssf->perfect_times_R[i]) {
                notesR[i].flag = false;
                startCount_Judge = currentTime;
                judgeRank = 5;
                judge_endProcess(ssf->notesType_R[i]);

                //hold�����Ⴄ����
                if (ssf->notesType_R[i] == 2) {
                    notesR[ssf->hold.connect_R[i]].flag = false;
                    judgingHold_R = false;
                    judge_endProcess(ssf->notesType_R[ssf->hold.connect_R[i]]);
                }
            }
        }
    }
}

//����I�����̏���
void judge_endProcess(int notesType) {
    combo_update();
    calcScore();
    //����ɂ�����炸������(�~�X�܂�)
    if (notesType == holdEnd) {
        PlaySoundMem(sound.hold_end, DX_PLAYTYPE_BACK);
        return;
    }
    if (judgeRank == miss) return;
    //����ɂ�����炸������
    if (flickLeft <= notesType && notesType <= flickDown) {
        PlaySoundMem(sound.flick, DX_PLAYTYPE_BACK);
        return;
    }
    if (notesType == exTap) {
        PlaySoundMem(sound.tap_Ex, DX_PLAYTYPE_BACK);
        return;
    }
    switch (judgeRank)
    {
    case wonderful:
        PlaySoundMem(sound.tap_wonderful, DX_PLAYTYPE_BACK);
        break;
    case great:
        PlaySoundMem(sound.tap_great, DX_PLAYTYPE_BACK);
        break;
    case nice:
        PlaySoundMem(sound.tap_nice, DX_PLAYTYPE_BACK);
        break;
    case bad:
        PlaySoundMem(sound.tap_bad, DX_PLAYTYPE_BACK);
        break;
    default:
        PlaySoundMem(sound.tap_none, DX_PLAYTYPE_BACK);
        break;
    }
}
#pragma endregion

//���̑�
bool checkChartPlaying(void) {
    if (notesL[ssf->perfect_timeSize_L - 1].flag) return true;
    if (notesR[ssf->perfect_timeSize_R - 1].flag) return true;
    return false;
}
int checkClearLevel(void) {
    if (combo.count[nice] > 0 || combo.count[bad] > 0 || combo.count[miss] > 0) return 2;
    if (combo.count[great] > 0) return 3;
    return 4;
}
int getClearMark(void) {
    if (totalNotes == combo.count[wonderful]) return 4;
    if (totalNotes == combo.count[maxCombo]) return 3;
    if (totalScore >= 5000000) return 2;
    return 1;
}
void getScoreFromNotes(int* score, int comboCount[6], bool* notUseAutoMode_) {
    *notUseAutoMode_ = notUseAutoMode;
    *score = int(round(totalScore));
    for (int i = 0; i < 6; i++)
    {
        comboCount[i] = combo.count[i];
    }

}

//�`��n
void drawNotesL(image_ image) {
    //hold�֘A
    for (int i = 0; i < NOTES_NUM; i++) {
        if (ssf->notesType_L[i] == holdStart && notesL[i].deg >= startDeg && notesL[ssf->hold.connect_L[i]].flag) {
            DrawCircleGauge(-20, 666, min(notesL[i].deg,70.0) / 360.0 * 100, image.notes_holdStrut, max(startDeg, notesL[ssf->hold.connect_L[i]].deg) / 360.0 * 100);
            if (notesL[ssf->hold.connect_L[i]].deg < startDeg) {
                DrawRotaGraph(int(-20 + 550 * sin(startDeg * M_PI / 180)), int(666 - 550 * cos(startDeg * M_PI / 180)), notesL[i].extRate, 0, image.notes_hold, true);
            }
        }
    }
    if (!notesL[number_L_hold].flag && notesL[ssf->hold.connect_L[number_L_hold]].flag && ssf->notesType_L[number_L_hold] == holdStart) DrawGraph(443, 423, image.notes_hold, true);

    for (int i = NOTES_NUM-1; i >= 0; i--) {
        if (!(notesL[i].flag && notesL[i].deg >= startDeg && notesL[i].deg <= 85)) continue;

        //�o�����̑傫���ς�����
        {if (notesL[i].deg <= 8.0) notesL[i].extRate = 0.6;
        else if (notesL[i].deg <= 8.5) notesL[i].extRate = 0.7;
        else if (notesL[i].deg <= 9.0) notesL[i].extRate = 0.8;
        else if (notesL[i].deg <= 9.5) notesL[i].extRate = 0.9;
        else if (notesL[i].deg <= 10.0) notesL[i].extRate = 1.0;
        else if (notesL[i].deg <= 10.5) notesL[i].extRate = 1.1;
        else if (notesL[i].deg <= 11.0) notesL[i].extRate = 1.2;
        else if (notesL[i].deg <= 11.5) notesL[i].extRate = 1.3;
        else if (notesL[i].deg <= 12.0) notesL[i].extRate = 1.4;
        else if (notesL[i].deg <= 12.5) notesL[i].extRate = 1.5;
        else if (notesL[i].deg <= 13.0) notesL[i].extRate = 1.6;
        else if (notesL[i].deg <= 13.5) notesL[i].extRate = 1.7;
        else if (notesL[i].deg <= 14.0) notesL[i].extRate = 1.8;
        else if (notesL[i].deg <= 14.5) notesL[i].extRate = 1.9;
        else if (notesL[i].deg <= 15.0) notesL[i].extRate = 2.0;
        else if (notesL[i].deg <= 18.5) notesL[i].extRate = 1.9;
        else if (notesL[i].deg <= 19.0) notesL[i].extRate = 1.8;
        else if (notesL[i].deg <= 19.5) notesL[i].extRate = 1.7;
        else if (notesL[i].deg <= 20.0) notesL[i].extRate = 1.6;
        else if (notesL[i].deg <= 20.5) notesL[i].extRate = 1.5;
        else if (notesL[i].deg <= 21.0) notesL[i].extRate = 1.4;
        else if (notesL[i].deg <= 21.5) notesL[i].extRate = 1.3;
        else if (notesL[i].deg <= 22.0) notesL[i].extRate = 1.2;
        else if (notesL[i].deg <= 22.5) notesL[i].extRate = 1.1;
        else if (notesL[i].deg <= 70.0) notesL[i].extRate = 1.0; }

        switch (ssf->notesType_L[i]) {
        case tap:
            DrawRotaGraphF(float(notesL[i].x), float(notesL[i].y), notesL[i].extRate, 0, image.notes_tap, true);
            break;
        case holdStart:
            DrawRotaGraphF(float(notesL[i].x), float(notesL[i].y), notesL[i].extRate, 0, image.notes_hold, true);
            break;
        case holdEnd:
            DrawRotaGraphF(float(notesL[i].x), float(notesL[i].y), 1.0, 0, image.notes_hold, true);
            break;
        case flickLeft:
            DrawRotaGraphF(float(notesL[i].x), float(notesL[i].y), notesL[i].extRate, 0, image.notes_flick_L, true);
            break;
        case flickRight:
            DrawRotaGraphF(float(notesL[i].x), float(notesL[i].y), notesL[i].extRate, 0, image.notes_flick_R, true);
            break;
        case flickUp:
            DrawRotaGraphF(float(notesL[i].x), float(notesL[i].y), notesL[i].extRate, 0, image.notes_flick_U, true);
            break;
        case flickDown:
            DrawRotaGraphF(float(notesL[i].x), float(notesL[i].y), notesL[i].extRate, 0, image.notes_flick_D, true);
            break;
        case exTap:
            DrawRotaGraphF(float(notesL[i].x), float(notesL[i].y), notesL[i].extRate, 0, image.notes_tap_EX, true);
            break;
        default:
            break;
        }
    }
}
void drawNotesR(image_ image) {
    //hold�֘A
    for (int i = 0; i < NOTES_NUM; i++) {
        if (ssf->notesType_R[i] == holdStart && notesR[i].deg >= startDeg && notesR[ssf->hold.connect_R[i]].flag) {
            DrawCircleGauge(1300, 666, min(notesR[i].deg, 70.0) / 360.0 * 100, image.notes_holdStrut, max(startDeg, notesR[ssf->hold.connect_R[i]].deg) / 360.0 * 100,1.0,1,0);
            if (notesR[ssf->hold.connect_R[i]].deg < 7.0) {
                DrawRotaGraph(int(1300 - 550 * sin(startDeg * M_PI / 180)), int(666 - 550 * cos(startDeg * M_PI / 180)), notesR[i].extRate, 0, image.notes_hold, true);
            }
        }
    }
    if (!notesR[number_R_hold].flag && notesR[ssf->hold.connect_R[number_R_hold]].flag && ssf->notesType_R[number_R_hold] == holdStart) DrawGraph(727, 423, image.notes_hold, true);

    for (int i = NOTES_NUM - 1; i >= 0; i--) {
        if (!(notesR[i].flag && notesR[i].deg >= startDeg && notesR[i].deg <= 85)) continue;

        //�o�����̑傫���ς�����
        {if (notesR[i].deg <= 8.0) notesR[i].extRate = 0.6;
        else if (notesR[i].deg <= 8.5) notesR[i].extRate = 0.7;
        else if (notesR[i].deg <= 9.0) notesR[i].extRate = 0.8;
        else if (notesR[i].deg <= 9.5) notesR[i].extRate = 0.9;
        else if (notesR[i].deg <= 10.0) notesR[i].extRate = 1.0;
        else if (notesR[i].deg <= 10.5) notesR[i].extRate = 1.1;
        else if (notesR[i].deg <= 11.0) notesR[i].extRate = 1.2;
        else if (notesR[i].deg <= 11.5) notesR[i].extRate = 1.3;
        else if (notesR[i].deg <= 12.0) notesR[i].extRate = 1.4;
        else if (notesR[i].deg <= 12.5) notesR[i].extRate = 1.5;
        else if (notesR[i].deg <= 13.0) notesR[i].extRate = 1.6;
        else if (notesR[i].deg <= 13.5) notesR[i].extRate = 1.7;
        else if (notesR[i].deg <= 14.0) notesR[i].extRate = 1.8;
        else if (notesR[i].deg <= 14.5) notesR[i].extRate = 1.9;
        else if (notesR[i].deg <= 15.0) notesR[i].extRate = 2.0;
        else if (notesR[i].deg <= 18.5) notesR[i].extRate = 1.9;
        else if (notesR[i].deg <= 19.0) notesR[i].extRate = 1.8;
        else if (notesR[i].deg <= 19.5) notesR[i].extRate = 1.7;
        else if (notesR[i].deg <= 20.0) notesR[i].extRate = 1.6;
        else if (notesR[i].deg <= 20.5) notesR[i].extRate = 1.5;
        else if (notesR[i].deg <= 21.0) notesR[i].extRate = 1.4;
        else if (notesR[i].deg <= 21.5) notesR[i].extRate = 1.3;
        else if (notesR[i].deg <= 22.0) notesR[i].extRate = 1.2;
        else if (notesR[i].deg <= 22.5) notesR[i].extRate = 1.1;
        else if (notesR[i].deg <= 70.0) notesR[i].extRate = 1.0; }

        switch (ssf->notesType_R[i]) {
        case tap:
            DrawRotaGraphF(float(notesR[i].x), float(notesR[i].y), notesR[i].extRate, 0, image.notes_tap, true);
            break;
        case holdStart:
            DrawRotaGraphF(float(notesR[i].x), float(notesR[i].y), notesR[i].extRate, 0, image.notes_hold, true);
            break;
        case holdEnd:
            DrawRotaGraphF(float(notesR[i].x), float(notesR[i].y), 1.0, 0, image.notes_hold, true);
            break;
        case flickLeft:
            DrawRotaGraphF(float(notesR[i].x), float(notesR[i].y), notesR[i].extRate, 0, image.notes_flick_L, true);
            break;
        case flickRight:
            DrawRotaGraphF(float(notesR[i].x), float(notesR[i].y), notesR[i].extRate, 0, image.notes_flick_R, true);
            break;
        case flickUp:
            DrawRotaGraphF(float(notesR[i].x), float(notesR[i].y), notesR[i].extRate, 0, image.notes_flick_U, true);
            break;
        case flickDown:
            DrawRotaGraphF(float(notesR[i].x), float(notesR[i].y), notesR[i].extRate, 0, image.notes_flick_D, true);
            break;
        case exTap:
            DrawRotaGraphF(float(notesR[i].x), float(notesR[i].y), notesR[i].extRate, 0, image.notes_tap_EX, true);
            break;
        default:
            break;
        }
    }
}

void drawSameTimeNotes(double currentTime){
    for (int i = 0; i < NOTES_NUM; i++) {      //L
        for (int j = 0; j < NOTES_NUM; j++) {  //R
            if (!(notesL[i].flag && notesR[j].flag)) continue;
            if (ssf->perfect_times_L[i] != ssf->perfect_times_R[j]) continue;
            if (notesL[i].deg < startDeg && notesR[j].deg < startDeg) continue;
            if (notesL[i].deg > 85 && notesR[j].deg > 85) continue;
            if (currentTime - ssf->perfect_times_L[i] > 0.0 && ssf->notesType_L[i] == holdStart)
            {
                DrawLineAA(float(1280 - notesR[j].x), float(notesL[i].y + notesR[j].y) / 2, float(notesR[j].x), float(notesL[i].y + notesR[j].y) / 2, GetColor(255, 255, 255), 3);
            }
            else if (currentTime - ssf->perfect_times_L[i] > 0.0 && ssf->notesType_R[j] == holdStart)
            {
                DrawLineAA(float(notesL[i].x), float(notesL[i].y + notesR[j].y) / 2, float(1280 - notesL[i].x), float(notesL[i].y + notesR[j].y) / 2, GetColor(255, 255, 255), 3);
            }
            else DrawLineAA(float(notesL[i].x), float(notesL[i].y), float(notesR[j].x), float(notesR[j].y), GetColor(255, 255, 255), 3);
        }
    }
}
void drawClearLevel(int clearLevel) {
    if (clearLevel >= 3) {
        DrawCircleAA(625, 430, 8, 32, GetColor(240, 240, 240), 1);
        DrawCircleAA(625, 430, 5, 32, GetColor(255, 160, 0  ), 1);
    }
    if (clearLevel >= 4) {
        DrawCircleAA(655, 430, 8, 32, GetColor(240, 240, 240), 1);
        DrawCircleAA(655, 430, 5, 32, GetColor(160, 160, 255), 1);
    }
}
void drawJudge(double currentTime, image_ image) {
    double extRate_judgeString = 1.0;
    int darkLevel_judgeString = 10;
    double sinceJudgedTime = currentTime - startCount_Judge;
    if (sinceJudgedTime >= 1.0) {
        judgeRank = 0;
    }
    //���Ԃŉ摜�̑傫���ƔZ����ύX
    {
        if (judgeRank == 4 || judgeRank == 5) { 
            extRate_judgeString = 1.0;
            darkLevel_judgeString = 10;
        }
        else {
            if (sinceJudgedTime <= 0.01) extRate_judgeString = 0.1;
            else if (sinceJudgedTime <= 0.018) extRate_judgeString = 0.2;
            else if (sinceJudgedTime <= 0.026) extRate_judgeString = 0.3;
            else if (sinceJudgedTime <= 0.034) extRate_judgeString = 0.4;
            else if (sinceJudgedTime <= 0.042) extRate_judgeString = 0.5;
            else if (sinceJudgedTime <= 0.050) extRate_judgeString = 0.6;
            else if (sinceJudgedTime <= 0.058) extRate_judgeString = 0.7;
            else if (sinceJudgedTime <= 0.066) extRate_judgeString = 0.8;
            else if (sinceJudgedTime <= 0.054) extRate_judgeString = 0.9;
            else if (sinceJudgedTime <= 0.062) extRate_judgeString = 1.0;
            else if (sinceJudgedTime <= 0.070) extRate_judgeString = 1.1;
            else if (sinceJudgedTime <= 0.078) extRate_judgeString = 1.2;
            else if (sinceJudgedTime <= 0.090) extRate_judgeString = 1.3;
            else if (sinceJudgedTime <= 0.105) extRate_judgeString = 1.2;
            else if (sinceJudgedTime <= 0.117) extRate_judgeString = 1.1;
            else if (sinceJudgedTime <= 0.125) extRate_judgeString = 1.0;

            if (sinceJudgedTime <= 0.0) darkLevel_judgeString = 0;
            else if (sinceJudgedTime <= 0.82) darkLevel_judgeString = 10;
            else if (sinceJudgedTime <= 0.84) darkLevel_judgeString = 9;
            else if (sinceJudgedTime <= 0.86) darkLevel_judgeString = 8;
            else if (sinceJudgedTime <= 0.88) darkLevel_judgeString = 7;
            else if (sinceJudgedTime <= 0.90) darkLevel_judgeString = 6;
            else if (sinceJudgedTime <= 0.92) darkLevel_judgeString = 5;
            else if (sinceJudgedTime <= 0.94) darkLevel_judgeString = 4;
            else if (sinceJudgedTime <= 0.96) darkLevel_judgeString = 3;
            else if (sinceJudgedTime <= 0.98) darkLevel_judgeString = 2;
            else if (sinceJudgedTime <= 1.00) darkLevel_judgeString = 1;
        }
    }
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, int(25.6 * darkLevel_judgeString));//�Z��
    if (sinceJudgedTime < 1.0) {
        switch (judgeRank) {
        case 1:
            DrawRotaGraph(640, 400, extRate_judgeString, 0, image.judgeString_wonderful, true);
            break;
        case 2:
            DrawRotaGraph(640, 400, extRate_judgeString, 0, image.judgeString_great, true);
            break;
        case 3:
            DrawRotaGraph(640, 400, extRate_judgeString, 0, image.judgeString_nice, true);
            break;
        case 4:
            DrawRotaGraph(640, 400, extRate_judgeString, 0, image.judgeString_bad, true);
            break;
        case 5:
            DrawRotaGraph(640, 400, extRate_judgeString, 0, image.judgeString_miss, true);
            break;
        default:
            break;
        }
    }
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND,0);
//    clsDx();
//    printfDx("rank=%d,count_judge=%f",judgeRank,double(startCount_Judge));
}

//drawCombo->���U���g�ł��g���̂�draw.cpp
//drawScore->���U���g�ł��g���̂�draw.cpp

//�Z�b�g�A�b�v�܂Ƃ�
void setup_notes() {
    setNotes();
    setJudge();
    setScore();
    createNotes();
}
//����܂Ƃ�
void judge_notes(double currentTime, bool autoMode) 
{
    (autoMode) ? judgeNotes_auto(currentTime) : judgeNotes(currentTime);
    if (holding_L) {
        judge_holding(currentTime, 'L');
        if (!autoMode) judge_holding_quit(currentTime, 'L');
    }
    if (holding_R) {
        judge_holding(currentTime, 'R');
        if (!autoMode) judge_holding_quit(currentTime, 'R');
    }
    clearMark = checkClearLevel();
}
//�`��܂Ƃ�
void draw_notes(double currentTime, image_ image) {
    DrawGraph(727, 423, image.judgeFrame, true);
    DrawGraph(443, 423, image.judgeFrame, true);
    drawSameTimeNotes(currentTime);
    drawNotesL(image);
    drawNotesR(image);
    drawJudge(currentTime, image);
    drawClearLevel(clearMark);
    drawCombo(1106, 12, combo.combo, image, true);
    drawScore(1070, 656, totalScore, image, true);
    //DrawFormatString(10, 140, GetColor(255, 255, 255), "count:%d,%d,%d,%d,%d", combo.count[wonderful], combo.count[great], combo.count[nice], combo.count[bad], combo.count[miss]);
}
