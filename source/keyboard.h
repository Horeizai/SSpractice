#pragma once

int updateKey();
int checkKey(int keyName);
bool isMoveSongBlock(int keyNumber);
void getSeparateFlag(bool separateFlag);//flagŽæ“¾
int countNotesKeyPressed(char LR);
bool isNotesKeyPressing(char LR);

typedef enum {
	upKey,
	downKey,
	leftKey,
	rightKey,
	enterKey,
	escKey
}key_songselect;