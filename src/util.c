#include "util.h"

void UTIL_Rotate(Vector2* v, float deg) { 
    float r = deg * DEG2RAD, s = sinf(r), c = cosf(r), x = v->x, y = v->y; 
    v->x = x * c - y * s; 
    v->y = x * s + y * c;
}