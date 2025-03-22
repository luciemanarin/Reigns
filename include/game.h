#ifndef GAME_H
#define GAME_H

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_mixer.h>
#include <sqlite3.h>
#include <stdbool.h>
#include "resource.h"
#include "constants.h"

#define COLOR_R 0
#define COLOR_G 0
#define COLOR_B 0

#define COLOR_BLACK_R 0
#define COLOR_BLACK_G 0
#define COLOR_BLACK_B 0

#define COLOR_WHITE_R 255
#define COLOR_WHITE_G 255
#define COLOR_WHITE_B 255
#define COLOR_WHITE_A 255

#define COLOR_GOLD_R 255
#define COLOR_GOLD_G 215
#define COLOR_GOLD_B 0
#define COLOR_GOLD_A 255


#define STATS_ID 1
#define STATS_COLUMN_YEARS 0
#define STATS_COLUMN_BEST_REIGN 1
#define STATS_COLUMN_REIGN_COUNT 2
#define STATS_BIND_ANNEES 1
#define STATS_BIND_REGNE 2
#define STATS_BIND_NB_REGNES 3

// interface recommencer
#define STATS_BOX_WIDTH 600
#define STATS_BOX_HEIGHT 300
#define STATS_BOX_MARGIN 300
#define STATS_LEFT_MARGIN 250
#define STATS_TOP 100
#define STATS_LINE_SPACING 50
#define STATS_BOX_WIDTH_DIVISOR 2
#define STATS_BOX_HEIGHT_DIVISOR 2
#define STATS_TEXT_WIDTH_DIVISOR 2
#define STATS_TEXT_HEIGHT_DIVISOR 2

//histoire
#define BUTTON_WIDTH 240
#define BUTTON_HEIGHT 50
#define BORDER_THICKNESS 2 // epaisseur
#define BUTTON_VERTICAL_OFFSET 90
#define STORY_BUTTON_WIDTH 300
#define STORY_BUTTON_HEIGHT 200
#define STORY_BUTTON_PADDING 50
#define STORY_BUTTON_TEXT_OFFSET 50
#define BUTTON_CORNER_RADIUS 15
#define BUTTON_COLOR_VALUE 200
#define BUTTON_ALPHA 255

// Bonus
#define BONUS_SHIELD_VALUE 10 // shield = bouclier
#define BONUS_BALANCE_DIVISOR 2
#define DIVINE_VISION_BONUS 1
#define SHIELD_BONUS 3
#define BALANCE_BONUS 4

// bdd
#define COLUMN_1 0
#define COLUMN_2 1
#define COLUMN_3 2
#define COLUMN_4 3

#define SLIDE_LEFT -10
#define SLIDE_RIGHT 10


typedef struct {
    int annees_total;
    int meilleur_regne;
    int nb_regnes;
} Stats;

typedef enum {
    STAT_ANNEES_TOTAL = 0,
    STAT_MEILLEUR_REGNE = 1,
    STAT_NB_REGNES = 2
} StatType;

typedef enum {
    GAME_QUIT = 0,
    GAME_RUNNING = 1,
    GAME_OVER = 2,
    GAME_STORY_SELECT = 3
} GameState;

typedef struct {
    bool vision_divine_active;
    bool bouclier_active;
    bool equilibre_active;
    int vision_divine_duration;
    int bouclier_duration;
    int equilibre_duration;
} PermanentBonus;

typedef enum {
    CHOICE_LEFT,
    CHOICE_RIGHT
} ChoiceDirection;

typedef struct {
    GameState state;
    Resources resources;
    bool bonus_active;
    SDL_Surface *screen;
    sqlite3 *database;
    Mix_Chunk *swipe_sound;
} Game;

// Variables globales
extern FILE *log_file;
extern Uint32 game_over_start_time;
extern Stats game_stats;
extern int regne_annees;
extern int selected_story;
extern PermanentBonus permanent_bonus;

void initialize_resources(Game *game);
void display_resources(Game *game);
void display_game_over(Game *game, const char *message);
const char* get_game_over_message(sqlite3* db, const char* resource, const char* type);
void apply_consequences(Game *game, ChoiceDirection decision);
void check_game_over_conditions(Game *game);
void handle_game_over_image(Game *game, const char *image_path);
void display_game_over_message(Game *game, const char *message);
void render_story_select(Game *game);
void render_game(Game *game);
void handle_events(Game *game, SDL_Event *event);
void cleanup_game(Game *game);
void reset_game(Game *game);
void render_text(Game *game, const char *text, int x, int y);
void render_text_color(Game *game, const char *text, int x, int y, SDL_Color color);
void save_stats(Game *game);
void load_stats(Game *game);
void display_stats(Game *game);
void handle_game_end(Game *game, bool is_game_over);

#endif