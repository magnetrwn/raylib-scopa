#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <stdbool.h>
#include "raylib.h"
#include "card.h"
#define MAX_CARDS_IN_TICK    80    // max cards that can be rendered per tick
#define MAX_SYMBOLS_PER_CARD 16    // max symbols (characters) that can be drawn per card, minus 1 for terminator

#define COLOR_HEARTS    (Color) { 220,  20,  60, 255 }
#define COLOR_DIAMONDS  (Color) { 232, 109,  20, 255 }
#define COLOR_CLUBS     (Color) {   0,   0, 139, 255 }
#define COLOR_SPADES    (Color) {   0,   0,   0, 255 }

#define COLOR_BG        (Color) {  31, 105,  56, 255 }   
#define COLOR_CARD      (Color) { 255, 255, 245, 255 }  
#define COLOR_HOVER     (Color) { 219, 235, 160, 255 }
#define COLOR_CLICK     (Color) { 186, 255,  66, 255 }

#define COLOR_THEMES_N (sizeof(COLOR_THEMES)/4)
static const Color COLOR_THEMES[] = { 
    (Color) {   0,  10,  28, 255 }, 
    (Color) { 150,   0,  13, 255 }, 
    (Color) { 200, 178,  10, 255 }, 
    (Color) {   0,  69, 183, 255 }, 
    (Color) { 170, 220, 200, 255 } 
};

typedef struct _CardInfo {
    Card c;
    float x, y, w, h, angle_deg;
    Color tint;
    bool is_flipped;
} CardInfo; // you should use this to keep track of cards instead of just Card[] alone

// typedef enum _DetailType {
//     PLAYER_DECK,
//     PLAYER_DECK_DETAILS,
//     TABLE,
//     TABLE_DETAILS,
//     ROUND_INDICATOR,
// } DetailType;

typedef struct _TabInfo {
    const char* title;
    float x, y, w, h;
    bool is_open;
} TabInfo;

void GFX_Init(void);
void GFX_DeInit(void);

void GFX_BuildCardTextureAtlas(RenderTexture2D *out, int card_w, int card_h);
void GFX_SetCardRearTheme(int idx);

void GFX_CardDraw(const CardInfo* ci);
void GFX_CardDrawN(const CardInfo* ci, int n);

void GFX_RenderTick(const RenderTexture2D* atlas);

#endif // GRAPHICS_H_