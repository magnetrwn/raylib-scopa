#include "graphics.h"
#include <raylib.h>
#include <stdio.h>
#include "card.h"

typedef struct _CardInfo {
    Card c;
    CardRenderInfo cri;
} CardInfo;

static CardInfo renderArr[MAX_CARDS_IN_TICK];
static int renderIdx;

void GFX_Init(void) {
    InitWindow(800, 600, "GameWindow");
    SetTargetFPS(60);
    renderIdx = 0;
}

void GFX_DrawCard(const Card* c, const CardRenderInfo* cri) {
    if (c == NULL || cri == NULL || CARD_IsError(c) || renderIdx >= MAX_CARDS_IN_TICK)
        return;
    renderArr[renderIdx++] = (CardInfo) { *c, *cri };
}

void GFX_Tick(void) {
    BeginDrawing();
    ClearBackground(BLACK);
    
    for (int i = 0; i < renderIdx; ++i) {
        CardInfo* info = &renderArr[i];
        DrawRectanglePro(
            (Rectangle) {info->cri.x, info->cri.y, info->cri.w, info->cri.h}, 
            (Vector2) {info->cri.w / 2, info->cri.h / 2}, 
            info->cri.angle_deg,
            WHITE
        );
        // TODO put actual card drawing logic here
        printf("Drawing Card: Suit=%d, Rank=%d at (%d, %d)\n", 
               info->c.suit, info->c.rank, info->cri.x, info->cri.y);
    }

    EndDrawing();
    renderIdx = 0;
}