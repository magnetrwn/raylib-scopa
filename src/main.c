#include <stdio.h>
#include "raylib.h"
#include "random.h"
#include "graphics.h"

int main(void) {
    RND_InitOnce();
    printf("Hello, World!\n");

    GFX_Init();
    int x = 10, y = 20, a = 75.0f;
    while (1) {
        if (WindowShouldClose())
            break;

        GFX_DrawCard(&(Card) {CARD_SUIT_HEARTS, CARD_RANK_ACE}, &(CardRenderInfo) {x, y, 200, 300, (float) a});
        GFX_Tick();

        ++x; ++y; ++a;
        x %= 800;
        y %= 600;
        a %= 360;
    }
    return 0;
}