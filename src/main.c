#include <stdio.h>
#include "raylib.h"
#include "random.h"
#include "graphics.h"
#include "ui.h"
#include "config.h"

#define CARD_W 112
#define CARD_H 192

#define CARDS_DEMO_N 40

static RenderTexture2D card_atlas;

int main(void) {
    CFG_Load();
    RND_InitOnce();
    GFX_Init();
    GFX_BuildCardTextureAtlas(&card_atlas, CARD_W, CARD_H);
    GFX_SetCardRearTheme(CARD_REAR_THEME_IDX);
    UI_Init();

    CardInfo ci_arr[CARDS_DEMO_N];
    for (int i = 0; i < CARDS_DEMO_N; ++i) {
        ci_arr[i].c.suit = (i / CARD_RANKS) % CARD_SUITS;
        ci_arr[i].c.rank = 1 + (i % CARD_RANKS);
        ci_arr[i].w = CARD_W;
        ci_arr[i].h = CARD_H;
        ci_arr[i].x = RND_Get(CARD_W, WINDOW_W - CARD_W);
        ci_arr[i].y = RND_Get(CARD_H, WINDOW_H - CARD_H);
        ci_arr[i].angle_deg = (float) RND_Get(-15, +15);
        ci_arr[i].is_flipped = !RND_Get(0, 2);
        ci_arr[i].tint = WHITE;
    }

    IfElement ie_arr[CARDS_DEMO_N]; // TODO make another array of these for other UI elements
    for (int i = 0; i < CARDS_DEMO_N; ++i)
        UI_IfCreateCard(&ie_arr[i], &ci_arr[i]);

    for (;;) {
        if (WindowShouldClose())
            break;

        if (IsKeyPressed(KEY_T)) {
            int new_theme = (CARD_REAR_THEME_IDX + 1) % COLOR_THEMES_N;
            CARD_REAR_THEME_IDX = new_theme;
            GFX_SetCardRearTheme(new_theme);
        }

        UI_IfPlaceN(ie_arr, CARDS_DEMO_N);
        UI_IfTick(GetMousePosition(), IsMouseButtonPressed(MOUSE_BUTTON_LEFT));

        IfEvtIdx e;
        while ((e = UI_EvtPop()).evt != IF_EVT_END) {
            if (e.evt == IF_EVT_CLICK) {
                ci_arr[e.idx].tint = COLOR_CLICK;
                ci_arr[e.idx].is_flipped = !ci_arr[e.idx].is_flipped;
                break;
            } else if (e.evt == IF_EVT_HOVER) {
                ci_arr[e.idx].tint = COLOR_HOVER;
                break;
            } else
                ci_arr[e.idx].tint = WHITE;
        }

        GFX_CardDrawN(ci_arr, CARDS_DEMO_N);
        GFX_RenderTick(&card_atlas);
    }

    UnloadTexture(card_atlas.texture);
    GFX_DeInit();
    return 0;
}