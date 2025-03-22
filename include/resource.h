#ifndef RESOURCES_H
#define RESOURCES_H

#include <sqlite3.h>

#define RESOURCE_DEFAULT 50

typedef enum {
    RESOURCE_MAX = 100,
    RESOURCE_MIN = 0
} ResourceLimits;


typedef enum {
    RESOURCE_SUCCESS = 1,
    RESOURCE_FAILURE = 0
} ResourceStatus;

typedef struct {
    int money;
    int population;
    int army;
    int religion;
} Resources;

Resources* create_resources(void);
void free_resources(Resources *res);

void update_resources(Resources *res, int money, int pop, int army, int religion);
ResourceStatus check_resources(const Resources *res);
ResourceStatus save_resources_to_db(sqlite3 *db, const Resources *res);
ResourceStatus load_resources_from_db(sqlite3 *db, Resources *res);

#endif