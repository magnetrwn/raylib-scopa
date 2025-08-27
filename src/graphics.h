#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <stdbool.h>
#include "raylib.h"
#include "card.h"
#define MAX_CARDS_IN_TICK    80    // max cards that can be rendered per tick
#define MAX_SYMBOLS_PER_CARD 16    // max symbols (characters) that can be drawn per card, minus 1 for terminator

#define COLOR_HEARTS   (Color) { 220, 20, 60, 255 }
#define COLOR_DIAMONDS /*(Color) { 232, 109, 20, 255 }*/(Color) { 220, 20, 60, 255 }
#define COLOR_CLUBS    /*(Color) { 0, 0, 139, 255 }*/(Color) { 0, 0, 0, 255 }
#define COLOR_SPADES   (Color) { 0, 0, 0, 255 }

#define COLOR_BG       (Color) { 31, 105, 56, 255 }
#define COLOR_CARD     (Color) { 255, 255, 245, 255 }
#define COLOR_THEME1   (Color) { 0, 10, 28, 255 }
#define COLOR_THEME2   (Color) { 100, 0, 13, 255 }
#define COLOR_THEME3   (Color) { 200, 178, 10, 255 }
#define COLOR_THEME4   (Color) { 73, 40, 183, 255 }
#define COLOR_THEME5   (Color) { 170, 220, 200, 255 }
static const Color COLOR_THEMES[] = { COLOR_THEME1, COLOR_THEME2, COLOR_THEME3, COLOR_THEME4, COLOR_THEME5 }; // you can add more here!
#define COLOR_THEMES_N (sizeof(COLOR_THEMES) / 4)

#define COLOR_HOVER    (Color) { 219, 235, 160, 255 }/*(Color) { 91, 211, 204, 255 }*/
#define COLOR_CLICK    (Color) { 186, 255, 66, 255 }

typedef struct _CardInfo {
    Card c;
    float x, y;
    int w, h;
    float angle_deg;
    Color tint;
    bool is_flipped;
} CardInfo; // you should use this to keep track of cards instead of just Card[] alone

void GFX_Init(int w, int h);
void GFX_BuildCardTextureAtlas(RenderTexture2D *out, int card_w, int card_h);
void GFX_DrawCard(const CardInfo* ci);
void GFX_DrawCardN(const CardInfo* ci, int n);
void GFX_RenderTick(const RenderTexture2D* atlas);
void GFX_DeInit(void);

#endif // GRAPHICS_H_