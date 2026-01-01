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

void UI_IfCreateTab(IfElement* ie, const TabInfo* ti) {
    ie->type = IF_EL_TAB;
    ie->tab = ti;
    ie->pos.x = ti->x - ti->w * 0.5f;
    switch (ti->roll_dir) {
        case TAB_ROLL_UP:
            ie->pos.y = ti->y - ti->h * 0.5f;
            break;
        case TAB_ROLL_DOWN:
            ie->pos.y = ti->y + ti->h * 0.5f - ti->rolled_h;
            break;
    }
}

void UI_IfCreateCard(IfElement* ie, const CardInfo* ci) {
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

void UI_IfTick(const IfTickInputs* in) {
    for (int i = 0; i < el_idx; ++i) {
        IfElement* ie = &el_arr[i];
        evt_arr[i] = IF_EVT_NONE;

        switch (ie->type) {
            case IF_EL_NONE:
                break;
            case IF_EL_TAB:
                if (CheckCollisionPointRec(in->mouse_pos, (Rectangle) { ie->pos.x, ie->pos.y, ie->tab->w, ie->tab->rolled_h })) {
                    evt_arr[i] = IF_EVT_HOVER;
                    if (in->mouse_click_l)
                        evt_arr[i] = IF_EVT_CLICK;
                }
                break;
            case IF_EL_CARD:
                if (CheckCollisionPointTriangle(in->mouse_pos, ie->corners[0], ie->corners[1], ie->corners[2]) ||
                    CheckCollisionPointTriangle(in->mouse_pos, ie->corners[0], ie->corners[2], ie->corners[3])) {
                    evt_arr[i] = IF_EVT_HOVER;
                    if (in->mouse_click_l)
                        evt_arr[i] = IF_EVT_CLICK;
                }
                break;
            default:
                break;
        }
    }

    // make sure to call UI_IfClear() after processing events at the end of the main tick loop
}

IfEvtIdx UI_EvtPop(void) {
    for (int i = el_idx; i >= 0; --i) {
        if (evt_arr[i] == IF_EVT_NONE)
            continue;

        IfEvtIdx ret = { evt_arr[i], i };
        evt_arr[i] = IF_EVT_NONE;
        return ret;
    }
    return NO_EVENTS_LEFT;
}

IfEvtIdx UI_EvtFind(IfElEvent type) {
    for (int i = el_idx; i >= 0; --i) {
        if (evt_arr[i] != type)
            continue;

        return (IfEvtIdx) { evt_arr[i], i };
    }
    return NO_EVENTS_LEFT;
}

void UI_IfClear(void) {
    el_idx = 0;
}