#include "ui.h"
#include <unistd.h>
#include <string.h>
#include "util.h"

static IfElement el_arr[MAX_IF_ELS_IN_TICK];
static IfElEvent evt_arr[MAX_IF_ELS_IN_TICK];
static int el_idx;

void UI_Init(void) {
    el_idx = 0;
}

void UI_IfCreateLabel(IfElement* ie, const char* text, const Rectangle info) {
    ie->type = IF_EL_LABEL;
    ie->text = text;
    ie->corners[0] = (Vector2) { info.x, info.y };
    ie->corners[1] = (Vector2) { info.x + info.width, info.y };
    ie->corners[2] = (Vector2) { info.x + info.width, info.y + info.height };
    ie->corners[3] = (Vector2) { info.x, info.y + info.height };
}

void UI_IfCreateButton(IfElement* ie, const char* text, const Rectangle info) {
    ie->type = IF_EL_BTN;
    ie->text = text;
    ie->corners[0] = (Vector2) { info.x, info.y };
    ie->corners[1] = (Vector2) { info.x + info.width, info.y };
    ie->corners[2] = (Vector2) { info.x + info.width, info.y + info.height };
    ie->corners[3] = (Vector2) { info.x, info.y + info.height };
}

void UI_IfCreateCard(IfElement* ie, CardInfo* ci) {
    ie->type = IF_EL_CARD;
    ie->card = ci;

    const float cx = ci->x, cy = ci->y;
    const float w2 = ci->w * 0.5f, h2 = ci->h * 0.5f;

    Vector2 off[4] = { { -w2, -h2 }, { +w2, -h2 }, { +w2, +h2 }, { -w2, +h2 } };
    for (int i = 0; i < 4; ++i) {
        Vector2 v = off[i];
        UTIL_Rotate(&v, ci->angle_deg);
        ie->corners[i].x = cx + v.x;
        ie->corners[i].y = cy + v.y;
    }
}

void UI_IfPlace(const IfElement* ie) {
    if (!ie || el_idx >= MAX_IF_ELS_IN_TICK) 
        return;
    el_arr[el_idx++] = *ie;
}

void UI_IfPlaceN(const IfElement* ie, int n) {
    if (!ie || n <= 0) 
        return;
    if (el_idx + n > MAX_IF_ELS_IN_TICK) 
        n = MAX_IF_ELS_IN_TICK - el_idx;
    memcpy(&el_arr[el_idx], ie, sizeof(IfElement) * n);
    el_idx += n;
}

void UI_IfTick(Vector2 mouse_pos, bool mouse_click) {
    for (int i = 0; i < el_idx; ++i) {
        IfElement* ie = &el_arr[i];
        evt_arr[i] = IF_EVT_NONE;

        switch (ie->type) {
            case IF_EL_NONE:
                break;
            case IF_EL_LABEL:
                break;
            case IF_EL_CARD:
                ie->card->tint = WHITE;
                evt_arr[i] = IF_EVT_NONE;
                if (CheckCollisionPointTriangle(mouse_pos, ie->corners[0], ie->corners[1], ie->corners[2]) ||
                    CheckCollisionPointTriangle(mouse_pos, ie->corners[0], ie->corners[2], ie->corners[3])) {
                    //ie->card->tint = COLOR_HOVER;
                    evt_arr[i] = IF_EVT_HOVER;
                    if (mouse_click) {
                        //ie->card->tint = COLOR_CLICK;
                        evt_arr[i] = IF_EVT_CLICK;
                    }
                }
                break;
            case IF_EL_BTN:
                break;
            default:
                break;
        }
    }

    el_idx = 0;
}

IfEvtIdx UI_EvtPop(void) {
    for (int i = MAX_IF_ELS_IN_TICK - 1; i >= 0; --i) {
        if (evt_arr[i] == IF_EVT_NONE)
            continue;

        IfEvtIdx ret = { evt_arr[i], i };
        evt_arr[i] = IF_EVT_NONE;
        return ret;
    }
    return NO_EVENTS_LEFT;
}