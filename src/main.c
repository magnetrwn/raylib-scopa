#include <stdio.h>
#include "raylib.h"
#include "random.h"
#include "graphics.h"

#define WINDOW_W 1152
#define WINDOW_H 864
#define CARD_W 112
#define CARD_H 192

#define CARDS_DEMO_N 64

static RenderTexture2D card_atlas;

int main(void) {
    RND_InitOnce();
    GFX_Init(WINDOW_W, WINDOW_H);
    GFX_BuildCardTextureAtlas(&card_atlas, CARD_W, CARD_H);

    CardInfo ci_arr[CARDS_DEMO_N];
    for (int i = 0; i < CARDS_DEMO_N; ++i) {
        ci_arr[i].c.suit = RND_Get(CARD_SUIT_HEARTS, CARD_SUIT_SPADES);
        ci_arr[i].c.rank = RND_Get(CARD_RANK_ACE, CARD_RANK_KING);
        ci_arr[i].w = CARD_W;
        ci_arr[i].h = CARD_H;
        ci_arr[i].x = RND_Get(0, WINDOW_W - CARD_W);
        ci_arr[i].y = RND_Get(0, WINDOW_H - CARD_H);
        ci_arr[i].angle_deg = (float) RND_Get(-15, +15);
        ci_arr[i].is_hover = false;
    }

    double delay_ft = GetTime();
    for (;;) {
        if (WindowShouldClose())
            break;

        for (int i = 0; i < CARDS_DEMO_N; ++i)
            GFX_DrawCard(&ci_arr[i]);

        GFX_RenderTick(&card_atlas);

        if (GetTime() - delay_ft < 1.0f)
            continue;

        printf("Frame time: %.2f ms\n", GetFrameTime() * 1000.0f);
        delay_ft = GetTime();
    }

    UnloadTexture(card_atlas.texture);
    GFX_DeInit();
    return 0;
}