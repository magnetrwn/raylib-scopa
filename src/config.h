#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdbool.h>
#define CFG_INI_PATH "assets/config.ini"

// global config data (in .c file)
extern int WINDOW_W;
extern int WINDOW_H;
extern bool FULLSCREEN;
extern float ATLAS_UPSCALE_MUL;
extern int FONT_BASE_LOAD;
extern float FONT_BASE_MUL;
extern int ATLAS_PAD_PX;
extern int CARD_REAR_THEME_IDX;

void CFG_Load(void);
void CFG_Save(void);

#endif // CONFIG_H_