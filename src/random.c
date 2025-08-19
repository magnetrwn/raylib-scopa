#include "random.h"
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

static bool initialized = false;

void RND_InitOnce(void) {
    if (!initialized) {
        srand(time(NULL));
        initialized = true;
    }
}

int RND_Get(int min, int max) {
    if (!initialized)
        return -1;
    return min + rand() % (max - min + 1);
}