#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <stdbool.h>
#include "raylib.h"
#include "card.h"
#define MAX_GFX_EL_IN_TICK   384   // max graphics elements (currently cards+tabs) that can be rendered per tick
#define MAX_SYMBOLS_PER_CARD 16    // max symbols (characters) that can be drawn per card, minus 1 for terminator
#define GFX_Z_DEFAULT        0

#define COLOR_HEARTS    (Color) { 220,  20,  48, 255 }
#define COLOR_DIAMONDS  (Color) { 220,  20,  48, 255 }//(Color) { 232, 109,  20, 255 }
#define COLOR_CLUBS     (Color) {   0,   0,   0, 255 }//(Color) {   0,   0, 139, 255 }
#define COLOR_SPADES    (Color) {   0,   0,   0, 255 }

#define COLOR_BG        (Color) {  31, 105,  56, 255 }   
#define COLOR_CARD      (Color) { 255, 255, 245, 255 }  
#define COLOR_TAB_BG    (Color) {  44, 129,  64, 199 }//(Color) {  80, 180,  80, 212 }
#define COLOR_TAB_TEXT  (Color) { 240, 255, 225, 255 }
#define COLOR_HOVER     (Color) { 219, 235, 160, 255 }
#define COLOR_CLICK     (Color) { 186, 255,  66, 255 }

#define COLOR_THEMES_N  (sizeof(COLOR_THEMES) / sizeof(Color))
static const Color COLOR_THEMES[] = { 
    (Color) {   0,  69, 183, 255 }, 
    (Color) { 170, 220, 200, 255 },
    (Color) {   0,  10,  28, 255 }, 
    (Color) { 150,   0,  13, 255 }, 
    (Color) { 200, 178,  10, 255 }
};

typedef struct {
    Card c;
    float x, y, w, h, angle_deg;
    const RenderTexture2D* atlas;
    Color tint;
    bool is_flipped;
    bool is_active;
} CardInfo; // you should use this to keep track of cards instead of just Card[] alone

typedef enum {
    TAB_ROLL_UP,
    TAB_ROLL_DOWN
} TabRollDir;

typedef struct {
    const char* title;
    float x, y, w, h, rolled_h;
    Color tint;
    bool is_open;
    TabRollDir roll_dir;
} TabInfo;

typedef enum {
    GFX_DRAW_CARD,
    GFX_DRAW_TAB
} GfxElType;

void GFX_Init(void);
void GFX_DeInit(void);

void GFX_BuildCardTextureAtlas(RenderTexture2D* atlas, int card_w, int card_h);
void GFX_SetCardRearTheme(int idx);

void GFX_DrawCard(const CardInfo* ci);
void GFX_DrawTab(const TabInfo* ti);

void GFX_DrawCardZ(const CardInfo* ci, int z);
void GFX_DrawTabZ(const TabInfo* ti, int z);

void GFX_DrawCardN(const CardInfo* ci, int n);
void GFX_DrawTabN(const TabInfo* ti, int n);

void GFX_DrawCardZN(const CardInfo* ci, int z, int n);
void GFX_DrawTabZN(const TabInfo* ti, int z, int n);

void GFX_RenderTick(void);

#endif // GRAPHICS_H_