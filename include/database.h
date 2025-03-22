#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include "resource.h"
#include "event.h"


#define DB_PATH "database/reigns.db"
#define DB_NO_PARAMS NULL
#define DB_NO_LIMIT -1 // pas de limite de caractères

#define FIRST_PARAM 1
#define SECOND_PARAM 2
#define THIRD_PARAM 3
#define FOURTH_PARAM 4

#define SQL_LENGTH_AUTO -1

#define COLUMN_DURATION 0
#define DEFAULT_BONUS_DURATION 0


typedef enum {
    DB_SUCCESS = 1,
    DB_FAILURE = 0
} DatabaseStatus;

DatabaseStatus init_database(sqlite3 **db);
void close_database(sqlite3 *db);
Event* get_random_event(sqlite3 *db);
void mark_event_as_used(sqlite3 *db, int event_id);
void save_game_stats(sqlite3 *db, int regne_duree);
int get_bonus_duration(sqlite3* db, const char* bonus_name);

#endif