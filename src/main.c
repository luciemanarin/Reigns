    #include <windows.h>
    #include <SDL/SDL.h>
    #include <SDL/SDL_mixer.h>
    #include <SDL/SDL_ttf.h>
    #include <SDL/SDL_image.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <sqlite3.h>
    #include "game.h"
    #include "config.h"
    #include "config.h"
    #include "card.h"
    #include "init.h"
    #include "database.h"
    #include "event.h"
    #include "story.h"

    #define AUDIO_FREQ 44100
    #define AUDIO_CHANNELS 2
    #define AUDIO_BUFFER 2048
    #define CONFIG_PATH "documents/config.txt"
    #define MUSIC_BOUCLE -1

    extern FILE *log_file;

    int main(int argc, char *argv[]) {
        Game game = {0};
        game.state = GAME_STORY_SELECT;

        log_file = fopen("reigns.log", "w");
        if (!log_file) {
            fprintf(stderr, "Impossible de créer le fichier log\n");
            return EXIT_FAILURE;
        }

        fprintf(log_file, "Démarrage du programme...\n");
        fflush(log_file);

        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
            fprintf(log_file, "Erreur SDL_Init : %s\n", SDL_GetError());
            fclose(log_file);
            return EXIT_FAILURE;
        }

        int imgFlags = IMG_INIT_PNG;
        if (!(IMG_Init(imgFlags) & imgFlags)) {
            fprintf(log_file, "Erreur IMG_Init : %s\n", IMG_GetError());
            SDL_Quit();
            fclose(log_file);
            return EXIT_FAILURE;
        }

        if (Mix_OpenAudio(AUDIO_FREQ, MIX_DEFAULT_FORMAT, AUDIO_CHANNELS, AUDIO_BUFFER) != 0) {
            fprintf(log_file, "Erreur Mix_OpenAudio : %s\n", Mix_GetError());
            IMG_Quit();
            SDL_Quit();
            fclose(log_file);
            return EXIT_FAILURE;
        }

        char *music_path = read_config("music_path", CONFIG_PATH);
        if (!music_path) {
            fprintf(log_file, "Erreur lecture configuration musique\n");
            Mix_CloseAudio();
            IMG_Quit();
            SDL_Quit();
            fclose(log_file);
            return EXIT_FAILURE;
        }

        char *volume_str = read_config("volume", CONFIG_PATH);
        if (!volume_str) {
            fprintf(log_file, "Erreur lecture configuration volume\n");
            free(music_path);
            Mix_CloseAudio();
            IMG_Quit();
            SDL_Quit();
            fclose(log_file);
            return EXIT_FAILURE;
        }

        int volume = atoi(volume_str);
        free(volume_str);

        Mix_Music *background_music = Mix_LoadMUS(music_path);
        free(music_path);

        if (!background_music) {
            fprintf(log_file, "Erreur chargement musique : %s\n", Mix_GetError());
            Mix_CloseAudio();
            IMG_Quit();
            SDL_Quit();
            fclose(log_file);
            return EXIT_FAILURE;
        }

        Mix_VolumeMusic(volume);
        Mix_PlayMusic(background_music, MUSIC_BOUCLE);

        if (TTF_Init() < 0) {
            fprintf(log_file, "Erreur TTF_Init : %s\n", TTF_GetError());
            Mix_FreeMusic(background_music);
            Mix_CloseAudio();
            IMG_Quit();
            SDL_Quit();
            fclose(log_file);
            return EXIT_FAILURE;
        }


        fprintf(log_file, "Initialisation du jeu...\n");
        if (!init_game(&game)) {
            fprintf(log_file, "Erreur initialisation jeu\n");
            TTF_Quit();
            Mix_FreeMusic(background_music);
            Mix_CloseAudio();
            IMG_Quit();
            SDL_Quit();
            fclose(log_file);
            return EXIT_FAILURE;
        }

        if (init_database(&game.database) != EVENT_SUCCESS) {
            fprintf(log_file, "Erreur initialisation base de données\n");
            cleanup_game(&game);
            TTF_Quit();
            Mix_FreeMusic(background_music);
            Mix_CloseAudio();
            IMG_Quit();
            SDL_Quit();
            fclose(log_file);
            return EXIT_FAILURE;
        }

        initialize_resources(&game);
        init_card(game.database);

        fprintf(log_file, "État initial du jeu : %d\n", game.state);
        fflush(log_file);


        SDL_Event event;
        while (game.state != GAME_QUIT) {
            while (SDL_PollEvent(&event)) {
                handle_events(&game, &event);
            }
            render_game(&game);
            SDL_Delay(FRAME_DELAY);
        }

        Mix_FreeMusic(background_music);
        cleanup_game(&game);
        TTF_Quit();
        Mix_CloseAudio();
        IMG_Quit();
        SDL_Quit();

        fprintf(log_file, "Programme terminé avec succès\n");
        fclose(log_file);
        return EXIT_SUCCESS;
    }