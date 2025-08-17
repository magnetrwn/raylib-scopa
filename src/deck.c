#include "deck.h"
#include <stdlib.h>
#include <time.h>

void DECK_RNGInit(void) {
    srand(time(NULL));
}

Deck DECK_Create(void) {
    Deck d;
    for (int i = 0; i < DECK_SIZE; ++i)
        d.cards[i] = i;
    d.index = 0;
    return d;
}

void DECK_Shuffle(Deck* d) {
    for (int i = 0; i < DECK_SIZE; ++i) {
        int j = rand() % DECK_SIZE;
        int swap = d->cards[j];
        d->cards[j] = d->cards[i];
        d->cards[i] = swap;
    }
}

Card DECK_Pop(Deck* d) {
    if (d->index < DECK_SIZE)
        return CARD_SET[d->cards[d->index++]];
    else
        return CARD_ERROR;
}