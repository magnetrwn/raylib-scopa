#ifndef DECK_H_
#define DECK_H_

#include "card.h"
#define DECK_SIZE CARDS_N

typedef struct _Deck {
    int cards[DECK_SIZE];
    int index;
} Deck;

void DECK_Create(Deck* d);
void DECK_Shuffle(Deck* d);
Card DECK_Pop(Deck* d);

#endif // DECK_H_