#include "card.h"
#include <stdbool.h>

bool CARD_IsError(const Card* c) {
    return c->rank == CARD_ERROR.rank;
}

bool CARD_IsEqual(const Card* a, const Card* b) {
    if (CARD_IsError(a) || CARD_IsError(b))
        return false;
        
    return (a->suit == b->suit) && (a->rank == b->rank);
}