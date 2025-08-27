#include "graphics.h"
#include "raylib.h"
#include <stdio.h>
#include "card.h"
#include <string.h>
#include "util.h"

#define ATLAS_PAD_PX 4
#define ATLAS_UPSCALE_MUL    2.0f // render the atlas at this much size for better quality when scaling down

#define SYMBOL_W_DIV_BY      28.0f // divide the card width by this many units and use the symbol layouts over the generated grid
#define SYMBOL_H_DIV_BY      54.0f // divide the card width by this many units and use the symbol layouts over the generated grid
#define FONT_BASE_LOAD       96    // base font size to load the symbol font with
#define FONT_BASE_MUL        8.0f  // base font size multiplier before scaling
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

const Rectangle FIGURE_ATLAS_SRC[3] = {
    { 2.0f, 0.0f, 280.0f, 480.0f }, // j
    { 284.0f, 0.0f, 280.0f, 480.0f }, // q
    { 566.0f, 0.0f, 280.0f, 480.0f }, // k
};

static Font sym_font;
static Texture2D figure_atlas;
static CardInfo render_arr[MAX_CARDS_IN_TICK];
static int render_idx;

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
    
        const float scale = fminf(w2 / src.width, h2 / src.height) * 2.0f;
        const float dst_w = src.width * scale;
        const float dst_h = src.height * scale;

        Rectangle dst = { center.x, center.y, dst_w, dst_h };
        Vector2 pivot = { dst_w * 0.5f, dst_h * 0.5f };

        DrawTexturePro(figure_atlas, src, dst, pivot, ci->angle_deg, sym_color);
    }
}

static inline Rectangle _CardAtlasSrc(int suit, int rank, int card_w, int card_h) { 
    card_w *= ATLAS_UPSCALE_MUL;
    card_h *= ATLAS_UPSCALE_MUL;

    return (Rectangle) { 
        (float) (ATLAS_PAD_PX + (rank - 1) * (card_w + ATLAS_PAD_PX)), 
        (float) (ATLAS_PAD_PX + suit * (card_h + ATLAS_PAD_PX)), 
        (float) card_w, 
        -(float) card_h
    }; 
}

// public api

void GFX_Init(int w, int h) {
    InitWindow(w, h, "GameWindow");
    SetTargetFPS(60);

    sym_font = LoadFontEx("assets/fnt/symbols.ttf", FONT_BASE_LOAD, NULL, 0);

    Image i = LoadImage("assets/img/face-figure-atlas.png");
    figure_atlas = LoadTextureFromImage(i);
    UnloadImage(i);

    render_idx = 0;
}

void GFX_BuildCardTextureAtlas(RenderTexture2D *out, int card_w, int card_h) {
    card_w *= ATLAS_UPSCALE_MUL;
    card_h *= ATLAS_UPSCALE_MUL;

    *out = LoadRenderTexture(
        CARD_RANKS * card_w + (CARD_RANKS + 1) * ATLAS_PAD_PX, 
        (CARD_SUITS + 1) * card_h + (CARD_SUITS + 2) * ATLAS_PAD_PX // add a row for back drawing
    );
    SetTextureWrap(out->texture, TEXTURE_WRAP_CLAMP);

    BeginTextureMode(*out);
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

        const int N = 96;
        Vector2 pts[96];

        for (int i = 0; i < N; ++i) {
            float t = (2 * PI * i) / (N - 1);
            float r = rad * 0.75f * cosf(8.0f * t);
            pts[i] = (Vector2) { c.x + r * sinf(t), c.y + r * cosf(t) };
        }
        DrawLineStrip(pts, N, COLOR_THEMES[(theme + 1) % COLOR_THEMES_N]);
        DrawRing(c, rad * 0.70f, rad * 0.75f, 0, 360, 128, COLOR_THEMES[(theme + 1) % COLOR_THEMES_N]);

        for (int i = 0; i < N; ++i) {
            float t = (2 * PI * i) / (N - 1);
            float r = rad * 1.0f * cosf(5.0f * t + 0.85f);
            pts[i] = (Vector2) { c.x + r * sinf(t), c.y + r * cosf(t) };
        }
        DrawLineStrip(pts, N, COLOR_THEMES[(theme + 2) % COLOR_THEMES_N]);
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
        DrawLineStrip(pts, 5, COLOR_THEMES[(theme + 1) % COLOR_THEMES_N]);

        w2 = back.w * 0.5f * BORDER_BASE_BACK_MUL + 4.0f;
        h2 = back.h * 0.5f * BORDER_BASE_BACK_MUL + 4.0f;
        const Vector2 off2[5] = {
            { -w2, -h2 }, { +w2, -h2 }, { +w2, +h2 }, { -w2, +h2 }, { -w2, -h2 }
        };
        for (int k = 0; k < 5; ++k) {
            Vector2 r = off2[k];
            pts[k].x = c.x + r.x;
            pts[k].y = c.y + r.y;
        }
        DrawLineStrip(pts, 5, COLOR_THEMES[(theme + 2) % COLOR_THEMES_N]);

        BeginBlendMode(BLEND_ADDITIVE);
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 6; ++j) {
                if ((i + j) % 2 == 0)
                    DrawRectangle(
                        c.x - (int)(back.w * 0.5f) + i * (int)(back.w / 4),
                        c.y - (int)(back.h * 0.5f) + j * (int)(back.h / 6),
                        back.w / 4,
                        back.h / 6,
                        (Color) { 255, 255, 255, 18 }
                    );
                else
                    DrawRectangle(
                        c.x - (int)(back.w * 0.5f) + i * (int)(back.w / 4),
                        c.y - (int)(back.h * 0.5f) + j * (int)(back.h / 6),
                        back.w / 4,
                        back.h / 6,
                        (Color) { 255, 255, 255, 0 }
                    );
            }
        }
        EndBlendMode();
    }

    EndTextureMode();
    GenTextureMipmaps(&out->texture);
    SetTextureFilter(out->texture, TEXTURE_FILTER_TRILINEAR);
}

void GFX_DrawCard(const CardInfo* ci) {
    if (ci == NULL || render_idx >= MAX_CARDS_IN_TICK)
        return;
    render_arr[render_idx++] = *ci;
}

void GFX_DrawCardN(const CardInfo* ci, int n) {
    if (ci == NULL || n <= 0)
        return;
    if (render_idx + n >= MAX_CARDS_IN_TICK)
        n = MAX_CARDS_IN_TICK - render_idx;
    memcpy(&render_arr[render_idx], ci, sizeof(CardInfo) * n);
    render_idx += n;
}

void GFX_RenderTick(const RenderTexture2D* atlas) {
    BeginDrawing();
    ClearBackground(COLOR_BG);

    // DrawTextureRec(atlas->texture,
    //     (Rectangle) { 0, -160, (float) atlas->texture.width, (float) -atlas->texture.height },
    //     (Vector2) { 0, 0 }, 
    //     (Color) { 255, 255, 255, 255 }
    // );
    
    for (int i = 0; i < render_idx; ++i) {
        CardInfo* ci = &render_arr[i];
        if (ci->w <= 0 || ci->h <= 0 || CARD_IsError(&ci->c))
            continue;
        DrawTexturePro(
            atlas->texture, 
            _CardAtlasSrc(ci->c.suit, ci->c.rank, ci->w, ci->h), 
            (Rectangle) { ci->x, ci->y, ci->w, ci->h }, 
            (Vector2) { ci->w * 0.5f, ci->h * 0.5f }, 
            ci->angle_deg, 
            ci->tint
        );
    }

    EndDrawing();
    render_idx = 0;
}

void GFX_DeInit(void) {
    UnloadFont(sym_font);
    UnloadTexture(figure_atlas);
    CloseWindow();
}