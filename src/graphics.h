#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "card.h"
#define MAX_CARDS_IN_TICK 128

typedef struct _CardRenderInfo {
    int x, y, w, h;
    float angle_deg;
} CardRenderInfo;

void GFX_Init(void);
void GFX_DrawCard(const Card* c, const CardRenderInfo* cri);
void GFX_Tick(void);

#endif // GRAPHICS_H_