#include <stdio.h>
#include "raylib.h"
#include "random.h"
#include "graphics.h"
#include "ui.h"
#include "config.h"

#define CARD_W_BIG (164)
#define CARD_H_BIG (280)
#define CARD_W_SMALL (72)
#define CARD_H_SMALL (112)

#define CARDS_DEMO_N 100
#define TABS_DEMO_N 3

int main(void) {
    CFG_Load();
    RND_InitOnce();
    GFX_Init();

    RenderTexture2D big_atlas, small_atlas;
    GFX_BuildCardTextureAtlas(&big_atlas, CARD_W_BIG, CARD_H_BIG);
    GFX_BuildCardTextureAtlas(&small_atlas, CARD_W_SMALL, CARD_H_SMALL);

    GFX_SetCardRearTheme(CARD_REAR_THEME_IDX);

    UI_Init();

    TabInfo ti_arr[TABS_DEMO_N] = {
        { "Table Details", 160, 100, 320, 200, 24, COLOR_TAB_BG, false, TAB_ROLL_UP },
        { "Statistics", 480 + (WINDOW_W - 640) / 2, 100, WINDOW_W - 320, 200, 24, COLOR_TAB_BG, false, TAB_ROLL_UP },
        { "Chances", WINDOW_W / 2, WINDOW_H - 100, WINDOW_W, 200, 24, COLOR_TAB_BG, false, TAB_ROLL_DOWN }
    };

    CardInfo ci_arr[CARDS_DEMO_N];
    for (int i = 0; i < CARDS_DEMO_N / 2; ++i) {
        ci_arr[i].atlas = &big_atlas;
        ci_arr[i].c.suit = (i / CARD_RANKS) % CARD_SUITS;
        ci_arr[i].c.rank = 1 + (i % CARD_RANKS);
        ci_arr[i].w = CARD_W_BIG;
        ci_arr[i].h = CARD_H_BIG;
        ci_arr[i].x = RND_Get(CARD_W_BIG, WINDOW_W - CARD_W_BIG);
        ci_arr[i].y = RND_Get(CARD_H_BIG, WINDOW_H - CARD_H_BIG);
        ci_arr[i].angle_deg = RND_Get(-3, +3) * 5.0f;
        ci_arr[i].is_flipped = !RND_Get(0, 2);
        ci_arr[i].is_active = true;
        ci_arr[i].tint = WHITE;
    }

    for (int i = CARDS_DEMO_N / 2; i < CARDS_DEMO_N; ++i) {
        ci_arr[i].atlas = &small_atlas;
        ci_arr[i].c.suit = (i / CARD_RANKS) % CARD_SUITS;
        ci_arr[i].c.rank = 1 + (i % CARD_RANKS);
        ci_arr[i].w = CARD_W_SMALL;
        ci_arr[i].h = CARD_H_SMALL;
        ci_arr[i].x = RND_Get(CARD_W_SMALL, WINDOW_W - CARD_W_SMALL);
        ci_arr[i].y = RND_Get(CARD_H_SMALL, WINDOW_H - CARD_H_SMALL);
        ci_arr[i].angle_deg = RND_Get(-3, +3) * 5.0f;
        ci_arr[i].is_flipped = !RND_Get(0, 2);
        ci_arr[i].is_active = true;
        ci_arr[i].tint = WHITE;
    }

    CardInfo ci_stats_arr[CARDS_N];
    for (int s = 0; s < CARD_SUITS; ++s)
        for (int r = 1; r <= CARD_RANKS; ++r) {
            int i = s * CARD_RANKS + (r - 1);
            ci_stats_arr[i].atlas = &small_atlas;
            ci_stats_arr[i].c.suit = s;
            ci_stats_arr[i].c.rank = r;
            ci_stats_arr[i].w = CARD_W_SMALL;
            ci_stats_arr[i].h = CARD_H_SMALL;
            ci_stats_arr[i].x = ti_arr[1].x - ti_arr[1].w / 2 + 48 + (r - 1) * (CARD_W_SMALL - 50) + s * 10 * (CARD_W_SMALL - 50) + s * 60;
            ci_stats_arr[i].y = ti_arr[1].y;
            ci_stats_arr[i].angle_deg = 0.0f;
            ci_stats_arr[i].is_flipped = true;
            ci_stats_arr[i].is_active = false;
            ci_stats_arr[i].tint = WHITE;
        }

    IfElement ie_card_arr[CARDS_DEMO_N];
    for (int i = 0; i < CARDS_DEMO_N; ++i)
        UI_IfCreateCard(&ie_card_arr[i], &ci_arr[i]);

    IfElement ie_tab_arr[TABS_DEMO_N];
    for (int i = 0; i < TABS_DEMO_N; ++i)
        UI_IfCreateTab(&ie_tab_arr[i], &ti_arr[i]);

    for (;;) {
        if (WindowShouldClose())
            break;

        if (IsKeyPressed(KEY_T)) {
            int new_theme = (CARD_REAR_THEME_IDX + 1) % COLOR_THEMES_N;
            CARD_REAR_THEME_IDX = new_theme;
            GFX_SetCardRearTheme(new_theme);
            //for (int i = 0; i < TABS_DEMO_N; ++i)
            //    ti_arr[i].is_open = !ti_arr[i].is_open;
        }

        UI_IfPlaceN(ie_card_arr, CARDS_DEMO_N);
        UI_IfPlaceN(ie_tab_arr, TABS_DEMO_N);

        IfTickInputs inputs = {
            .mouse_pos = GetMousePosition(),
            .mouse_click_l = IsMouseButtonPressed(MOUSE_BUTTON_LEFT),
            .mouse_down_l = IsMouseButtonDown(MOUSE_BUTTON_LEFT)
        };
        UI_IfTick(&inputs);

        IfEvtIdx e;
        while ((e = UI_EvtPop()).evt != IF_EVT_NONE) {
            if (e.idx < CARDS_DEMO_N && ci_arr[e.idx].is_active)
                switch (e.evt) {
                    case IF_EVT_CLICK:
                        ci_arr[e.idx].tint = COLOR_CLICK;
                        ci_arr[e.idx].is_flipped = !ci_arr[e.idx].is_flipped;
                        goto event_done;
                    case IF_EVT_HOVER:
                        ci_arr[e.idx].tint = COLOR_HOVER;
                        goto event_done;
                }
            else if (e.idx - CARDS_DEMO_N < TABS_DEMO_N)
                switch (e.evt) {
                    case IF_EVT_CLICK:
                        ti_arr[e.idx - CARDS_DEMO_N].tint = COLOR_CLICK;
                        ti_arr[e.idx - CARDS_DEMO_N].is_open = !ti_arr[e.idx - CARDS_DEMO_N].is_open;
                        goto event_done;
                    case IF_EVT_HOVER:
                        goto event_done;
                }
        }
        event_done:
        UI_IfClear();

        GFX_DrawCardN(ci_arr, CARDS_DEMO_N);
        GFX_DrawTabN(ti_arr, TABS_DEMO_N);

        if (ti_arr[1].is_open)
            GFX_DrawCardN(ci_stats_arr, CARDS_N);

        GFX_RenderTick();

        for (int i = 0; i < CARDS_DEMO_N; ++i)
            ci_arr[i].tint = WHITE;
        for (int i = 0; i < TABS_DEMO_N; ++i)
            ti_arr[i].tint = COLOR_TAB_BG;
    }
    
    UnloadTexture(big_atlas.texture);
    UnloadTexture(small_atlas.texture); // TODO fit into rest of graphics interface
    GFX_DeInit();
    return 0;
}