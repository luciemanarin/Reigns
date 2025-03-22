#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include "card.h"
#include "database.h"
#include "resource.h"
#include "image.h"
#include "game.h"
#include "event.h"
#include "init.h"
#include "constants.h"

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define RMASK 0xff000000
#define GMASK 0x00ff0000
#define BMASK 0x0000ff00
#define AMASK 0x000000ff
#define RSHIFT 24
#define GSHIFT 16
#define BSHIFT 8
#define ASHIFT 0
#else
#define RMASK 0x000000ff
#define GMASK 0x0000ff00
#define BMASK 0x00ff0000
#define AMASK 0xff000000
#define RSHIFT 0
#define GSHIFT 8
#define BSHIFT 16
#define ASHIFT 24
#endif

Card current_card;

void init_card(sqlite3 *database) {
    current_card.image = NULL;
    current_card.dragging = 0;
    current_card.start_x = 0;
    current_card.card_start_x = 0;
    current_card.event = NULL;

    Event *evt = get_random_event(database);
    if (evt && evt->chemin_image) {
        current_card.event = evt;

        SDL_Surface *temp = load_image(evt->chemin_image);
        if (temp) {

            current_card.image = SDL_CreateRGBSurface(NO_TRANSPARENT_COLOR,
                                                      DEFAULT_CARD_SIZE,
                                                      DEFAULT_CARD_SIZE,
                                                      32, RMASK, GMASK, BMASK, AMASK);
            //32 = 4 * 8, 4 bytes per pixel, 8 bits per byte
            if (current_card.image) {
                SDL_SoftStretch(temp, NULL, current_card.image, NULL);
            }
            SDL_FreeSurface(temp);
        }
    }
}

void load_next_card(sqlite3 *database) {
    if (current_card.image) {
        SDL_FreeSurface(current_card.image);
        current_card.image = NULL;
    }
    if (current_card.event) {
        free_event(current_card.event);
        current_card.event = NULL;
    }

    Event *evt = get_random_event_by_story(database, selected_story);
    if (evt) {
        current_card.event = evt;
        SDL_Surface *temp = load_image(evt->chemin_image);
        if (temp) {
            current_card.image = SDL_CreateRGBSurface(NO_TRANSPARENT_COLOR,
                                                      DEFAULT_CARD_SIZE,
                                                      DEFAULT_CARD_SIZE,
                                                      32, RMASK, GMASK, BMASK, AMASK);
            SDL_SoftStretch(temp, NULL, current_card.image, NULL);
            SDL_FreeSurface(temp);
        }
    }
}

void cleanup_card(void) {
    if (current_card.image) {
        SDL_FreeSurface(current_card.image);
        current_card.image = NULL;
    }

    if (current_card.event) {
        free_event(current_card.event);
        current_card.event = NULL;
    }
}