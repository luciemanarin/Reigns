#ifndef INIT_H
#define INIT_H

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include "game.h"

//gauche droite
#define BUTTON_LEFT_MARGIN 50
#define RIGHT_BUTTON_MARGIN 100

#define TTF_INIT_ERROR -1
#define RECT_INIT_COORD_WIDTH 0
#define RECT_INIT_COORD_HEIGHT 0
#define SDL_INIT_ERROR 0

#define BONUS_NONE 0


extern TTF_Font *font;
extern SDL_Color textColor;
extern SDL_Color redColor;
extern SDL_Rect left_area, right_area;

int init_game(Game *game);
void render_text(Game *game, const char *text, int x, int y);
void render_text_color(Game *game, const char *text, int x, int y, SDL_Color color);

#endif