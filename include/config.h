#ifndef CONFIG_H
#define CONFIG_H

#include <SDL/SDL_mixer.h>
#include "constants.h"

extern int window_width;
extern int window_height;
extern char* music_path;
extern int volume;
extern char* font_path;
extern int sound_volume;
extern char* swipe_sound_path;

Config init_config(void);
void save_config(Config config, const char* path);
void load_config(const char* filename);
char* read_config(const char* key, const char* filename);

#define COMPARE_SUCCESS 0
#define CONFIG_PATH "documents/config.txt"

#endif