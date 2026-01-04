#ifndef CARD_H_
#define CARD_H_

#include <stdbool.h>
#define CARD_SUITS 4
#define CARD_RANKS 10
#define CARDS_N (CARD_SUITS * CARD_RANKS)
#define NO_CARDS_LEFT ((Card) {-1, -1})

typedef enum {
    CARD_SUIT_HEARTS,
    CARD_SUIT_DIAMONDS,
    CARD_SUIT_CLUBS,
    CARD_SUIT_SPADES
} CardSuit;

typedef enum {
    CARD_RANK_ACE = 1,
    CARD_RANK_TWO,
    CARD_RANK_THREE,
    CARD_RANK_FOUR,
    CARD_RANK_FIVE,
    CARD_RANK_SIX,
    CARD_RANK_SEVEN,
    CARD_RANK_JACK,
    CARD_RANK_QUEEN,
    CARD_RANK_KING
} CardRank;

typedef struct {
    CardSuit suit;
    CardRank rank;
} Card;

bool CARD_IsError(const Card* c);
bool CARD_IsEqual(const Card* a, const Card* b);

#endif // CARD_H_