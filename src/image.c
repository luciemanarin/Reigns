#include "image.h"
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <time.h>
#include "game.h"
#include "card.h"
#include "event.h"
#include "resource.h"
#include "image.h"
#include "database.h"


SDL_Surface* load_image(const char* filename) {
    printf("=== Début chargement image ===\n");
    printf("Chemin du fichier: %s\n", filename);

    if (!filename) {
        printf("ERREUR: Chemin de fichier NULL\n");
        return NULL;
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("ERREUR: Fichier introuvable: %s\n", filename);
        return NULL;
    }
    fclose(file);

    SDL_Surface* loadedImage = NULL;
    SDL_Surface* optimizedImage = NULL;

    printf("Tentative de chargement avec IMG_Load...\n");
    loadedImage = IMG_Load(filename);

    if (loadedImage == NULL) {
        printf("ERREUR: Impossible de charger l'image: %s\n", IMG_GetError());
        printf("Chemin complet tenté: %s\n", filename);
        return NULL;
    }

    printf("Image chargée avec succès, dimensions: %dx%d\n",
           loadedImage->w, loadedImage->h);
    printf("Optimisation de l'image...\n");

    optimizedImage = SDL_DisplayFormat(loadedImage);
    SDL_FreeSurface(loadedImage);

    if (optimizedImage == NULL) {
        printf("ERREUR: Échec optimisation: %s\n", SDL_GetError());
        return NULL;
    }

    printf("Image optimisée avec succès\n");
    printf("Dimensions finales: %dx%d\n", optimizedImage->w, optimizedImage->h);
    printf("=== Fin chargement image ===\n");

    return optimizedImage;
}


void draw_card(SDL_Surface* screen, SDL_Surface* image, int window_width, int window_height) {
    if (screen && image) {
        SDL_Rect dest = {
                (window_width - DEFAULT_CARD_SIZE) / 2,
                (window_height - DEFAULT_CARD_SIZE) / 2,
                DEFAULT_CARD_SIZE,
                DEFAULT_CARD_SIZE
        };
        SDL_SoftStretch(image, NULL, screen, &dest);
    }
}

void cleanup_image(SDL_Surface* image) {
    if (image) {
        SDL_FreeSurface(image);
    }
}