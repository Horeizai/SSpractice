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

//ループ処理
int ProcessLoop() {
	if (ScreenFlip() != 0) return -1;
	if (ProcessMessage() != 0) return -1;//プロセス処理がエラーなら-1を返す
	if (ClearDrawScreen() != 0) return -1;//画面クリア処理がエラーなら-1を返す
	if (updateKey() != 0) return -1;//現在のキー入力処理を行う
	return 0;
}
//シーン変更処理
int scene = scene_songSelect;
int scene_changing = scene_black;
bool isSceneChanging = false;
config_ config;

void setScene(int scene_) { scene = scene_; }
//フォント処理
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

//config渡し
config_ getConfig(void) {
	return config;
}

////////////////////////////////////////////////////////////////////////////////////

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	config.loadConfig(&config, "config.ini");
	// 画面モードの設定
	ChangeWindowMode(TRUE);								//ウィンドウモード
	SetMainWindowText("SSplactice v1.0.0");				//ウインドウのタイトルを設定
	SetWindowIconID(IDI_ICON1);							//アイコンを設定
	SetGraphMode(1280, 720, 16, 15);					//ウインドウサイズ
	SetWindowSizeExtendRate(config.windowWidth/1280.0);	//横幅によって画面の大きさを変更
	SetWindowSizeChangeEnableFlag(FALSE);				//画面サイズ変更不可
	SetWaitVSyncFlag(config.isV_SYNC);					//垂直同期(設定で可変にしたい)
	SetAlwaysRunFlag(TRUE);
	// ＤＸライブラリ初期化処理
	if (DxLib_Init() == -1) return -1;// エラーが起きたら直ちに終了
	// 描画先画面を裏画面にセット
	SetDrawScreen(DX_SCREEN_BACK);

	//データのロード
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
	
	DxLib_End();			// ＤＸライブラリ使用の終了処理
	return 0;				// ソフトの終了
}