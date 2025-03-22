#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "database.h"
#include "event.h"

typedef enum {
    COL_ID = 0,
    COL_INTITULE,
    COL_IMAGE,
    COL_PERSO,
    COL_MONEY_OUI,
    COL_POP_OUI,
    COL_ARMY_OUI,
    COL_REL_OUI,
    COL_MONEY_NON,
    COL_POP_NON,
    COL_ARMY_NON,
    COL_REL_NON
} TableColumns;

extern FILE *log_file;

DatabaseStatus init_database(sqlite3 **db) {
    if (!db) {
        fprintf(stderr, "Pointeur de base de données invalide\n");
        return EVENT_FAILURE;
    }

    if (sqlite3_open(DB_PATH, db) != SQLITE_OK) {
        fprintf(stderr, "Erreur connexion : %s\n", sqlite3_errmsg(*db));
        return EVENT_FAILURE;
    }

    return EVENT_SUCCESS;
}

void close_database(sqlite3 *db) {
    if (db) {
        sqlite3_close(db);
    }
}


ResourceStatus save_resources_to_db(sqlite3 *db, const Resources *res) {
    if (!db || !res) {
        return RESOURCE_FAILURE;
    }

    sqlite3_stmt *stmt;
    const char *sql = "UPDATE Resources SET "
                      "money = ?, population = ?, army = ?, religion = ? "
                      "WHERE id = 1;";

    if (sqlite3_prepare_v2(db, sql, DB_NO_LIMIT, &stmt, DB_NO_PARAMS) != SQLITE_OK) {
        return RESOURCE_FAILURE;
    }



    if (sqlite3_bind_int(stmt, FIRST_PARAM, res->money) != SQLITE_OK || // 1 car money est le premier paramètre
        sqlite3_bind_int(stmt, SECOND_PARAM, res->population) != SQLITE_OK ||
        sqlite3_bind_int(stmt, THIRD_PARAM, res->army) != SQLITE_OK ||
        sqlite3_bind_int(stmt, FOURTH_PARAM, res->religion) != SQLITE_OK) {

        sqlite3_finalize(stmt);
        return RESOURCE_FAILURE;
    }

    int rc = sqlite3_step(stmt); // rc = return code
    sqlite3_finalize(stmt);

    return (rc == SQLITE_DONE) ? RESOURCE_SUCCESS : RESOURCE_FAILURE; // rc ok = success, sinon failure
}

Event* get_random_event(sqlite3 *db) {
    if (!db) {
        fprintf(log_file ? log_file : stderr, "Erreur: db est NULL\n");
        return NULL;
    }

    Event *evt = create_event();
    if (!evt) {
        fprintf(log_file ? log_file : stderr, "Erreur: échec création événement\n");
        return NULL;
    }

    sqlite3_stmt *stmt;
    const char *sql = "SELECT Id, Intitule, Chemin_Image, Personnage_Id, "
                      "Money_Oui, Population_Oui, Army_Oui, Religion_Oui, "
                      "Money_Non, Population_Non, Army_Non, Religion_Non "
                      "FROM Evenement ORDER BY RANDOM() LIMIT 1;";

    if (sqlite3_prepare_v2(db, sql, DB_NO_LIMIT, &stmt, DB_NO_PARAMS) != SQLITE_OK) {
        free_event(evt);
        return NULL;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        evt->id = sqlite3_column_int(stmt, COL_ID);
        evt->intitule = strdup((const char*)sqlite3_column_text(stmt, COL_INTITULE)); // strdup copie le texte de l'intitulé
        evt->chemin_image = strdup((const char*)sqlite3_column_text(stmt, COL_IMAGE));
        evt->personnage_id = sqlite3_column_int(stmt, COL_PERSO);

        evt->impact_gauche.money = sqlite3_column_int(stmt, COL_MONEY_OUI);
        evt->impact_gauche.population = sqlite3_column_int(stmt, COL_POP_OUI);
        evt->impact_gauche.army = sqlite3_column_int(stmt, COL_ARMY_OUI);
        evt->impact_gauche.religion = sqlite3_column_int(stmt, COL_REL_OUI);

        evt->impact_droite.money = sqlite3_column_int(stmt, COL_MONEY_NON);
        evt->impact_droite.population = sqlite3_column_int(stmt, COL_POP_NON);
        evt->impact_droite.army = sqlite3_column_int(stmt, COL_ARMY_NON);
        evt->impact_droite.religion = sqlite3_column_int(stmt, COL_REL_NON);

        sqlite3_finalize(stmt);
        return evt;
    }

    sqlite3_finalize(stmt);
    free_event(evt);
    return NULL;
}

void mark_event_as_used(sqlite3 *db, int event_id) {
    const char *sql = "UPDATE evenement SET used = 1 WHERE id = ?";
    sqlite3_stmt *stmt;


    if (sqlite3_prepare_v2(db, sql, SQL_LENGTH_AUTO, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_int(stmt, FIRST_PARAM, event_id);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
}

int get_bonus_duration(sqlite3* db, const char* bonus_name) {
    sqlite3_stmt* stmt;
    int duration = DEFAULT_BONUS_DURATION;
    const char* query = "SELECT duration FROM permanent_bonus WHERE name = ?";

    if (sqlite3_prepare_v2(db, query, SQL_LENGTH_AUTO, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, FIRST_PARAM, bonus_name, SQL_LENGTH_AUTO, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            duration = sqlite3_column_int(stmt, COLUMN_DURATION);
        }
        sqlite3_finalize(stmt);
    }
    return duration;
}
