#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "init.h"
#include "constants.h"

int window_width = DEFAULT_WINDOW_WIDTH;
int window_height = DEFAULT_WINDOW_HEIGHT;
char* music_path = NULL;
int volume = DEFAULT_VOLUME;
char* font_path = NULL;
int sound_volume = DEFAULT_VOLUME;
char* swipe_sound_path = SOUND_SWIPE;

void init_config_values(void) {
    window_width = DEFAULT_WINDOW_WIDTH;
    window_height = DEFAULT_WINDOW_HEIGHT;
    volume = DEFAULT_VOLUME;
    sound_volume = DEFAULT_VOLUME;

    if (music_path) free(music_path);
    if (font_path) free(font_path);
    if (swipe_sound_path) free(swipe_sound_path);

    music_path = strdup(DEFAULT_MUSIC_PATH);
    font_path = strdup(DEFAULT_FONT_PATH);
    swipe_sound_path = strdup(SOUND_SWIPE);

    load_config("config.txt");
}

char* read_config(const char *key, const char *filename) {
    FILE *file;
    char buffer[MAX_LINE_LENGTH];
    char *value = NULL;

    file = fopen(filename, "r");
    if (!file) return NULL;

    while (fgets(buffer, sizeof(buffer), file)) {
        buffer[strcspn(buffer, "\n")] = '\0';
        char *token = strtok(buffer, "=");
        if (token && strcmp(token, key) == COMPARE_SUCCESS) {
            token = strtok(NULL, "\n");
            if (token) {
                value = strdup(token);
                break;
            }
        }
    }
    fclose(file);
    return value;
}

void load_config(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return;

    char buffer[MAX_LINE_LENGTH];
    while (fgets(buffer, sizeof(buffer), file)) {
        buffer[strcspn(buffer, "\n")] = '\0';
        char *key = strtok(buffer, "=");
        char *value = strtok(NULL, "\n");

        if (key && value) {
            if (COMPARE_STRINGS(key, "window_width")) {
                int temp = atoi(value);
                if (temp > MIN_WINDOW_WIDTH) window_width = temp;
            }
            else if (COMPARE_STRINGS(key, "window_height")) {
                int temp = atoi(value);
                if (temp > MIN_WINDOW_HEIGHT) window_height = temp;
            }
            else if (COMPARE_STRINGS(key, "volume")) {
                int temp = atoi(value);
                if (temp >= MIN_VOLUME && temp <= MAX_VOLUME) volume = temp;
            }
            else if (COMPARE_STRINGS(key, "sound_volume")) {
                int temp = atoi(value);
                if (temp >= MIN_VOLUME && temp <= MAX_SOUND_VOLUME) {
                    sound_volume = temp;
                    Mix_Volume(-1, sound_volume);
                }
            }
            else if (COMPARE_STRINGS(key, "music_path")) {
                if (music_path) free(music_path);
                music_path = strdup(value);
            }
            else if (COMPARE_STRINGS(key, "font_path")) {
                if (font_path) free(font_path);
                font_path = strdup(value);
            }
            else if (COMPARE_STRINGS(key, "swipe_sound_path")) {
                if (swipe_sound_path) free(swipe_sound_path);
                swipe_sound_path = strdup(value);
            }
        }
    }
    fclose(file);
}

void save_current_config(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) return;

    fprintf(file, "window_width=%d\n", window_width);
    fprintf(file, "window_height=%d\n", window_height);
    fprintf(file, "volume=%d\n", volume);
    fprintf(file, "sound_volume=%d\n", sound_volume);
    if (music_path) fprintf(file, "music_path=%s\n", music_path);
    if (font_path) fprintf(file, "font_path=%s\n", font_path);
    if (swipe_sound_path) fprintf(file, "swipe_sound_path=%s\n", swipe_sound_path);

    fclose(file);
}