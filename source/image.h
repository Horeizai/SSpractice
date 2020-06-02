#pragma once
class image
{
public:
	//背景
	int bg_select, bg_live, bg_result;
	//選曲
	int noImage;//ジャケット指定がないときの画像
	//ライブ
	int tap;
	int flickUp, flickDown, flickRight, flickLeft;
	int hold, holdStrut;
	int judgeCircle;
	int spGauge;
	int strategyRight, strategyLeft;
	int stopButton;
	//全般
	int number;
};

