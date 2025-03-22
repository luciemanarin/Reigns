// card.h
#ifndef CARD_H
#define CARD_H

#include <SDL/SDL.h>
#include <sqlite3.h>
#include "event.h"

#define DEFAULT_CARD_SIZE 300
#define NO_TRANSPARENT_COLOR 0

typedef struct {
    SDL_Surface *image;
    int dragging;
    int start_x;
    int start_y;
    int card_start_x;
    int card_start_y;
    Event *event;
} Card;

extern Card current_card;

void init_card(sqlite3 *database);
void load_next_card(sqlite3 *database);
void cleanup_card(void);

#endif