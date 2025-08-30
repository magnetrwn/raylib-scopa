#include "config.h"
#include "minIni.h"
#include <stdbool.h>

int WINDOW_W;
int WINDOW_H;
bool FULLSCREEN;
float ATLAS_UPSCALE_MUL;
int FONT_BASE_LOAD;
float FONT_BASE_MUL;
int ATLAS_PAD_PX;
int CARD_REAR_THEME_IDX;

void CFG_Load(void) {
    WINDOW_W = ini_getl("Window", "WINDOW_W", 0, CFG_INI_PATH);
    WINDOW_H = ini_getl("Window", "WINDOW_H", 0, CFG_INI_PATH);
    FULLSCREEN = ini_getbool("Window", "FULLSCREEN", false, CFG_INI_PATH);
    ATLAS_UPSCALE_MUL = ini_getf("Quality", "ATLAS_UPSCALE_MUL", 2.0f, CFG_INI_PATH);
    FONT_BASE_LOAD = ini_getl("Quality", "FONT_BASE_LOAD", 96, CFG_INI_PATH);
    FONT_BASE_MUL = ini_getf("Quality", "FONT_BASE_MUL", 8.0f, CFG_INI_PATH);
    ATLAS_PAD_PX = ini_getl("Quality", "ATLAS_PAD_PX", 4, CFG_INI_PATH);
    CARD_REAR_THEME_IDX = ini_getl("Options", "CARD_REAR_THEME_IDX", 0, CFG_INI_PATH);
}

void CFG_Save(void) {

}