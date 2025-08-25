#ifndef UI_H_
#define UI_H_

#include "graphics.h"
#define MAX_IF_ELS_IN_TICK 128

typedef enum _IfElType {
    IF_EL_NONE,
    IF_EL_LABEL,
    IF_EL_BTN,
    IF_EL_CARD
} IfElType;

typedef enum _IfElEvent {
    IF_EVT_NONE,
    IF_EVT_HOVER,
    IF_EVT_CLICK
} IfElEvent;

typedef struct _IfElement {
    IfElType type;
    IfElEvent event;
    union { const char* text; CardInfo* card; };
    Vector2 corners[4]; 
} IfElement;

void UI_Init(void);
void UI_IfCreateLabel(IfElement* ie, const char* text, const Rectangle info);
void UI_IfCreateButton(IfElement* ie, const char* text, const Rectangle info);
void UI_IfCreateCard(IfElement* ie, const CardInfo* ci);
void UI_IfPlace(const IfElement* ie);
void UI_IfPlaceN(const IfElement* ie, int n);
void UI_IfTick(Vector2 mouse_pos, bool mouse_click);

#endif // UI_H_