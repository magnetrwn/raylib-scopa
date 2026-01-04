#ifndef UI_H_
#define UI_H_

#include "graphics.h"
#define MAX_IF_ELS_IN_TICK MAX_CARDS_IN_TICK
#define NO_EVENTS_LEFT (IfEvtIdx) { IF_EVT_NONE, -1 }

typedef enum _IfElType {
    IF_EL_NONE,
    IF_EL_TAB,
    IF_EL_CARD
} IfElType;

typedef enum _IfElEvent {
    IF_EVT_NONE,
    IF_EVT_HOVER,
    IF_EVT_CLICK
} IfElEvent;

typedef struct _IfEvtIdx {
    IfElEvent evt;
    int idx;
} IfEvtIdx;

typedef struct _IfElement {
    IfElType type;
    union { const TabInfo* tab; const CardInfo* card; };
    union { Vector2 pos; Vector2 corners[4]; };
} IfElement; // you should use this to keep track of UI elements together with CardInfo[]

typedef struct _IfTickInputs {
    Vector2 mouse_pos;
    bool mouse_click_l;
    bool mouse_down_l;
} IfTickInputs;

// typedef enum _DetailType {
//     PLAYER_DECK,
//     PLAYER_DECK_DETAILS,
//     TABLE,
//     TABLE_DETAILS,
//     ROUND_INDICATOR,
// } DetailType;

void UI_Init(void);

void UI_IfCreateTab(IfElement* ie, const TabInfo* ti);
void UI_IfCreateCard(IfElement* ie, const CardInfo* ci);
void UI_IfPlace(const IfElement* ie);
void UI_IfPlaceN(const IfElement* ie, int n);
void UI_IfTick(const IfTickInputs* in);
IfEvtIdx UI_EvtPop(void);
IfEvtIdx UI_EvtFind(IfElEvent type);
void UI_IfClear(void);

#endif // UI_H_