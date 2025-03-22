    #ifndef IMAGE_H
    #define IMAGE_H

    #include <SDL/SDL.h>
    #include "constants.h"


    #define IMAGE_WIDTH 0
    #define IMAGE_HEIGHT 0
    #define SRC_START_X 0
    #define SRC_START_Y 0

    SDL_Surface* load_image(const char* path);
    void draw_image(SDL_Surface* screen, SDL_Surface* image, int x, int y);
    void draw_scaled_image(SDL_Surface* screen, SDL_Surface* image, int x, int y, int width, int height);
    void cleanup_image(SDL_Surface* image);

    #endif