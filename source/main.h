#pragma once

#include "DxLib.h"
#define GET_ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "load.h"

//ÉVÅ[Éìèàóù
typedef enum {
	scene_black,
	scene_songSelect,
	scene_live,
	scene_result,
	scene_end
}scene_e;
typedef enum {
	bigFont_e,
	bigFont2_e,
	smallFont_e,
	resultTitleFont_e
}font_e;

void setScene(int scene_);
int getFontHandle(int fontNumber);
config_ getConfig(void);


