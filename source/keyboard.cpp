#include "main.h"
#include "keyboard.h"

int key[260]; // キーが押されているフレーム数を格納する

// キーの入力状態を更新する
int updateKey() {
    char tmpKey[260]; // 現在のキーの入力状態を格納する
    GetHitKeyStateAll(tmpKey); // 全てのキーの入力状態を得る
    for (int i = 0; i < 260; i++) {
        if (tmpKey[i] != 0) { // i番のキーコードに対応するキーが押されていたら
            key[i]++;     // 加算
        }
        else {              // 押されていなければ
            key[i] = 0;   // 0にする
        }
    }
    return 0;
}
int key_L1 = KEY_INPUT_F, key_L2 = KEY_INPUT_D, key_R1 = KEY_INPUT_J, key_R2 = KEY_INPUT_K;
bool separateFlag_k;

int checkKey(int keyName) {
    return key[keyName];
}

bool isMoveSongBlock(int keyNumber) {
    switch (keyNumber)
    {
    case upKey:
        if (checkKey(KEY_INPUT_UP) || checkKey(key_L2)) return true;
        break;
    case downKey:
        if (checkKey(KEY_INPUT_DOWN) || checkKey(key_R2)) return true;
        break;
    case leftKey:
        if (checkKey(KEY_INPUT_LEFT)) return true;
        break;
    case rightKey:
        if (checkKey(KEY_INPUT_RIGHT)) return true;
        break;
    case enterKey:
        if (checkKey(KEY_INPUT_RETURN) == 1 || checkKey(KEY_INPUT_SPACE) == 1 || checkKey(key_L1) == 1 || checkKey(key_R1) == 1) return true;
        break;
    case escKey:
        if (checkKey(KEY_INPUT_ESCAPE) == 1 /*|| checkKey(KEY_INPUT_BACK) == 1*/) return true;
        break;
    default:
        break;
    }
    return false;
}

void getSeparateFlag(bool separateFlag) { separateFlag_k = separateFlag; }//flag取得
int countNotesKeyPressed(char LR){
    int cnt_key = 0;
    //左右で判定を分ける
    if (separateFlag_k) {
        switch (LR)
        {
        case 'L':
            if (checkKey(key_L1) == 1) cnt_key++;
            if (checkKey(key_L2) == 1) cnt_key++;
            break;
        case 'R':
            if (checkKey(key_R1) == 1) cnt_key++;
            if (checkKey(key_R2) == 1) cnt_key++;
            break;
        //both
        case 'B':
            if (checkKey(key_L1) == 1) cnt_key++;
            if (checkKey(key_L2) == 1) cnt_key++;
            if (checkKey(key_R1) == 1) cnt_key++;
            if (checkKey(key_R2) == 1) cnt_key++;
            break;
        default:
            break;
        }
    }
    //分けない
    else {
        if (checkKey(key_L1) == 1) cnt_key++;
        if (checkKey(key_L2) == 1) cnt_key++;
        if (checkKey(key_R1) == 1) cnt_key++;
        if (checkKey(key_R2) == 1) cnt_key++;
    }
    return cnt_key;
}
bool isNotesKeyPressing(char LR) {
    //左右で判定を分ける
    if (separateFlag_k) {
        switch (LR)
        {
        case 'L':
            if (checkKey(key_L1) == 0 && checkKey(key_L2) == 0) return false;
            break;
        case 'R':
            if (checkKey(key_R1) == 0 && checkKey(key_R2) == 0) return false;
            break;
        default:
            break;
        }
    }
    //分けない
    else {
        if (checkKey(key_L1) == 0 && checkKey(key_L2) == 0 && checkKey(key_R1) == 0 && checkKey(key_R2) == 0) return false;
    }
    //どこにも引っかからなければtrue
    return true;
}