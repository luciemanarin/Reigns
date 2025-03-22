#include <SDL/SDL_mixer.h>
#include <stdio.h>
#include "audio.h"
#include "config.h"
#include "constants.h"

Mix_Music* load_music(const char* file) {
    Mix_Music* music = Mix_LoadMUS(file);
    if (!music) {
        fprintf(stderr, ERROR_LOAD_MUSIC, Mix_GetError());
    }
    return music;
}

void play_music(Mix_Music* music) {
    if (music) {
        Mix_PlayMusic(music, LOOP_INFINITE);
    }
}

void set_volume(int volume) {
    Mix_VolumeMusic(volume);

    void stop_music() {
        Mix_HaltMusic();
    }
}