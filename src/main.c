#include <stdio.h>
#include "raylib.h"
#include "random.h"
#include "graphics.h"

#define WINDOW_W 1024
#define WINDOW_H 768
#define CARD_W 128
#define CARD_H 208

#define CARDS_DEMO_N 64

int main(void) {
    RND_InitOnce();
    printf("Hello, World!\n");

    GFX_Init(WINDOW_W, WINDOW_H);
    CardInfo ci_arr[64] = {};

    float angv[64];
    float xv[64];
    float yv[64];

    for (int i = 0; i < CARDS_DEMO_N; ++i) {
        ci_arr[i].c.suit = RND_Get(0, 3);
        ci_arr[i].c.rank = RND_Get(1, 10);
        ci_arr[i].cri.x = RND_Get(100, WINDOW_W - 100);
        ci_arr[i].cri.y = RND_Get(100, WINDOW_H - 100);
        int scale_mul = RND_Get(2, 4);
        int scale_div = RND_Get(5, 6);
        ci_arr[i].cri.w = CARD_W * scale_mul / scale_div;
        ci_arr[i].cri.h = CARD_H * scale_mul / scale_div;
        ci_arr[i].cri.angle_deg = RND_Get(0, 180);

        angv[i] = (float) RND_Get(-10, 10) * 0.05f;
        xv[i] = (float) RND_Get(-10, 10) * 0.05f;
        yv[i] = (float) RND_Get(-10, 10) * 0.05f;
    }

    while (1) {
        if (WindowShouldClose())
            break;

        for (int i = 0; i < CARDS_DEMO_N; ++i) {
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                Vector2 mp = GetMousePosition();
                Rectangle mr = { mp.x - 20, mp.y - 20, 40, 40 };
                Rectangle cr = { ci_arr[i].cri.x - ci_arr[i].cri.w / 2, ci_arr[i].cri.y - ci_arr[i].cri.h / 2, ci_arr[i].cri.w, ci_arr[i].cri.h };
                if (CheckCollisionRecs(mr, cr)) {
                    GFX_DrawCard(&ci_arr[i]);
                    continue;
                }
            }

            ci_arr[i].cri.angle_deg += angv[i];
            if (ci_arr[i].cri.angle_deg >= 360.0f) 
                ci_arr[i].cri.angle_deg -= 360.0f;
            if (ci_arr[i].cri.angle_deg < 0.0f) 
                ci_arr[i].cri.angle_deg += 360.0f;

            ci_arr[i].cri.x += xv[i];
            if (ci_arr[i].cri.x < CARD_W / 2)
                xv[i] = -xv[i]; 
            if (ci_arr[i].cri.x > WINDOW_W - CARD_W / 2)
                xv[i] = -xv[i]; 

            ci_arr[i].cri.y += yv[i];
            if (ci_arr[i].cri.y < CARD_H / 2)
                yv[i] = -yv[i]; 
            if (ci_arr[i].cri.y > WINDOW_H - CARD_H / 2)
                yv[i] = -yv[i]; 

            GFX_DrawCard(&ci_arr[i]);
        }

        GFX_Tick();
    }

    GFX_DeInit();
    return 0;
}