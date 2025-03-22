#ifndef EVENT_H
#define EVENT_H

#include <sqlite3.h>
#include <stdbool.h>
#include "game.h"

#define EVENT_SUCCESS 0
#define EVENT_FAILURE 1

#define COLUMN_ID 0
#define COLUMN_INTITULE 1
#define COLUMN_IMAGE 2
#define COLUMN_MONEY_OUI 3
#define COLUMN_POPULATION_OUI 4
#define COLUMN_ARMY_OUI 5
#define COLUMN_RELIGION_OUI 6
#define COLUMN_MONEY_NON 7
#define COLUMN_POPULATION_NON 8
#define COLUMN_ARMY_NON 9
#define COLUMN_RELIGION_NON 10

#define FIRST_PARAM 1


typedef struct {
    int money;
    int population;
    int army;
    int religion;
} Impact;

typedef struct {
    int id;
    char *intitule;
    char *chemin_image;
    int personnage_id;
    int histoire_id;
    Impact impact_gauche;
    Impact impact_droite;
    int choix;
} Event;

Event* create_event(void);
Event* get_event_by_id(sqlite3* database, int event_id);
Event* get_random_event_by_story(sqlite3* database, int story_id);
bool check_bonus_active(Game *game, Event *evt);
void reset_used_events(sqlite3* database, int story_id);
void free_event(Event *evt);

#endif
