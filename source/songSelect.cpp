#include "main.h"
#include "load.h"
#include "keyboard.h"
#include "songSelect.h"
#include "live.h"
#include "result.h"
#include "ssf_.h"
#include "draw.h"

#define SSF_NUM 1000
#define GENRE_NUM 100
#define SONGBLOCK_NUM 21
#define SONGBLOCK_MOVESIZE_Y 104
#define SONGBLOCK_MOVESIZE_X 600
#define SONGBLOCK_SELECT songBlock[10]
typedef enum {
	genre,
	ssf
}move_e;

SSFINFO ssfInfo[GENRE_NUM][SSF_NUM];
SAVEDATA savedata[GENRE_NUM][SSF_NUM];
GENREINFO genreInfo[GENRE_NUM];
SONGBLOCK songBlock[SONGBLOCK_NUM];
static config_ config;

//ファイル走査用の変数
char dir[260] = { "SSp_File" };
bool isGenreSelect = true;
int dirDepth = 0;
int currentGenre = 0;
int cnt_genre = 0;
int cnt_file[GENRE_NUM];
//左上とエラー用の変数
int startCount_error;
static char statusMessage[260];
static char errorMessage[260];
static double nowTime_status = 1.0;
static double nowTime_error;
bool isError = false;
//SongBlock移動用の変数
int startCount_move;
static double nowTime_move;
int num_move[2] = {0,0};
int cnt_moving = 0;
int moveSize = 0;
bool isMoving_x = false;
bool isMoving_y = false;
//フォントハンドル格納
static int titleFont;
static int titleFont_sb;
static int subTitleFont;

#pragma region functions
void createSongBlock();
void updateSongBlock();
void moveSongBlock();
void drawSongBlock(image_ image);
void keyProcess();

void setGenreInfo(void);
void search_ssfFile(char dirName[260]);
void analyze_ssfFile();
void analyze_datFile();
void analyze_genreIni();

void drawChartInfo(image_ image);
void draw_songSelect(image_ image);
#pragma endregion


bool once = true;
bool isOK_VSYNC = false;
//----------------------------------------------------------------------------------
void song_select_main(image_ image, sound_ sound) {
	if(once){
		config = getConfig();
		setStatus(config);
		setGenreInfo();
		search_ssfFile(dir);
		analyze_ssfFile();
		analyze_datFile();
		analyze_genreIni();
		createSongBlock();
		once = false;
		startCount_error = GetNowCount();
		//
		titleFont = getFontHandle(bigFont_e);
		titleFont_sb = getFontHandle(bigFont2_e);
		subTitleFont = getFontHandle(smallFont_e);
	}
	if(isOK_VSYNC) keyProcess();
	changeStatus();
	moveSongBlock();
	draw_songSelect(image);
	getStatusMessage(statusMessage, &nowTime_status);
	drawStatus(-1, nowTime_status, statusMessage);

	nowTime_error = (double(GetNowCount()) - startCount_error) / 1000.0;
	if (2 <= nowTime_error) isError = false;
	if (isError) {
		drawErrowWindow(nowTime_error, errorMessage);
	}
	if (!isOK_VSYNC) {
		if (config.isV_SYNC == 0) {
			isOK_VSYNC = true;
			return;
		}
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
		DrawBox(400, 220, 880, 440, GetColor(255, 255, 255), 1);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
		DrawFormatString(405, 315, GetColor(255, 255, 255), "　　　　垂直同期がONのため，\n　　プレイ結果は保存されません。");
		DrawFormatStringToHandle(537, 412, GetColor(255, 255, 255), subTitleFont, "Press SpaceKey or EnterKey");
		if (checkKey(KEY_INPUT_RETURN) || checkKey(KEY_INPUT_SPACE)) isOK_VSYNC = true;
	}
}
//----------------------------------------------------------------------------------
void setGenreInfo(void) {
	for (int i = 0; i < GENRE_NUM; i++) {
		strcpyDx(genreInfo[i].genreName, "genre.iniがありません");
		genreInfo[i].genreColor = GetColor(160, 160, 160);
		genreInfo[i].fontColor = GetColor(255, 255, 255);
	}
}

void search_ssfFile(char dirName[260]) {
	FILEINFO FileInfo;
	DWORD_PTR FindHandle;
	char dirName_search[260];
	char dirName_recursion[260];

	// 検索開始、最初のファイルの情報を取得
	// ( *.* や ???.* などのシンプルなワイルドカード表現に対応しています )
	sprintfDx(dirName_search, "%s\\*.*", dirName);
	FindHandle = FileRead_findFirst(dirName_search, &FileInfo);

	// ファイルが見つかった場合のみ分岐の中を実行
	if (FindHandle == -1) return;
	do
	{
		//フォルダの場合，そのフォルダの中身を調べるような再帰関数にする
		if (FileInfo.DirFlag) {
			//"." ".."はスキップ
			if (_tcscmp(FileInfo.Name, ".") == 0 || _tcscmp(FileInfo.Name, "..") == 0) continue;
			
			if (dirDepth == 0) {
				currentGenre = cnt_genre;
				cnt_genre++;
			}
			dirDepth++;
			sprintfDx(dirName_recursion, "%s\\%s", dirName, FileInfo.Name);
			search_ssfFile(dirName_recursion);
		}
		//ファイルの場合，そのファイル名を格納する
		else {
			char fileName_copy[260], str[260], * extension, * next_token = NULL;
			//ファイル名をコピー後に分割して拡張子を抽出
			sprintfDx(fileName_copy, "%s", FileInfo.Name);
			extension = strtok_s(fileName_copy, ".", &next_token);
			while (extension != NULL) {
				sprintfDx(str, "%s", extension);
				extension = strtok_s(NULL, ".", &next_token);
			}
			if (strcmpDx(str, "ssf") == 0) {
				strcpyDx(ssfInfo[currentGenre][cnt_file[currentGenre]].fileName, FileInfo.Name);
				strcpyDx(ssfInfo[currentGenre][cnt_file[currentGenre]].dirPath, dirName);
//				int len = strlen(FileInfo.Name);
				sprintfDx(ssfInfo[currentGenre][cnt_file[currentGenre]].datName, "%s.dat", fileName_copy);

				cnt_file[currentGenre]++;
			}
			if (strcmpDx(FileInfo.Name, "genre.ini") == 0) {
				sprintfDx(genreInfo[currentGenre].pass, "%s\\%s", dirName, FileInfo.Name);
			}
		}
		// 検索に引っかかる次のファイルの情報を取得、無かったらループを抜ける
	} while (FileRead_findNext(FindHandle, &FileInfo) >= 0);

	// 検索ハンドルの後始末
	FileRead_findClose(FindHandle);
	// 再帰終了の処理
	if (dirDepth == 1) currentGenre = 0;
	dirDepth--;
}
//ファイル名とssf_infoを渡してssf_infoを更新する関数
void analyze_ssfFile() {
	char filePath[260], path[260];
	for (int i = 0; i < cnt_genre; i++)
	{
		for (int j = 0; j < cnt_file[i]; j++)
		{
			sprintfDx(filePath, "%s\\%s", ssfInfo[i][j].dirPath, ssfInfo[i][j].fileName);
			loadSongData(&ssfInfo[i][j], filePath);

			sprintfDx(path, "%s\\%s", ssfInfo[i][j].dirPath, ssfInfo[i][j].jacketPath);
			ssfInfo[i][j].jacketHandle = LoadGraph(path);
			sprintfDx(path, "%s\\%s", ssfInfo[i][j].dirPath, ssfInfo[i][j].bgPath);
			ssfInfo[i][j].bgHandle = LoadGraph(path);
			sprintfDx(path, "%s\\%s", ssfInfo[i][j].dirPath, ssfInfo[i][j].wavePath);
			ssfInfo[i][j].waveHandle = LoadSoundMem(path);
			SetSoundCurrentTime(int(ssfInfo[i][j].demoStart * 1000), ssfInfo[i][j].waveHandle);
			ChangeVolumeSoundMem(255 * ssfInfo[i][j].songVolime / 100, ssfInfo[i][j].waveHandle);
		}
	}
}
void analyze_datFile() {
	char filePath[260];
	for (int i = 0; i < cnt_genre; i++)
	{
		for (int j = 0; j < cnt_file[i]; j++)
		{
			sprintfDx(filePath, "%s\\%s", ssfInfo[i][j].dirPath, ssfInfo[i][j].datName);
			FILE* fp;
			if ((fopen_s(&fp, filePath, "rb")) != 0 || fp == 0) { // "fpが0である可能性があります"というエラー対策
				continue;
			}
			fread_s(&savedata[i][j], sizeof(savedata[i][j]), sizeof(savedata[i][j]), 1, fp);
			fclose(fp);
		}
	}
}
void analyze_genreIni() {
	for (int i = 0; i < cnt_genre; i++)
	{
		loadGenreIni(&genreInfo[i], genreInfo[i].pass);
	}
}

#pragma region SongBlock

void createSongBlock() {
	for (int i = 0; i < SONGBLOCK_NUM; i++) songBlock[i].x = 50;
	updateSongBlock();
}
void updateSongBlock() {
	//ssfやジャンルの情報だけ移動
	if (isGenreSelect) {
		currentGenre = 0;
		num_move[genre] -= moveSize;
		sprintfDx(songBlock[10].title, genreInfo[((num_move[genre] % cnt_genre) + cnt_genre) % cnt_genre].genreName);
		songBlock[10].genreNumber = ((num_move[genre] % cnt_genre) + cnt_genre) % cnt_genre;
		for (int i = 1; i <= SONGBLOCK_NUM / 2; i++) {
			sprintfDx(songBlock[10 + i].title, genreInfo[(((num_move[genre] + i) % cnt_genre) + cnt_genre) % cnt_genre].genreName);
			sprintfDx(songBlock[10 - i].title, genreInfo[(((num_move[genre] - i) % cnt_genre) + cnt_genre) % cnt_genre].genreName);
			songBlock[10 + i].genreNumber = ((num_move[genre] + i) % cnt_genre + cnt_genre) % cnt_genre;
			songBlock[10 - i].genreNumber = ((num_move[genre] - i) % cnt_genre + cnt_genre) % cnt_genre;
		}
	}
	else {
		currentGenre = songBlock[10].genreNumber;
		num_move[ssf] -= moveSize;
		sprintfDx(songBlock[10].title, ssfInfo[currentGenre][((num_move[ssf] % cnt_file[currentGenre]) + cnt_file[currentGenre]) % cnt_file[currentGenre]].title);
		songBlock[10].genreNumber = currentGenre;
		songBlock[10].info = ssfInfo[currentGenre][((num_move[ssf] % cnt_file[currentGenre]) + cnt_file[currentGenre]) % cnt_file[currentGenre]];
		songBlock[10].dat = savedata[currentGenre][((num_move[ssf] % cnt_file[currentGenre]) + cnt_file[currentGenre]) % cnt_file[currentGenre]];
		for (int i = 1; i <= SONGBLOCK_NUM / 2; i++) {
			//マイナス方向へのループ性を持たせたら怪文になった
			int fileNum_plus  = ((num_move[ssf] + i) % cnt_file[currentGenre] + cnt_file[currentGenre]) % cnt_file[currentGenre];
			int fileNum_minus = ((num_move[ssf] - i) % cnt_file[currentGenre] + cnt_file[currentGenre]) % cnt_file[currentGenre];

			sprintfDx(songBlock[10 + i].title, ssfInfo[currentGenre][fileNum_plus].title);
			sprintfDx(songBlock[10 - i].title, ssfInfo[currentGenre][fileNum_minus].title);
			songBlock[10 + i].info = ssfInfo[currentGenre][fileNum_plus];
			songBlock[10 - i].info = ssfInfo[currentGenre][fileNum_minus];
			songBlock[10 + i].dat = savedata[currentGenre][fileNum_plus];
			songBlock[10 - i].dat = savedata[currentGenre][fileNum_minus];
			songBlock[10 + i].genreNumber = currentGenre;
			songBlock[10 - i].genreNumber = currentGenre;
		}
	}
	//ブロックのy位置だけ初期化
	for (int i = 0; i < SONGBLOCK_NUM; i++) {
		songBlock[i].y = SONGBLOCK_MOVESIZE_Y * i - SONGBLOCK_MOVESIZE_Y * 7 + 3;
	}
}
void moveSongBlock() {
	if (isMoving_y) {
		if (cnt_moving == 0) {
			StopSoundMem(songBlock[10].info.waveHandle);
			SetSoundCurrentTime(int(songBlock[10].info.demoStart * 1000), songBlock[10].info.waveHandle);
			startCount_move = GetNowCount();
			cnt_moving++;
		}
		nowTime_move = (double(GetNowCount()) - startCount_move) / 1000.0;
		for (int i = 0; i < SONGBLOCK_NUM; i++)
		{
			for (int j = 0; j < 52; j++)
			{
				if ( j*0.005 <= nowTime_move) songBlock[i].y = SONGBLOCK_MOVESIZE_Y * i - SONGBLOCK_MOVESIZE_Y * 7 + 3 + int(moveSize * 2 * j);
			}
		}
		if (nowTime_move >= 0.26) {
			updateSongBlock();
			moveSize = 0;
			isMoving_y = false;
			cnt_moving = 0;
			if (!isGenreSelect) PlaySoundMem(songBlock[10].info.waveHandle, DX_PLAYTYPE_BACK, false);
		}
	}
	else if (isMoving_x) {
		if (cnt_moving == 0) {
			startCount_move = GetNowCount();
			cnt_moving++;
		}
		nowTime_move = (double(GetNowCount()) - startCount_move) / 1000.0;
		for (int i = 0; i < SONGBLOCK_NUM; i++)
		{
			for (int j = 0; j < 50; j++)
			{
				if (j * 0.005 <= nowTime_move && cnt_moving == 1) songBlock[i].x = 50 - (j * 12);
				if (j * 0.005 + 0.25 <= nowTime_move) songBlock[i].x = 50 - (600 - j * 12);
			}
		}
		if (nowTime_move >= 0.25 && cnt_moving == 1) {
			isGenreSelect = !isGenreSelect;
			updateSongBlock();
			cnt_moving++;
		}
		if (nowTime_move >= 0.50) {
			isMoving_x = false;
			cnt_moving = 0;
			for (int i = 0; i < SONGBLOCK_NUM; i++) songBlock[i].x = 50;
			if (!isGenreSelect) PlaySoundMem(songBlock[10].info.waveHandle, DX_PLAYTYPE_BACK, false);
		}
	}
}
//↓カウント依存移動法
/*
void moveSongBlockNoVSYNC() {
	if (isMoving_y) {
		if (cnt_moving == 0) {
			StopSoundMem(songBlock[10].info.waveHandle);
			SetSoundCurrentTime(int(songBlock[10].info.demoStart * 1000), songBlock[10].info.waveHandle);
		}
		for (int i = 0; i < SONGBLOCK_NUM; i++)
		{
			if (cnt_moving < SONGBLOCK_MOVESIZE_Y) songBlock[i].y += moveSize;
		}
		cnt_moving++;
		if (cnt_moving == SONGBLOCK_MOVESIZE_Y) {
			updateSongBlock();
			moveSize = 0;
			isMoving_y = false;
			cnt_moving = 0;
			if (!isGenreSelect) PlaySoundMem(songBlock[10].info.waveHandle, DX_PLAYTYPE_BACK, false);
		}
	}
	else if (isMoving_x) {
		for (int i = 0; i < SONGBLOCK_NUM; i++)
		{
			if (cnt_moving < 125) songBlock[i].x -= 4;
			else if (cnt_moving < 250)songBlock[i].x += 4;
		}
		cnt_moving++;
		if (cnt_moving == 125) {
			isGenreSelect = !isGenreSelect;
			updateSongBlock();
		}
		if (cnt_moving == 250) {
			isMoving_x = false;
			cnt_moving = 0;
			for (int i = 0; i < SONGBLOCK_NUM; i++) songBlock[i].x = 50;
			if (!isGenreSelect) PlaySoundMem(songBlock[10].info.waveHandle, DX_PLAYTYPE_BACK, false);
		}
	}
}
*/
#pragma endregion

void keyProcess() {
	if (isMoveSongBlock(upKey)) {
		moveSize = 1;
		if(!isMoving_x) isMoving_y = true;
	}
	if (isMoveSongBlock(downKey)) {
		moveSize = -1;
		if (!isMoving_x) isMoving_y = true;
	}
	if (isMoveSongBlock(leftKey)) {
		moveSize = 7;
		if (!isMoving_x) isMoving_y = true;
	}
	if (isMoveSongBlock(rightKey)) {
		moveSize = -7;
		if (!isMoving_x) isMoving_y = true;
	}
	if (isMoveSongBlock(enterKey)) {
		if (isMoving_y || isMoving_x) return;
		if (isGenreSelect) {
			//移動先のジャンルに譜面がない場合、エラー
			if (cnt_file[songBlock[10].genreNumber] == 0) {
				isError = true;
				strcpyDx(errorMessage, "このフォルダには譜面がありません！");
				startCount_error = GetNowCount();
				return;
			}
			isMoving_x = true;
		}
		else {
			char path[260];
			sprintfDx(path, "%s\\%s", songBlock[10].info.dirPath, songBlock[10].info.datName);

			StopSoundMem(songBlock[10].info.waveHandle);
			SetSoundCurrentTime(int(songBlock[10].info.demoStart * 1000), songBlock[10].info.waveHandle);
			get_ssfInfo(songBlock[10].info);
			getFilePath(path);
			setScene(scene_live);
		}
	}
	if (isMoveSongBlock(escKey)) {
		StopSoundMem(songBlock[10].info.waveHandle);
		SetSoundCurrentTime(int(songBlock[10].info.demoStart * 1000), songBlock[10].info.waveHandle);
		if (isGenreSelect) setScene(scene_end);
		else {
			isMoving_x = true;
		}
	}

}

//描画
void drawSongBlock(image_ image) {
	for (int i = 0; i < SONGBLOCK_NUM; i++) {
		int c = 1, fullC = 1, perfectL = 1;
		//ブロックのフチ
		DrawBox(songBlock[i].x, songBlock[i].y, songBlock[i].x + 450, songBlock[i].y + 90, GetColor(128, 128, 128), 1);
		DrawBox(songBlock[i].x, songBlock[i].y, songBlock[i].x + 448, songBlock[i].y + 88, GetColor(255, 255, 255), 1);
		//ブロック本体
		DrawBox(songBlock[i].x + 2, songBlock[i].y + 2, songBlock[i].x + 448, songBlock[i].y + 88, genreInfo[songBlock[i].genreNumber].genreColor, 1);
		DrawFormatStringToHandle(songBlock[i].x + 5, songBlock[i].y + 5, genreInfo[songBlock[i].genreNumber].fontColor, titleFont_sb, songBlock[i].title);
		if (!isGenreSelect) 
		{
			DrawGraph(songBlock[i].x + 5, songBlock[i].y + 45, image.level[songBlock[i].info.level], 1);
			if (songBlock[i].dat.mark == 4) perfectL = perfectLive;
			if (songBlock[i].dat.mark >= 3) fullC = fullCombo;
			if (songBlock[i].dat.mark >= 2) c = clear;
			DrawGraph(songBlock[i].x + 325, songBlock[i].y + 45, image.trophy[c], 1);
			DrawGraph(songBlock[i].x + 365, songBlock[i].y + 45, image.trophy[fullC], 1);
			DrawGraph(songBlock[i].x + 405, songBlock[i].y + 45, image.trophy[perfectL], 1);

		}
	}
}
void drawChartInfo(image_ image) {
	//フチ
	DrawBox(520, 100, 1250, 700, GetColor(128, 128, 128), 1);
	DrawBox(520, 100, 1247, 697, genreInfo[songBlock[10].genreNumber].genreColor, 1);
	//本体
	DrawBox(523, 103, 1247, 697, GetColor(234, 234, 234), 1);
	//
	if (!isGenreSelect) {
		DrawGraph(540, 115, image.level[songBlock[10].info.level], 1);
		DrawFormatStringToHandle(540, 160, GetColor(255, 255, 255), titleFont, songBlock[10].info.title);
		DrawFormatStringToHandle(540, 220, GetColor(255, 255, 255), subTitleFont, songBlock[10].info.subTitle);
		DrawFormatStringToHandle(1100, 220, GetColor(255, 255, 255), subTitleFont, "BPM:%s", songBlock[10].info.bpm);

		DrawBox(747, 247, 1233, 523, GetColor(255, 255, 255), 1);
		if(songBlock[10].info.bgHandle <= 0) DrawExtendGraph(750, 250, 1230, 520, image.noimage_b, 1);
		else DrawExtendGraph(750, 250, 1230, 520, songBlock[10].info.bgHandle, 1);
		DrawQuadrangleAA(537, 267, 783, 257, 793, 503, 547, 513, GetColor(255, 255, 255), 1);
		SetDrawMode(DX_DRAWMODE_BILINEAR);
		if (songBlock[10].info.jacketHandle <= 0) DrawModiGraph(540, 270, 780, 260, 790, 500, 550, 510, image.noimage_j, 1);
		else DrawModiGraph(540, 270, 780, 260, 790, 500, 550, 510, songBlock[10].info.jacketHandle, 1);
		SetDrawMode(DX_DRAWMODE_NEAREST);

		DrawGraph(540, 570, image.clearMark[songBlock[10].dat.mark], 1);
		DrawFormatString(540, 630, GetColor(255, 255, 255), "BEST SCORE:%d", songBlock[10].dat.bestScore);
		DrawFormatStringToHandle(540, 670, GetColor(255, 255, 255), subTitleFont,"Notes Designer:%s",songBlock[10].info.designer);
	}
}

void draw_songSelect(image_ image) {
	DrawGraph(0, 0, image.bg_songSelect, 1);
	drawSongBlock(image);
	drawChartInfo(image);
	//選択枠
	DrawBox(45, 310, 50, 405, GetColor(80, 80, 255), 1);
	DrawBox(45, 405, 500, 410, GetColor(80, 80, 255), 1);
	DrawBox(50, 310, 505, 315, GetColor(80, 80, 255), 1);
	DrawBox(500, 310, 505, 410, GetColor(80, 80, 255), 1);
	DrawBox(47, 312, 503, 408, GetColor(40, 40, 255), 0);
	//new表示
	for (int i = 0; i < SONGBLOCK_NUM; i++)
	{
		if (!isGenreSelect && songBlock[i].dat.mark == 0) DrawGraph(songBlock[i].x, songBlock[i].y - 14, image.newMark, 1);
	}
}

void update_songBlock(void) {
	analyze_datFile();
	updateSongBlock();
}