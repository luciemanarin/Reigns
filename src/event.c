#include "event.h"
#include "game.h"
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <stdbool.h>

Event* create_event(void) {
    Event *evt = malloc(sizeof(Event));
    if (evt) {
        evt->id = 0;
        evt->histoire_id = 0;
        evt->personnage_id = 0;
        evt->intitule = NULL;
        evt->chemin_image = NULL;
        evt->choix = 1;

        evt->impact_gauche.money = 0;
        evt->impact_gauche.population = 0;
        evt->impact_gauche.army = 0;
        evt->impact_gauche.religion = 0;
        evt->impact_droite.money = 0;
        evt->impact_droite.population = 0;
        evt->impact_droite.army = 0;
        evt->impact_droite.religion = 0;
    }
    return evt;
}

Event* get_event_by_id(sqlite3* database, int event_id) {
    const char* sql = "SELECT id, intitule, chemin_image, personnage_id, "
                      "money_oui, population_oui, army_oui, religion_oui, "
                      "histoire_id, money_non, population_non, army_non, religion_non, "
                      "bonus_permanent_id, used "
                      "FROM evenement WHERE id = ?";

    sqlite3_stmt* stmt;
    Event* event = NULL;

    if (sqlite3_prepare_v2(database, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, event_id);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            event = malloc(sizeof(Event));
            if (event) {
                event->id = sqlite3_column_int(stmt, COLUMN_ID);
                event->intitule = strdup((const char*)sqlite3_column_text(stmt, COLUMN_INTITULE));
                event->chemin_image = strdup((const char*)sqlite3_column_text(stmt, COLUMN_IMAGE));

                event->impact_gauche.money = sqlite3_column_int(stmt, COLUMN_MONEY_OUI);
                event->impact_gauche.population = sqlite3_column_int(stmt, COLUMN_POPULATION_OUI);
                event->impact_gauche.army = sqlite3_column_int(stmt, COLUMN_ARMY_OUI);
                event->impact_gauche.religion = sqlite3_column_int(stmt, COLUMN_RELIGION_OUI);

                event->impact_droite.money = sqlite3_column_int(stmt, COLUMN_MONEY_NON);
                event->impact_droite.population = sqlite3_column_int(stmt, COLUMN_POPULATION_NON);
                event->impact_droite.army = sqlite3_column_int(stmt, COLUMN_ARMY_NON);
                event->impact_droite.religion = sqlite3_column_int(stmt, COLUMN_RELIGION_NON);
            }
        }
        sqlite3_finalize(stmt);
    }
    return event;
}

Event* get_random_event_by_story(sqlite3* database, int story_id) {
    const char* sql = "SELECT id FROM evenement WHERE used = 0 AND histoire_id = ? ORDER BY RANDOM() LIMIT 1";
    sqlite3_stmt* stmt;
    Event* event = NULL;

    if (sqlite3_prepare_v2(database, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, FIRST_PARAM, story_id); // 1 pour remplacer le premier paramètre "?"
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            int event_id = sqlite3_column_int(stmt, COLUMN_ID);
            event = get_event_by_id(database, event_id);
        } else {
            reset_used_events(database, story_id);
            sqlite3_reset(stmt);
            if (sqlite3_step(stmt) == SQLITE_ROW) {
                int event_id = sqlite3_column_int(stmt, COLUMN_ID);
                event = get_event_by_id(database, event_id);
            }
        }
        sqlite3_finalize(stmt);
    }
    return event;
}

bool check_bonus_active(Game *game, Event *evt) {
    if (!game || !evt) {
        fprintf(log_file, "Erreur: parametres NULL dans check_bonus_active\n");
        return false;
    }

    sqlite3_stmt *stmt;
    const char *sql = "SELECT bp.id FROM Bonus_Permanent bp "
                      "JOIN Histoire h ON h.bonus_permanent_id = bp.id "
                      "WHERE h.id = ? AND bp.nom = 'Vision Divine'";

    if (sqlite3_prepare_v2(game->database, sql, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(log_file, "Erreur SQL dans check_bonus_active: %s\n", sqlite3_errmsg(game->database));
        return false;
    }

    sqlite3_bind_int(stmt, FIRST_PARAM, evt->histoire_id);
    bool has_bonus = false;

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        has_bonus = true;
        fprintf(log_file, "Bonus actif trouve pour l'evenement %d\n", evt->histoire_id);
    }

    sqlite3_finalize(stmt);
    return has_bonus;
}

void reset_used_events(sqlite3* database, int story_id) {
    const char* sql = "UPDATE evenement SET used = 0 WHERE histoire_id = ?";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(database, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, FIRST_PARAM, story_id);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
}

void free_event(Event *evt) {
    if (evt) {
        if (evt->intitule) free(evt->intitule);
        if (evt->chemin_image) free(evt->chemin_image);
        free(evt);
    }
}