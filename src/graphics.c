#include "graphics.h"
#include "raylib.h"
#include <stdio.h>
#include "card.h"
#include <string.h>
#include "util.h"
#include "config.h"

#define SYMBOL_W_DIV_BY      28.0f // divide the card width by this many units and use the symbol layouts over the generated grid
#define SYMBOL_H_DIV_BY      54.0f // divide the card width by this many units and use the symbol layouts over the generated grid
#define BORDER_BASE_MUL      0.8f  // base border position multiplier before scaling (to make it a bit inset)
#define BORDER_BASE_BACK_MUL 0.875f// base border position multiplier before scaling (to make it a bit inset)
#define ROUND_CORNER_PX      (ATLAS_UPSCALE_MUL*8.0f) // radius of the rounded corners of the card

#define GFX_TOP_LEFT_SYMBOL { -11, -16, SYMBOL_SUIT }, { -11, -23, SYMBOL_RANK } // put the suit first to make it easier for face drawing
#define GFX_END_SYMBOL      { 0, 0, SYMBOL_END }

// types related to card symbol layout and listing
typedef enum _CardSymbolType { 
    SYMBOL_END, SYMBOL_RANK, SYMBOL_SUIT
} CardSymbolType;

typedef struct _CardSymbol {
    int rx, ry; // position of each symbol to draw based on the grid produced over the width and height using SYMBOL_x_DIV_BY
    CardSymbolType type;
} CardSymbol;

typedef struct _CardSymbolInfo {
    CardSymbol syms[MAX_SYMBOLS_PER_CARD];
} CardSymbolInfo;

// layouts of the card symbols (rank and suit)
static const CardSymbolInfo CARD_SYMBOLS[] = {
    { .syms = { GFX_END_SYMBOL } }, // [0] unused
    { .syms = { GFX_TOP_LEFT_SYMBOL, { 0, 0,  SYMBOL_SUIT }, GFX_END_SYMBOL }}, 
    { .syms = { GFX_TOP_LEFT_SYMBOL, { 0, -15, SYMBOL_SUIT }, { 0, 15, SYMBOL_SUIT }, GFX_END_SYMBOL }},
    { .syms = { GFX_TOP_LEFT_SYMBOL, { 0, -15, SYMBOL_SUIT }, { 0, 0, SYMBOL_SUIT }, { 0, 15, SYMBOL_SUIT }, GFX_END_SYMBOL }},
    { .syms = { GFX_TOP_LEFT_SYMBOL, { -5, -12, SYMBOL_SUIT }, { 5, -12, SYMBOL_SUIT }, { -5, 12, SYMBOL_SUIT }, { 5, 12, SYMBOL_SUIT }, GFX_END_SYMBOL }},
    { .syms = { GFX_TOP_LEFT_SYMBOL, { -5, -12, SYMBOL_SUIT }, { 5, -12, SYMBOL_SUIT }, { 0, 0,  SYMBOL_SUIT }, { -5, 12, SYMBOL_SUIT }, { 5, 12, SYMBOL_SUIT }, GFX_END_SYMBOL }},
    { .syms = { GFX_TOP_LEFT_SYMBOL, { -5, -15, SYMBOL_SUIT }, { 5, -15, SYMBOL_SUIT }, { -5, 0,  SYMBOL_SUIT }, { 5, 0,  SYMBOL_SUIT }, { -5, 15, SYMBOL_SUIT }, { 5, 15, SYMBOL_SUIT }, GFX_END_SYMBOL }},
    { .syms = { GFX_TOP_LEFT_SYMBOL, { -5, -15, SYMBOL_SUIT }, { 5, -15, SYMBOL_SUIT }, { -5, 0,  SYMBOL_SUIT }, { 5, 0,  SYMBOL_SUIT }, { -5, 15, SYMBOL_SUIT }, { 5, 15, SYMBOL_SUIT }, { 0, -10, SYMBOL_SUIT }, GFX_END_SYMBOL }},
    { .syms = { GFX_TOP_LEFT_SYMBOL, GFX_END_SYMBOL }},
    { .syms = { GFX_TOP_LEFT_SYMBOL, GFX_END_SYMBOL }},
    { .syms = { GFX_TOP_LEFT_SYMBOL, GFX_END_SYMBOL }},
};

typedef struct _GfxEl {
    GfxElType type;
    union { const CardInfo* card; const TabInfo*  tab; };
    int z;
} GfxEl;

#define FIGURE_ATLAS_PAD_PX 2.0f
const Rectangle FIGURE_ATLAS_SRC[3] = {
    { FIGURE_ATLAS_PAD_PX, 0.0f, 280.0f, 480.0f }, // j
    { 280.0f + FIGURE_ATLAS_PAD_PX*2, 0.0f, 280.0f, 480.0f }, // q
    { 560.0f + FIGURE_ATLAS_PAD_PX*3, 0.0f, 280.0f, 480.0f }, // k
};

static Font sym_font;
static Font text_font;
static Texture2D figure_atlas;
static GfxEl render_arr[MAX_GFX_EL_IN_TICK];
static int render_idx;
static int theme_idx;

static void _DrawCardBase(const CardInfo* ci, Color color) {
    DrawRectanglePro(
        (Rectangle) { ci->x + ROUND_CORNER_PX, ci->y, ci->w - ROUND_CORNER_PX * 2, ci->h },
        (Vector2) { ci->w * 0.5f, ci->h * 0.5f }, 
        0.0f, 
        color
    );
    DrawRectanglePro(
        (Rectangle) { ci->x, ci->y + ROUND_CORNER_PX, ci->w, ci->h - ROUND_CORNER_PX * 2 },
        (Vector2) { ci->w * 0.5f, ci->h * 0.5f }, 
        0.0f, 
        color
    );
    DrawCircle(ci->x - ci->w * 0.5f + ROUND_CORNER_PX, ci->y - ci->h * 0.5f + ROUND_CORNER_PX, ROUND_CORNER_PX, color);
    DrawCircle(ci->x + ci->w * 0.5f - ROUND_CORNER_PX, ci->y - ci->h * 0.5f + ROUND_CORNER_PX, ROUND_CORNER_PX, color);
    DrawCircle(ci->x - ci->w * 0.5f + ROUND_CORNER_PX, ci->y + ci->h * 0.5f - ROUND_CORNER_PX, ROUND_CORNER_PX, color);
    DrawCircle(ci->x + ci->w * 0.5f - ROUND_CORNER_PX, ci->y + ci->h * 0.5f - ROUND_CORNER_PX, ROUND_CORNER_PX, color);
}

static void _DrawCardFace(const CardInfo* ci) {
    const float font_size = FONT_BASE_MUL * ci->h / SYMBOL_H_DIV_BY;
    Color sym_color = BLANK; // will keep the very last suit's color used for drawing the border

    Vector2 sz_rank = MeasureTextEx(sym_font, "2", font_size, 1.0f); // just take one rank and one suit char to get the approx size of the font symbols
    Vector2 sz_suit = MeasureTextEx(sym_font, "[", font_size, 1.0f);
    Vector2 origin_rank = (Vector2){ sz_rank.x * 0.5f, sz_rank.y * 0.5f };
    Vector2 origin_suit = (Vector2){ sz_suit.x * 0.5f, sz_suit.y * 0.5f };

    for (int i = 0; i < MAX_SYMBOLS_PER_CARD; ++i) {
        const CardSymbol* sym = &CARD_SYMBOLS[ci->c.rank].syms[i];

        if (sym->type == SYMBOL_END) 
            break;

        const char* to_draw = "?";

        if (sym->type == SYMBOL_RANK) {
            switch (ci->c.rank) {
                case CARD_RANK_ACE:   to_draw = "A"; break;
                case CARD_RANK_TWO:   to_draw = "2"; break;
                case CARD_RANK_THREE: to_draw = "3"; break;
                case CARD_RANK_FOUR:  to_draw = "4"; break;
                case CARD_RANK_FIVE:  to_draw = "5"; break;
                case CARD_RANK_SIX:   to_draw = "6"; break;
                case CARD_RANK_SEVEN: to_draw = "7"; break;
                case CARD_RANK_JACK:  to_draw = "J"; break;
                case CARD_RANK_QUEEN: to_draw = "Q"; break;
                case CARD_RANK_KING:  to_draw = "K"; break;
                default: break;
            }
        } 
        
        else if (sym->type == SYMBOL_SUIT) {
            switch (ci->c.suit) {
                case CARD_SUIT_HEARTS:   to_draw = "{"; sym_color = COLOR_HEARTS;   break;
                case CARD_SUIT_DIAMONDS: to_draw = "["; sym_color = COLOR_DIAMONDS; break;
                case CARD_SUIT_CLUBS:    to_draw = "]"; sym_color = COLOR_CLUBS;    break;
                case CARD_SUIT_SPADES:   to_draw = "}"; sym_color = COLOR_SPADES;   break;
            }
        }

        Vector2 off = (Vector2) {
            sym->rx * ci->w / SYMBOL_W_DIV_BY,
            sym->ry * ci->h / SYMBOL_H_DIV_BY
        };

        Vector2 pos = off;
        UTIL_Rotate(&pos, ci->angle_deg);
        pos.x += ci->x; pos.y += ci->y;

        DrawTextPro(sym_font, to_draw, pos, (sym->type == SYMBOL_SUIT) ? origin_suit : origin_rank, ci->angle_deg, font_size, 1.0f, sym_color);
    }

    const Vector2 center = (Vector2){ ci->x, ci->y };
    const float w2 = ci->w * 0.5f * BORDER_BASE_MUL;
    const float h2 = ci->h * 0.5f * BORDER_BASE_MUL;
    const Vector2 off[5] = {
        { -w2 * 0.7f, -h2 }, { +w2, -h2 }, { +w2, +h2 }, { -w2, +h2 }, { -w2, -h2 * 0.5f }
    };
    Vector2 pts[5];
    for (int k = 0; k < 5; ++k) {
        Vector2 r = off[k];
        UTIL_Rotate(&r, ci->angle_deg);
        pts[k].x = center.x + r.x;
        pts[k].y = center.y + r.y;
    }
    DrawLineStrip(pts, 5, sym_color);

    if (ci->c.rank >= CARD_RANK_JACK && ci->c.rank <= CARD_RANK_KING) {
        Rectangle src = FIGURE_ATLAS_SRC[ci->c.rank - CARD_RANK_JACK];
    
        const float dst_w = src.width * w2 / src.width * 2.0f;
        const float dst_h = src.height * h2 / src.height * 2.0f;

        Rectangle dst = { center.x, center.y, dst_w, dst_h };
        Vector2 pivot = { dst_w * 0.5f, dst_h * 0.5f };

        DrawTexturePro(figure_atlas, src, dst, pivot, ci->angle_deg, sym_color);
    }
}

static inline Rectangle _CardAtlasSrcFace(int suit, int rank, int card_w, int card_h) { 
    card_w *= ATLAS_UPSCALE_MUL;
    card_h *= ATLAS_UPSCALE_MUL;

    return (Rectangle) { 
        (float) (ATLAS_PAD_PX + (rank - 1) * (card_w + ATLAS_PAD_PX)), 
        (float) (ATLAS_PAD_PX + (CARD_SUITS - suit) * (card_h + ATLAS_PAD_PX)), 
        (float) card_w, 
        -(float) card_h
    }; 
}

static inline Rectangle _CardAtlasSrcBack(int theme, int card_w, int card_h) { 
    card_w *= ATLAS_UPSCALE_MUL;
    card_h *= ATLAS_UPSCALE_MUL;

    return (Rectangle) { 
        (float) (ATLAS_PAD_PX + theme * (card_w + ATLAS_PAD_PX)), 
        (float) (ATLAS_PAD_PX), 
        (float) card_w, 
        -(float) card_h
    }; 
}

static inline void _Enqueue(GfxElType type, int z, const void* ptr) {
    if (!ptr || render_idx >= MAX_GFX_EL_IN_TICK)
        return;

    GfxEl* el = &render_arr[render_idx++];
    el->type = type;
    el->z = z;

    switch (type) {
        case GFX_DRAW_CARD:
            el->card = (const CardInfo*) ptr;
            break;
        case GFX_DRAW_TAB:
            el->tab = (const TabInfo*) ptr;
            break;
    }
}

static void _StableSortByZ(GfxEl* a, int n) {
    for (int i = 1; i < n; ++i) {
        GfxEl key = a[i];
        int j = i - 1;
        while (j >= 0 && a[j].z > key.z) {
            a[j + 1] = a[j];
            --j;
        }
        a[j + 1] = key;
    }
}

// public api

void GFX_Init(void) {
    if (FULLSCREEN) 
        SetConfigFlags(FLAG_FULLSCREEN_MODE | FLAG_VSYNC_HINT);
    else
        SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(WINDOW_W, WINDOW_H, "raylib-scopa");
    SetTargetFPS(60);

    sym_font = LoadFontEx("assets/fnt/symbols.ttf", FONT_BASE_LOAD, NULL, 0);
    text_font = LoadFontEx("assets/fnt/DejaVuSans.ttf", FONT_BASE_LOAD, NULL, 0);

    Image i = LoadImage("assets/img/face-figure-atlas.png");
    figure_atlas = LoadTextureFromImage(i);
    UnloadImage(i);

    render_idx = 0;
    theme_idx = 0;
}

void GFX_BuildCardTextureAtlas(RenderTexture2D* atlas, int card_w, int card_h) {
    card_w *= ATLAS_UPSCALE_MUL;
    card_h *= ATLAS_UPSCALE_MUL;

    *atlas = LoadRenderTexture(
        CARD_RANKS * card_w + (CARD_RANKS + 1) * ATLAS_PAD_PX, 
        (CARD_SUITS + 1) * card_h + (CARD_SUITS + 2) * ATLAS_PAD_PX // add a row for back drawing
    );
    SetTextureWrap(atlas->texture, TEXTURE_WRAP_CLAMP);

    BeginTextureMode(*atlas);
    ClearBackground(BLANK);

    for (int s = 0; s < CARD_SUITS; ++s) {
        for (int r = 1; r <= CARD_RANKS; ++r) {
            CardInfo ci;
            ci.c.suit = s;
            ci.c.rank = r;
            ci.x = (ATLAS_PAD_PX + (r - 1) * (card_w + ATLAS_PAD_PX)) + card_w * 0.5f; // last part is to center cards
            ci.y = (ATLAS_PAD_PX + s * (card_h + ATLAS_PAD_PX)) + card_h * 0.5f;
            ci.w = card_w;
            ci.h = card_h;
            ci.angle_deg = 0.0f;

            _DrawCardBase(&ci, COLOR_CARD);
            _DrawCardFace(&ci);
        }
    }

    for (int theme = 0; theme < COLOR_THEMES_N; ++theme) {
        CardInfo back;
        back.x = ATLAS_PAD_PX + theme * (card_w + ATLAS_PAD_PX) + card_w * 0.5f;
        back.y = (ATLAS_PAD_PX + CARD_SUITS * (card_h + ATLAS_PAD_PX)) + card_h * 0.5f;
        back.w = card_w;
        back.h = card_h;
        back.angle_deg = 0.0f;
        _DrawCardBase(&back, COLOR_THEMES[theme]);

        back.x = ATLAS_PAD_PX + theme * (card_w + ATLAS_PAD_PX);
        back.y = (ATLAS_PAD_PX + CARD_SUITS * (card_h + ATLAS_PAD_PX));
        const Vector2 c = { back.x + back.w * 0.5f, back.y + back.h * 0.5f };
        const float rad = fminf(back.w, back.h) * 0.4f;

        const int N = 41;
        Vector2 pts[N];

        for (int i = 0; i < N; ++i) {
            float t = (2 * PI * i) / (N - 1);
            float r = rad * 0.75f * cosf(8.0f * t);
            pts[i] = (Vector2) { c.x + r * sinf(t), c.y + r * cosf(t) };
        }
        //DrawLineStrip(pts, N, COLOR_THEMES[(theme + 1) % COLOR_THEMES_N]);
        for (int i = 0; i < N; ++i)
            DrawLineEx(pts[i], pts[(i + 1) % N], 1.5f, COLOR_THEMES[(theme + 1) % COLOR_THEMES_N]);
        DrawRing(c, rad * 0.70f, rad * 0.75f, 0, 360, 128, COLOR_THEMES[(theme + 1) % COLOR_THEMES_N]);

        for (int i = 0; i < N; ++i) {
            float t = (2 * PI * i) / (N - 1);
            float r = rad * 1.0f * -cosf(5.0f * t);
            pts[i] = (Vector2) { c.x + r * sinf(t), c.y + r * cosf(t) };
        }
        //DrawLineStrip(pts, N, COLOR_THEMES[(theme + 2) % COLOR_THEMES_N]);
        for (int i = 0; i < N; ++i)
            DrawLineEx(pts[i], pts[(i + 1) % N], 2.0f, COLOR_THEMES[(theme + 2) % COLOR_THEMES_N]);
        DrawRing(c, rad * 0.45f, rad * 0.50f, 0, 360, 128, COLOR_THEMES[(theme + 2) % COLOR_THEMES_N]);
        
        float w2 = back.w * 0.5f * BORDER_BASE_BACK_MUL;
        float h2 = back.h * 0.5f * BORDER_BASE_BACK_MUL;
        const Vector2 off1[5] = {
            { -w2, -h2 }, { +w2, -h2 }, { +w2, +h2 }, { -w2, +h2 }, { -w2, -h2 }
        };
        for (int k = 0; k < 5; ++k) {
            Vector2 r = off1[k];
            pts[k].x = c.x + r.x;
            pts[k].y = c.y + r.y;
        }
        for (int i = 0; i < 5; ++i)
            DrawLineEx(pts[i], pts[(i + 1) % 5], 2.0f, COLOR_THEMES[(theme + 1) % COLOR_THEMES_N]);
        //DrawLineStrip(pts, 5, COLOR_THEMES[(theme + 1) % COLOR_THEMES_N]);

        w2 = back.w * 0.5f * BORDER_BASE_BACK_MUL + 5.0f;
        h2 = back.h * 0.5f * BORDER_BASE_BACK_MUL + 5.0f;
        const Vector2 off2[5] = {
            { -w2, -h2 }, { +w2, -h2 }, { +w2, +h2 }, { -w2, +h2 }, { -w2, -h2 }
        };
        for (int k = 0; k < 5; ++k) {
            Vector2 r = off2[k];
            pts[k].x = c.x + r.x;
            pts[k].y = c.y + r.y;
        }
        for (int i = 0; i < 5; ++i)
            DrawLineEx(pts[i], pts[(i + 1) % 5], 2.0f, COLOR_THEMES[(theme + 2) % COLOR_THEMES_N]);
        //DrawLineStrip(pts, 5, COLOR_THEMES[(theme + 2) % COLOR_THEMES_N]);

        BeginBlendMode(BLEND_ADDITIVE);
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 6; ++j)
                if ((i + j) % 2 == 0)
                    DrawRectangle(
                        c.x - (int)(back.w * 0.5f) + i * (int)(back.w / 4),
                        c.y - (int)(back.h * 0.5f) + j * (int)(back.h / 6),
                        back.w / 4,
                        back.h / 6,
                        (Color) { 255, 255, 255, 18 }
                    );
        EndBlendMode();
    }

    EndTextureMode();
    GenTextureMipmaps(&atlas->texture);
    SetTextureFilter(atlas->texture, TEXTURE_FILTER_TRILINEAR);
}

void GFX_SetCardRearTheme(int idx) {
    if (idx < 0 || idx >= COLOR_THEMES_N)
        return;
    theme_idx = idx;
}

void GFX_DrawCard(const CardInfo* ci) { 
    _Enqueue(GFX_DRAW_CARD, GFX_Z_DEFAULT, ci); 
}

void GFX_DrawTab(const TabInfo* ti) { 
    _Enqueue(GFX_DRAW_TAB, GFX_Z_DEFAULT, ti); 
}

void GFX_DrawCardZ(const CardInfo* ci, int z) { 
    _Enqueue(GFX_DRAW_CARD, z, ci); 
}

void GFX_DrawTabZ(const TabInfo* ti, int z) { 
    _Enqueue(GFX_DRAW_TAB, z, ti); 
}

void GFX_DrawCardN(const CardInfo* ci, int n) { 
    for (int i = 0; i < n; ++i) 
        GFX_DrawCard(&ci[i]); 
}

void GFX_DrawTabN(const TabInfo* ti, int n) { 
    for (int i = 0; i < n; ++i) 
        GFX_DrawTab(&ti[i]); 
}

void GFX_DrawCardZN(const CardInfo* ci, int z, int n) { 
    for (int i = 0; i < n; ++i) 
        GFX_DrawCardZ(&ci[i], z); 
}

void GFX_DrawTabZN(const TabInfo* ti, int z, int n) { 
    for (int i = 0; i < n; ++i) 
        GFX_DrawTabZ(&ti[i], z); 
}

void GFX_RenderTick(void) {
    BeginDrawing();
    ClearBackground(COLOR_BG);
    _StableSortByZ(render_arr, render_idx);

    for (int i = 0; i < render_idx; ++i) {
        switch (render_arr[i].type) {
            case GFX_DRAW_CARD: {
                const CardInfo* ci = render_arr[i].card;
                if (ci->w <= 0 || ci->h <= 0 || CARD_IsError(&ci->c))
                    continue;
                if (!ci->is_flipped)
                    DrawTexturePro(
                        ci->atlas->texture, 
                        _CardAtlasSrcFace(ci->c.suit, ci->c.rank, ci->w, ci->h), 
                        (Rectangle) { ci->x, ci->y, ci->w, ci->h }, 
                        (Vector2) { ci->w * 0.5f, ci->h * 0.5f }, 
                        ci->angle_deg, 
                        ci->tint
                    );
                else
                    DrawTexturePro(
                        ci->atlas->texture, 
                        _CardAtlasSrcBack(theme_idx, ci->w, ci->h), 
                        (Rectangle) { ci->x, ci->y, ci->w, ci->h }, 
                        (Vector2) { ci->w * 0.5f, ci->h * 0.5f }, 
                        ci->angle_deg, 
                        ci->tint
                    );
            }
                break;
                
            case GFX_DRAW_TAB: {
                const TabInfo* ti = render_arr[i].tab;
                if (ti->w <= 0 || ti->h <= 0)
                    continue;

                if (ti->is_open)
                    DrawRectanglePro(
                        (Rectangle) { ti->x, ti->y, ti->w, ti->h },
                        (Vector2) { ti->w * 0.5f, ti->h * 0.5f },
                        0.0f,
                        COLOR_TAB_BG
                    );

                const float arrow_off = 7.0f;
                Vector2 roll_pos;
                switch (ti->roll_dir) {
                    case TAB_ROLL_UP:   roll_pos.x = ti->x; roll_pos.y = ti->y - ti->h * 0.5f + ti->rolled_h * 0.5f; break;
                    case TAB_ROLL_DOWN: roll_pos.x = ti->x; roll_pos.y = ti->y + ti->h * 0.5f - ti->rolled_h * 0.5f; break;
                }
                DrawRectangle(roll_pos.x - ti->w * 0.5f, roll_pos.y - ti->rolled_h * 0.5f, ti->w, ti->rolled_h, ti->tint);
                DrawTextPro(text_font, ti->title, roll_pos, (Vector2) { ti->w * 0.5f - ti->rolled_h - arrow_off, ti->rolled_h * 0.5f }, 0.0f, ti->rolled_h, 1.0f, COLOR_TAB_TEXT);
                if ((ti->is_open && ti->roll_dir == TAB_ROLL_UP) || (!ti->is_open && ti->roll_dir == TAB_ROLL_DOWN))
                    DrawTriangle(
                        (Vector2) { roll_pos.x - ti->w * 0.5f + ti->rolled_h * 0.5f, roll_pos.y - ti->rolled_h * 0.5f + arrow_off },
                        (Vector2) { roll_pos.x - ti->w * 0.5f + arrow_off, roll_pos.y + ti->rolled_h * 0.5f - arrow_off },
                        (Vector2) { roll_pos.x - ti->w * 0.5f + ti->rolled_h - arrow_off, roll_pos.y + ti->rolled_h * 0.5f - arrow_off },
                        COLOR_TAB_TEXT
                    );
                else
                    DrawTriangle(
                        (Vector2) { roll_pos.x - ti->w * 0.5f + ti->rolled_h - arrow_off, roll_pos.y - ti->rolled_h * 0.5f + arrow_off },
                        (Vector2) { roll_pos.x - ti->w * 0.5f + arrow_off, roll_pos.y - ti->rolled_h * 0.5f + arrow_off },
                        (Vector2) { roll_pos.x - ti->w * 0.5f + ti->rolled_h * 0.5f, roll_pos.y + ti->rolled_h * 0.5f - arrow_off },
                        COLOR_TAB_TEXT
                    );
            }
                break;
        }
    }

    EndDrawing();
    render_idx = 0;
}

void GFX_DeInit(void) {
    UnloadFont(sym_font);
    UnloadFont(text_font);
    //UnloadTexture(ci->atlas->texture);
    UnloadTexture(figure_atlas);
    CloseWindow();
}