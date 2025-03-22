#include "resource.h"
#include <stdlib.h>

Resources* create_resources(void) {
    Resources *res = malloc(sizeof(Resources));
    if (res) {
        res->money = RESOURCE_DEFAULT;
        res->population = RESOURCE_DEFAULT;
        res->army = RESOURCE_DEFAULT;
        res->religion = RESOURCE_DEFAULT;
    }
    return res;
}

void free_resources(Resources *res) {
    free(res);
}

void update_resources(Resources *res, int money, int pop, int army, int religion) {
    if (!res) return;
    res->money = money;
    res->population = pop;
    res->army = army;
    res->religion = religion;
}