#include <stdio.h>
#include "raylib.h"
#include "random.h"
#include "graphics.h"

#define WINDOW_W 1024
#define WINDOW_H 768
#define CARD_W 120
#define CARD_H 210

int main(void) {
    RND_InitOnce();
    printf("Hello, World!\n");

    GFX_Init(WINDOW_W, WINDOW_H);
    CardInfo ci_arr[8] = { 
        {{ CARD_SUIT_HEARTS, CARD_RANK_ACE }, { 0, 0, CARD_W, CARD_H, 0.0f }},
        {{ CARD_SUIT_HEARTS, CARD_RANK_ACE }, { 0, 0, CARD_W, CARD_H, 0.0f }},
        {{ CARD_SUIT_HEARTS, CARD_RANK_ACE }, { 0, 0, CARD_W, CARD_H, 0.0f }},
        {{ CARD_SUIT_HEARTS, CARD_RANK_ACE }, { 0, 0, CARD_W, CARD_H, 0.0f }},
        {{ CARD_SUIT_HEARTS, CARD_RANK_ACE }, { 0, 0, CARD_W, CARD_H, 0.0f }},
        {{ CARD_SUIT_HEARTS, CARD_RANK_ACE }, { 0, 0, CARD_W, CARD_H, 0.0f }},
        {{ CARD_SUIT_HEARTS, CARD_RANK_ACE }, { 0, 0, CARD_W, CARD_H, 0.0f }},
        {{ CARD_SUIT_HEARTS, CARD_RANK_ACE }, { 0, 0, CARD_W, CARD_H, 0.0f }},
    };

    float angv[8] = { 0.3f, -0.2f, 0.5f, -0.4f, 0.1f, -0.3f, 0.4f, -0.1f };
    float xv[8] = { 0.5f, -0.4f, 0.3f, -0.2f, 0.1f, -0.1f, 0.4f, -0.3f };
    float yv[8] = { 0.3f, -0.2f, 0.5f, -0.4f, -0.1f, -0.3f, 0.2f, -0.1f };

    for (int i = 0; i < 8; ++i) {
        ci_arr[i].c.suit = RND_Get(0, 3);
        ci_arr[i].c.rank = RND_Get(1, 10);
        ci_arr[i].cri.x = RND_Get(100, WINDOW_W - 100);
        ci_arr[i].cri.y = RND_Get(100, WINDOW_H - 100);
        ci_arr[i].cri.angle_deg = RND_Get(0, 180);
    }

    while (1) {
        if (WindowShouldClose())
            break;

        for (int i = 0; i < 8; ++i) {
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