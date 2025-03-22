#include <stdio.h>
#include <sqlite3.h>

#include "story.h"
#include "event.h"
#include "game.h"
#include "card.h"

void load_story(Game *game, int story_id) {
    if (!game || !game->database) return;

    fprintf(stderr, "Chargement de l'histoire %d\n", story_id);

    const char *sql_reset = "UPDATE evenement SET used = 0 WHERE histoire_id = ?";
    sqlite3_stmt *stmt_reset;

    if (sqlite3_prepare_v2(game->database, sql_reset, -1, &stmt_reset, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt_reset, FIRST_PARAM, story_id);
        sqlite3_step(stmt_reset);
        sqlite3_finalize(stmt_reset);

        load_next_card(game->database);
    }
}