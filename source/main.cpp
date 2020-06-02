#define _GLOBAL_INSTANCE_
#include "main.h"
#include "keyboard.h"
#include "config.h"
#include "live.h"
#include "songSelect.h"
#include "result.h"
#include "draw.h"
#include "notes.h"
#include "load.h"
#include "../resource.h"

//���[�v����
int ProcessLoop() {
	if (ScreenFlip() != 0) return -1;
	if (ProcessMessage() != 0) return -1;//�v���Z�X�������G���[�Ȃ�-1��Ԃ�
	if (ClearDrawScreen() != 0) return -1;//��ʃN���A�������G���[�Ȃ�-1��Ԃ�
	if (updateKey() != 0) return -1;//���݂̃L�[���͏������s��
	return 0;
}
//�V�[���ύX����
int scene = scene_songSelect;
int scene_changing = scene_black;
bool isSceneChanging = false;
config_ config;

void setScene(int scene_) { scene = scene_; }
//�t�H���g����
int bigFont, bigFont2, smallFont, resultTitleFont;
int getFontHandle(int fontNumber) {
	switch (fontNumber)
	{
	case bigFont_e:
		return bigFont;
	case bigFont2_e:
		return bigFont2;
	case smallFont_e:
		return smallFont;
	case resultTitleFont_e:
		return resultTitleFont;
	default:
		break;
	}
	return 0;
}

//config�n��
config_ getConfig(void) {
	return config;
}

////////////////////////////////////////////////////////////////////////////////////

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	config.loadConfig(&config, "config.ini");
	// ��ʃ��[�h�̐ݒ�
	ChangeWindowMode(TRUE);								//�E�B���h�E���[�h
	SetMainWindowText("SSplactice v1.0.0");				//�E�C���h�E�̃^�C�g����ݒ�
	SetWindowIconID(IDI_ICON1);							//�A�C�R����ݒ�
	SetGraphMode(1280, 720, 16, 15);					//�E�C���h�E�T�C�Y
	SetWindowSizeExtendRate(config.windowWidth/1280.0);	//�����ɂ���ĉ�ʂ̑傫����ύX
	SetWindowSizeChangeEnableFlag(FALSE);				//��ʃT�C�Y�ύX�s��
	SetWaitVSyncFlag(config.isV_SYNC);					//��������(�ݒ�ŉςɂ�����)
	SetAlwaysRunFlag(TRUE);
	// �c�w���C�u��������������
	if (DxLib_Init() == -1) return -1;// �G���[���N�����璼���ɏI��
	// �`����ʂ𗠉�ʂɃZ�b�g
	SetDrawScreen(DX_SCREEN_BACK);

	//�f�[�^�̃��[�h
	image_ image;
	sound_ sound;
	image.loadImage(&image);
	sound.loadSound(&sound);
	bigFont = CreateFontToHandle(NULL, 35, -1, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
	bigFont2 = CreateFontToHandle(NULL, 28, -1, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
	smallFont = CreateFontToHandle(NULL, 22, -1, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
	resultTitleFont = CreateFontToHandle(NULL, 42, -1, DX_FONTTYPE_ANTIALIASING_EDGE_4X4);
	SetFontSize(24);
	ChangeFontType(DX_FONTTYPE_ANTIALIASING_EDGE_4X4);

	while (ProcessLoop() == 0) {
		switch (scene)
		{
		case scene_songSelect:
			song_select_main(image, sound);
			break;
		case scene_live:
			live_main(image, sound);
			break;
		case scene_result:
			result_main(image, sound);
			break;
		default:
			break;
		}

		if (scene == scene_end) break;
	}
	
	DxLib_End();			// �c�w���C�u�����g�p�̏I������
	return 0;				// �\�t�g�̏I��
}