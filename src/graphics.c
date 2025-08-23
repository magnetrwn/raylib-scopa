#include "graphics.h"
#include <raylib.h>
#include <stdio.h>
#include "card.h"
#include <math.h>

#define GFX_TOP_LEFT_SYMBOL { -11, -16, SYMBOL_SUIT }, { -11, -23, SYMBOL_RANK } // put the suit first to make it easier for face drawing
#define GFX_END_SYMBOL { 0, 0, SYMBOL_END }

// types related to card symbol layout and listing

typedef enum _CardSymbolType {
    SYMBOL_END,
    SYMBOL_RANK,
    SYMBOL_SUIT,
} CardSymbolType;

typedef struct _CardSymbol {
    int x, y;
    CardSymbolType type;
} CardSymbol;

typedef struct _CardSymbolInfo {
    CardSymbol syms[MAX_SYMBOLS_PER_CARD];
} CardSymbolInfo;

static const CardSymbolInfo CARD_SYMBOLS[] = {
    { .syms = { GFX_END_SYMBOL } }, // [0] unused

    // Ace
    { .syms = {
        GFX_TOP_LEFT_SYMBOL,
        {   0,   0,  SYMBOL_SUIT },
        GFX_END_SYMBOL
    }},

    // Two
    { .syms = {
        GFX_TOP_LEFT_SYMBOL,
        {   0,  -15, SYMBOL_SUIT }, {  0,  15, SYMBOL_SUIT },
        GFX_END_SYMBOL
    }},

    // Three
    { .syms = {
        GFX_TOP_LEFT_SYMBOL,
        {   0,  -15, SYMBOL_SUIT }, {  0,   0, SYMBOL_SUIT }, { 0, 15, SYMBOL_SUIT },
        GFX_END_SYMBOL
    }},

    // Four
    { .syms = {
        GFX_TOP_LEFT_SYMBOL,
        {  -5, -12, SYMBOL_SUIT }, {  5, -12, SYMBOL_SUIT },
        {  -5,  12, SYMBOL_SUIT }, {  5,  12, SYMBOL_SUIT },
        GFX_END_SYMBOL
    }},

    // Five
    { .syms = {
        GFX_TOP_LEFT_SYMBOL,
        {  -5, -12, SYMBOL_SUIT }, {  5, -12, SYMBOL_SUIT },
        {   0,   0,  SYMBOL_SUIT },
        {  -5,  12, SYMBOL_SUIT }, {  5,  12, SYMBOL_SUIT },
        GFX_END_SYMBOL
    }},

    // Six
    { .syms = {
        GFX_TOP_LEFT_SYMBOL,
        {  -5, -15, SYMBOL_SUIT }, {  5, -15, SYMBOL_SUIT },
        {  -5,   0,  SYMBOL_SUIT }, {  5,   0,  SYMBOL_SUIT },
        {  -5,  15, SYMBOL_SUIT }, {  5,  15, SYMBOL_SUIT },
        GFX_END_SYMBOL
    }},

    // Seven (six layout + top-center)
    { .syms = {
        GFX_TOP_LEFT_SYMBOL,
        {  -5, -15, SYMBOL_SUIT }, {  5, -15, SYMBOL_SUIT },
        {  -5,   0,  SYMBOL_SUIT }, {  5,   0,  SYMBOL_SUIT },
        {  -5,  15, SYMBOL_SUIT }, {  5,  15, SYMBOL_SUIT },
        {   0, -10, SYMBOL_SUIT },
        GFX_END_SYMBOL
    }},

    // j, q, k placeholders (corner only)
    { .syms = { GFX_TOP_LEFT_SYMBOL, GFX_END_SYMBOL }},
    { .syms = { GFX_TOP_LEFT_SYMBOL, GFX_END_SYMBOL }},
    { .syms = { GFX_TOP_LEFT_SYMBOL, GFX_END_SYMBOL }},
};

static Font sym_font;
static CardInfo render_arr[MAX_CARDS_IN_TICK];
static int render_idx;

static inline Vector2 _RotOffset(Vector2 off, float deg) {
    float r = deg * DEG2RAD, s = sinf(r), c = cosf(r);
    return (Vector2) { off.x * c - off.y * s, off.x * s + off.y * c };
}

static void _DrawCardFace(const CardInfo* ci) {
    const float font_size = FONT_BASE_MUL * ci->cri.h / SYMBOL_H_DIV_BY;
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
            }
        } else if (sym->type == SYMBOL_SUIT) {
            switch (ci->c.suit) {
                case CARD_SUIT_HEARTS:   to_draw = "{"; sym_color = COLOR_HEARTS;   break;
                case CARD_SUIT_DIAMONDS: to_draw = "["; sym_color = COLOR_DIAMONDS; break;
                case CARD_SUIT_CLUBS:    to_draw = "]"; sym_color = COLOR_CLUBS;    break;
                case CARD_SUIT_SPADES:   to_draw = "}"; sym_color = COLOR_SPADES;   break;
            }
        }

        Vector2 off = (Vector2){
            sym->x * ci->cri.w / SYMBOL_W_DIV_BY,
            sym->y * ci->cri.h / SYMBOL_H_DIV_BY
        };

        Vector2 pos = _RotOffset(off, ci->cri.angle_deg);
        pos.x += ci->cri.x; pos.y += ci->cri.y;

        DrawTextPro(sym_font, to_draw, pos, (sym->type == SYMBOL_SUIT) ? origin_suit : origin_rank, ci->cri.angle_deg, font_size, 1.0f, sym_color);
    }

    const Vector2 center = (Vector2){ ci->cri.x, ci->cri.y };
    const float w2 = ci->cri.w * 0.5f * BORDER_BASE_MUL;
    const float h2 = ci->cri.h * 0.5f * BORDER_BASE_MUL;
    const Vector2 off[5] = {{ -w2 * 0.7f, -h2 }, { +w2, -h2 }, { +w2, +h2 }, { -w2, +h2 }, { -w2, -h2 * 0.5f }};

    Vector2 pts[5]; // rotate the border line points
    for (int k = 0; k < 5; ++k) {
        Vector2 r = _RotOffset(off[k], ci->cri.angle_deg);
        pts[k].x = center.x + r.x;
        pts[k].y = center.y + r.y;
    }

    DrawLineStrip(pts, 5, sym_color);
}

void GFX_Init(int w, int h) {
    InitWindow(w, h, "GameWindow");
    SetTargetFPS(60);
    sym_font = LoadFont("assets/fnt/symbols.ttf");
    render_idx = 0;
}

void GFX_DrawCard(const CardInfo* ci) {
    if (ci == NULL || ci->cri.w <= 0 || ci->cri.h <= 0 || CARD_IsError(&ci->c) || render_idx >= MAX_CARDS_IN_TICK)
        return;
    render_arr[render_idx++] = *ci;
}

void GFX_Tick(void) {
    BeginDrawing();
    ClearBackground(BLACK);
    
    for (int i = 0; i < render_idx; ++i) {
        CardInfo* ci = &render_arr[i];
        DrawRectanglePro(
            (Rectangle) {ci->cri.x, ci->cri.y, ci->cri.w, ci->cri.h}, 
            (Vector2) {ci->cri.w / 2, ci->cri.h / 2}, 
            ci->cri.angle_deg,
            WHITE
        );

        _DrawCardFace(ci);
    }

    EndDrawing();
    render_idx = 0;
}

void GFX_DeInit(void) {
    UnloadFont(sym_font);
    CloseWindow();
}