#include <stdio.h>
#include "raylib.h"
#include "random.h"
#include "graphics.h"
#include "ui.h"
#include "config.h"

#define CARD_W (WINDOW_W / 16 + 64)
#define CARD_H (WINDOW_W / 8 + 84)

#define CARDS_DEMO_N 40
#define TABS_DEMO_N 3

int main(void) {
    CFG_Load();
    RND_InitOnce();
    GFX_Init();

    RenderTexture2D big_atlas, small_atlas;
    GFX_BuildCardTextureAtlas(&big_atlas, CARD_W, CARD_H);
    GFX_BuildCardTextureAtlas(&small_atlas, CARD_W / 2, CARD_H / 2);

    GFX_SetCardRearTheme(CARD_REAR_THEME_IDX);
    UI_Init();

    CardInfo ci_arr[CARDS_DEMO_N];
    for (int i = 0; i < CARDS_DEMO_N; ++i) {
        ci_arr[i].atlas = &big_atlas;
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

    TabInfo ti_arr[TABS_DEMO_N] = {
        { "Table Details", 160, 100, 320, 200, 24, COLOR_TAB_BG, false, TAB_ROLL_UP },
        { "Statistics", WINDOW_W - 160, 100, 320, 200, 24, COLOR_TAB_BG, false, TAB_ROLL_UP },
        { "Chances", WINDOW_W - 160, WINDOW_H - 100, 320, 200, 24, COLOR_TAB_BG, false, TAB_ROLL_DOWN }
    };

    IfElement ie_arr[CARDS_DEMO_N]; // TODO make another array of these for other UI elements
    for (int i = 0; i < CARDS_DEMO_N; ++i)
        UI_IfCreateCard(&ie_arr[i], &ci_arr[i]); // TODO later on, create UI only after animations are done, while animations interact directly with GFX_ functions

    IfElement tab_arr[TABS_DEMO_N];
    for (int i = 0; i < TABS_DEMO_N; ++i)
        UI_IfCreateTab(&tab_arr[i], &ti_arr[i]);

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

        UI_IfPlaceN(ie_arr, CARDS_DEMO_N);
        UI_IfPlaceN(tab_arr, TABS_DEMO_N);

        IfTickInputs inputs = {
            .mouse_pos = GetMousePosition(),
            .mouse_click_l = IsMouseButtonPressed(MOUSE_BUTTON_LEFT),
            .mouse_down_l = IsMouseButtonDown(MOUSE_BUTTON_LEFT)
        };
        UI_IfTick(&inputs);

        IfEvtIdx e;
        while ((e = UI_EvtPop()).evt != IF_EVT_NONE) {
            if (e.idx < CARDS_DEMO_N)
                switch (e.evt) {
                    case IF_EVT_CLICK:
                        ci_arr[e.idx].tint = COLOR_CLICK;
                        // ci_arr[e.idx].is_flipped = !ci_arr[e.idx].is_flipped;
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
                        ti_arr[e.idx - CARDS_DEMO_N].tint = COLOR_HOVER;
                        goto event_done;
                }
        }
        event_done:
        UI_IfClear();

        GFX_CardDrawN(ci_arr, CARDS_DEMO_N);
        GFX_TabDrawN(ti_arr, TABS_DEMO_N);
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