#ifndef CARD_H_
#define CARD_H_

#include <stdbool.h>
#define CARD_ERROR ((Card) {0, ERROR})

typedef enum _CardSuit {
    CARD_SUIT_HEARTS,
    CARD_SUIT_DIAMONDS,
    CARD_SUIT_CLUBS,
    CARD_SUIT_SPADES
} CardSuit;

typedef enum _CardRank {
    CARD_RANK_ACE = 1,
    CARD_RANK_TWO,
    CARD_RANK_THREE,
    CARD_RANK_FOUR,
    CARD_RANK_FIVE,
    CARD_RANK_SIX,
    CARD_RANK_SEVEN,
    CARD_RANK_JACK,
    CARD_RANK_QUEEN,
    CARD_RANK_KING,
    ERROR
} CardRank;

typedef struct _Card {
    CardSuit suit;
    CardRank rank;
} Card;

bool CARD_IsError(const Card* c);

#endif // CARD_H_