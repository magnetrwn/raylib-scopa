#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "card.h"
#define MAX_CARDS_IN_TICK    128   // max cards that can be rendered per tick
#define MAX_SYMBOLS_PER_CARD 16    // max symbols (characters) that can be drawn per card, minus 1 for terminator
#define SYMBOL_W_DIV_BY      28.0f // divide the card width by this many units and use the symbol layouts over the generated grid
#define SYMBOL_H_DIV_BY      54.0f // divide the card width by this many units and use the symbol layouts over the generated grid
#define FONT_BASE_MUL        8.0f  // base font size multiplier before scaling
#define BORDER_BASE_MUL      0.8f  // base border position multiplier before scaling (to make it a bit inset)

#define COLOR_HEARTS   (Color) { 220, 20, 60, 255 }
#define COLOR_DIAMONDS (Color) { 255, 140, 0, 255 }
#define COLOR_CLUBS    (Color) { 0, 0, 139, 255 }
#define COLOR_SPADES   (Color) { 0, 0, 0, 255 }

typedef struct _CardRenderInfo {
    float x, y;
    int w, h;
    float angle_deg;
} CardRenderInfo;

typedef struct _CardInfo {
    Card c;
    CardRenderInfo cri;
} CardInfo; // you should use this to keep track of cards instead of just Card[] alone

void GFX_Init(int w, int h);
void GFX_DrawCard(const CardInfo* ci);
void GFX_Tick(void);
void GFX_DeInit(void);

#endif // GRAPHICS_H_