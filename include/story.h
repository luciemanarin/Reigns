#ifndef STORY_H
#define STORY_H

#include <sqlite3.h>
#include "game.h"


typedef enum {
    STORY_SUCCESS,
    STORY_ERROR
} StoryStatus;

void load_story(Game *game, int story_id);
int get_selected_story();
#endif