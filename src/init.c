#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "init.h"
#include "game.h"

TTF_Font *font = NULL;
SDL_Color textColor = COLOR_WHITE;
SDL_Color redColor = COLOR_RED;
SDL_Rect left_area, right_area;


void handle_resize(Game *game) {
    if (!game) return;

    window_width = SDL_min(SDL_max(window_width, MIN_WINDOW_WIDTH), MAX_WINDOW_WIDTH);
    window_height = SDL_min(SDL_max(window_height, MIN_WINDOW_HEIGHT), MAX_WINDOW_HEIGHT);


    game->screen = SDL_SetVideoMode(window_width, window_height, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (!game->screen) {
        fprintf(stderr, "Erreur SDL_SetVideoMode : %s\n", SDL_GetError());
        return;
    }

    left_area.x = BUTTON_LEFT_MARGIN;
    left_area.y = window_height / 2;
    right_area.x = window_width - RIGHT_BUTTON_MARGIN;
    right_area.y = window_height / 2;
}

int init_game(Game *game) {
    if (!game) return INIT_FAILURE;

    game->state = GAME_STORY_SELECT;
    game->screen = NULL;
    game->database = NULL;

    permanent_bonus.bouclier_active = false;
    permanent_bonus.equilibre_active = false;
    permanent_bonus.bouclier_duration = BONUS_NONE;
    permanent_bonus.equilibre_duration = BONUS_NONE;

    if (SDL_Init(SDL_INIT_VIDEO) < SDL_INIT_ERROR) {
        fprintf(stderr, "Erreur SDL_Init: %s\n", SDL_GetError());
        return INIT_FAILURE;
    }

    load_config("documents/config.txt");

    game->screen = SDL_SetVideoMode(window_width, window_height, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (!game->screen) {
        fprintf(stderr, "Erreur SDL_SetVideoMode : %s\n", SDL_GetError());
        return INIT_FAILURE;
    }

    SDL_WM_SetCaption("Reigns", NULL);

    left_area.x = BUTTON_LEFT_MARGIN;
    left_area.y = window_height / 2;
    right_area.x = window_width - RIGHT_BUTTON_MARGIN;
    right_area.y = window_height / 2;

    if (TTF_Init() == TTF_INIT_ERROR) {
        fprintf(stderr, "Erreur TTF_Init: %s\n", TTF_GetError());
        return INIT_FAILURE;
    }

    font = TTF_OpenFont(FONT_PATH, FONT_SIZE);
    if (!font) {
        fprintf(stderr, "Erreur chargement police %s: %s\n", FONT_PATH, TTF_GetError());
        return INIT_FAILURE;
    }

    return INIT_SUCCESS;
}

void render_text(Game *game, const char *text, int x, int y) {
    if (!font || !game || !game->screen) return;
    SDL_Surface *message = TTF_RenderUTF8_Solid(font, text, textColor);
    if (message) {
        SDL_Rect dest = {x, y, RECT_INIT_COORD_WIDTH, RECT_INIT_COORD_HEIGHT};
        SDL_BlitSurface(message, NULL, game->screen, &dest);
        SDL_FreeSurface(message);
    }
}

void render_text_color(Game *game, const char *text, int x, int y, SDL_Color color) {
    if (!font || !game || !game->screen) return;
    SDL_Surface *message = TTF_RenderUTF8_Solid(font, text, color);
    if (message) {
        SDL_Rect dest = {x, y, RECT_INIT_COORD_WIDTH, RECT_INIT_COORD_HEIGHT};
        SDL_BlitSurface(message, NULL, game->screen, &dest);
        SDL_FreeSurface(message);
    }
}