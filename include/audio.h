#ifndef AUDIO_H
#define AUDIO_H

#include <SDL/SDL_mixer.h>

#define LOOP_INFINITE -1
#define ERROR_LOAD_MUSIC "Erreur chargement musique : %s\n"

Mix_Music* load_music(const char* file);
void play_music(Mix_Music* music);
void set_volume(int volume);
void stop_music();

#endif
