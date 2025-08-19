#include "card.h"
#include <stdbool.h>

bool CARD_IsError(const Card* c) {
    return c->rank == CARD_ERROR.rank;
}