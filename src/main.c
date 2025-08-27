#include <stdio.h>
#include "raylib.h"
#include "random.h"
#include "graphics.h"
#include "ui.h"

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
    UI_Init();

    CardInfo ci_arr[CARDS_DEMO_N];
    for (int i = 0; i < CARDS_DEMO_N; ++i) {
        ci_arr[i].c.suit = RND_Get(CARD_SUIT_HEARTS, CARD_SUIT_SPADES);
        ci_arr[i].c.rank = RND_Get(CARD_RANK_ACE, CARD_RANK_KING);
        ci_arr[i].w = CARD_W;
        ci_arr[i].h = CARD_H;
        ci_arr[i].x = RND_Get(0, WINDOW_W - CARD_W);
        ci_arr[i].y = RND_Get(0, WINDOW_H - CARD_H);
        ci_arr[i].angle_deg = (float) RND_Get(-15, +15);
        ci_arr[i].tint = WHITE;
    }

    IfElement ie_arr[CARDS_DEMO_N];
    for (int i = 0; i < CARDS_DEMO_N; ++i)
        UI_IfCreateCard(&ie_arr[i], &ci_arr[i]);

    double delay_ft = GetTime();
    for (;;) {
        if (WindowShouldClose())
            break;

        UI_IfPlaceN(ie_arr, CARDS_DEMO_N);
        UI_IfTick(GetMousePosition(), IsMouseButtonDown(MOUSE_BUTTON_LEFT));

        IfEvtIdx e;
        while ((e = UI_EvtPop()).evt != IF_EVT_NONE) {
            if (e.evt == IF_EVT_CLICK) {
                ci_arr[e.idx].tint = COLOR_CLICK;
                ci_arr[e.idx].is_flipped = !ci_arr[e.idx].is_flipped;
            } else if (e.evt == IF_EVT_HOVER) {
                ci_arr[e.idx].tint = COLOR_HOVER;
            } else {
                ci_arr[e.idx].tint = WHITE;
            }
        }

        GFX_DrawCardN(ci_arr, CARDS_DEMO_N);
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