#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sqlite3.h>
#include "config.h"
#include "game.h"
#include "init.h"
#include "database.h"
#include "card.h"
#include "resource.h"
#include "image.h"
#include "event.h"
#include "constants.h"
#include "story.h"

FILE *log_file = NULL;

float target_card_x;
float current_card_x;
float target_card_y;
float current_card_y;

Uint32 last_frame_time = INITIAL_TIME;
Uint32 game_over_start_time = INITIAL_TIME;
SDL_Surface *king_image = NULL;
Stats game_stats = INITIAL_STATS;

int regne_annees = INITIAL_YEARS;
int selected_story = INITIAL_STORY;

char bonus_message[MESSAGE_BUFFER_SIZE] = INITIAL_MESSAGE;
Uint32 bonus_message_time = INITIAL_TIME;

PermanentBonus permanent_bonus = {false};

void initialize_resources(Game *game) {
    if (!game) return;

    game->resources.money = RESOURCE_DEFAULT;
    game->resources.population = RESOURCE_DEFAULT;
    game->resources.army = RESOURCE_DEFAULT;
    game->resources.religion = RESOURCE_DEFAULT;

    game_over_start_time = INITIAL_TIME;

    current_card_x = CARD_CENTER_X;
    current_card_y = CARD_CENTER_Y;
    target_card_x = current_card_x; // target = cible
    target_card_y = current_card_y;

    regne_annees = INITIAL_YEARS;
    load_stats(game);
    permanent_bonus.vision_divine_active = false;

    if (king_image) {
        SDL_FreeSurface(king_image);
        king_image = NULL;
    }

    game->swipe_sound = Mix_LoadWAV(SOUND_SWIPE);
    if (game->swipe_sound) {
        Mix_VolumeChunk(game->swipe_sound, DEFAULT_VOLUME);
    }

    display_resources(game);
}


void display_resources(Game *game) {
    if (!game) return;

    SDL_Rect clear_area = {0, 0, window_width, window_height * SCREEN_HEIGHT_RATIO};
    SDL_FillRect(game->screen, &clear_area, SDL_MapRGB(game->screen->format, COLOR_R, COLOR_G, COLOR_B));

    SDL_Color white = COLOR_WHITE;
    char text[TEXT_BUFFER_SIZE];
    int usable_width = window_width - (2 * SCREEN_MARGIN);
    int spacing = usable_width / 4;
    int y = window_height * SCREEN_Y_OFFSET;

    snprintf(text, sizeof(text), "Argent: %d", game->resources.money);
    render_text_color(game, text, SCREEN_MARGIN, y, white);

    snprintf(text, sizeof(text), "Population: %d", game->resources.population);
    render_text_color(game, text, SCREEN_MARGIN + spacing, y, white);

    snprintf(text, sizeof(text), "Armee: %d", game->resources.army);
    render_text_color(game, text, SCREEN_MARGIN + 2*spacing, y, white);

    snprintf(text, sizeof(text), "Religion: %d", game->resources.religion);
    render_text_color(game, text, SCREEN_MARGIN + 3*spacing, y, white);
}


void apply_consequences(Game *game, ChoiceDirection decision) {
    if (!game || !current_card.event) return;

    FILE *log_file = fopen("reigns.log", "a");

    Impact impact = (decision == CHOICE_LEFT) ?
                    current_card.event->impact_gauche :
                    current_card.event->impact_droite;

    if (log_file) {
        fprintf(log_file, "\n=== Action effectuée ===\n");
        fprintf(log_file, "Valeurs initiales - Argent: %d, Population: %d, Armée: %d, Religion: %d\n",
                game->resources.money, game->resources.population,
                game->resources.army, game->resources.religion);
        fprintf(log_file, "Impact prévu - Argent: %d, Population: %d, Armée: %d, Religion: %d\n",
                impact.money, impact.population, impact.army, impact.religion);
    }

    if (permanent_bonus.bouclier_active && permanent_bonus.bouclier_duration > 0) {
        if (log_file) {
            fprintf(log_file, "Bonus Bouclier actif (durée restante: %d)\n",
                    permanent_bonus.bouclier_duration);
        }

        if (impact.money < 0) {
            fprintf(log_file, "Bouclier appliqué sur l'argent: %d -> %d\n",
                    impact.money, BONUS_SHIELD_VALUE);
            impact.money = BONUS_SHIELD_VALUE;
        }

        if (impact.population < 0) {
            fprintf(log_file, "Bouclier appliqué sur la population: %d -> %d\n",
                    impact.population, BONUS_SHIELD_VALUE);
            impact.population = BONUS_SHIELD_VALUE;
        }

        if (impact.army < 0) {
            fprintf(log_file, "Bouclier appliqué sur l'armée: %d -> %d\n",
                    impact.army, BONUS_SHIELD_VALUE);
            impact.army = BONUS_SHIELD_VALUE;
        }

        if (impact.religion < 0) {
            fprintf(log_file, "Bouclier appliqué sur la religion: %d -> %d\n",
                    impact.religion, BONUS_SHIELD_VALUE);
            impact.religion = BONUS_SHIELD_VALUE;
        }


        permanent_bonus.bouclier_duration--;
    }

    if (permanent_bonus.equilibre_active && permanent_bonus.equilibre_duration > 0) {
        if (log_file) {
            fprintf(log_file, "Bonus Equilibre actif (durée restante: %d)\n",
                    permanent_bonus.equilibre_duration);
        }

        if (impact.money < 0) {
            fprintf(log_file, "Equilibre appliqué sur l'argent: %d -> %d\n",
                    impact.money, impact.money / BONUS_BALANCE_DIVISOR);
            impact.money /= BONUS_BALANCE_DIVISOR;
        }

        if (impact.population < 0) {
            fprintf(log_file, "Equilibre appliqué sur la population: %d -> %d\n",
                    impact.population, impact.population / BONUS_BALANCE_DIVISOR);
            impact.population /= BONUS_BALANCE_DIVISOR;
        }

        if (impact.army < 0) {
            fprintf(log_file, "Equilibre appliqué sur l'armée: %d -> %d\n",
                    impact.army, impact.army / BONUS_BALANCE_DIVISOR);
            impact.army /= BONUS_BALANCE_DIVISOR;
        }

        if (impact.religion < 0) {
            fprintf(log_file, "Equilibre appliqué sur la religion: %d -> %d\n",
                    impact.religion, impact.religion / BONUS_BALANCE_DIVISOR);
            impact.religion /= BONUS_BALANCE_DIVISOR;
        }


        permanent_bonus.equilibre_duration--;
    }


    game->resources.money += impact.money;
    game->resources.population += impact.population;
    game->resources.army += impact.army;
    game->resources.religion += impact.religion;

    regne_annees++;

    if (log_file) {
        fprintf(log_file, "Valeurs finales - Argent: %d, Population: %d, Armée: %d, Religion: %d\n",
                game->resources.money, game->resources.population,
                game->resources.army, game->resources.religion);
        fprintf(log_file, "Année de règne: %d\n", regne_annees);
    }

    sqlite3_stmt *stmt_bonus;
    if (sqlite3_prepare_v2(game->database,
                           "SELECT bp.id, bp.nom, bp.duree FROM bonus_permanent bp "
                           "JOIN evenement e ON e.bonus_permanent_id = bp.id WHERE e.id = ?",
                           -1, &stmt_bonus, NULL) == SQLITE_OK) {

        sqlite3_bind_int(stmt_bonus, 1, current_card.event->id);

        if (sqlite3_step(stmt_bonus) == SQLITE_ROW) {
            int bonus_id = sqlite3_column_int(stmt_bonus, 0);
            int duree = sqlite3_column_int(stmt_bonus, 2);
            const char *nom_bonus = (const char *)sqlite3_column_text(stmt_bonus, 1);

            if (log_file) {
                fprintf(log_file, "Nouveau bonus activé: %s (durée: %d)\n",
                        nom_bonus, duree);
            }

            if (bonus_id == DIVINE_VISION_BONUS) {
                permanent_bonus.vision_divine_active = true;
                strcpy(bonus_message, "Vision Divine activee!");
                bonus_message_time = SDL_GetTicks();
            } else if (bonus_id == SHIELD_BONUS ) {
                permanent_bonus.bouclier_active = true;
                permanent_bonus.bouclier_duration = duree;
                strcpy(bonus_message, "Bouclier active!");
                bonus_message_time = SDL_GetTicks();
            } else if (bonus_id == BALANCE_BONUS) {
                permanent_bonus.equilibre_active = true;
                permanent_bonus.equilibre_duration = duree;
                strcpy(bonus_message, "Equilibre active!");
                bonus_message_time = SDL_GetTicks();
            }
        }
        sqlite3_finalize(stmt_bonus);
    }


    if (log_file) {
        fprintf(log_file, "=== Fin de l'action ===\n\n");
        fclose(log_file);
    }

    mark_event_as_used(game->database, current_card.event->id);
    check_game_over_conditions(game);
}


void check_game_over_conditions(Game *game) {
    if (!game) return;

    sqlite3_stmt *stmt;
    const char *sql = NULL;

    if (game->resources.population <= RESOURCE_MIN) {
        sql = "SELECT message, image_path FROM GameOver WHERE resource='population' AND type='low'";
    } else if (game->resources.religion <= RESOURCE_MIN) {
        sql = "SELECT message, image_path FROM GameOver WHERE resource='religion' AND type='low'";
    } else if (game->resources.army <= RESOURCE_MIN) {
        sql = "SELECT message, image_path FROM GameOver WHERE resource='army' AND type='low'";
    } else if (game->resources.money <= RESOURCE_MIN) {
        sql = "SELECT message, image_path FROM GameOver WHERE resource='money' AND type='low'";
    } else if (game->resources.money >= RESOURCE_MAX) {
        sql = "SELECT message, image_path FROM GameOver WHERE resource='money' AND type='high'";
    } else if (game->resources.population >= RESOURCE_MAX) {
        sql = "SELECT message, image_path FROM GameOver WHERE resource='population' AND type='high'";
    } else if (game->resources.army >= RESOURCE_MAX) {
        sql = "SELECT message, image_path FROM GameOver WHERE resource='army' AND type='high'";
    } else if (game->resources.religion >= RESOURCE_MAX) {
        sql = "SELECT message, image_path FROM GameOver WHERE resource='religion' AND type='high'";
    }

    if (!sql) return;

    if (sqlite3_prepare_v2(game->database, sql, -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const char *message = (const char *)sqlite3_column_text(stmt, 0);
            const char *image_path = (const char *)sqlite3_column_text(stmt, 1);

            game_stats.annees_total += regne_annees;
            game_stats.nb_regnes++;

            if (regne_annees > game_stats.meilleur_regne) {
                game_stats.meilleur_regne = regne_annees;
            }

            save_stats(game);
            handle_game_over_image(game, image_path);
            display_game_over_message(game, message);

            current_card.dragging = INITIAL_DRAG_STATE;
            target_card_x = CARD_CENTER_X;
            game->state = GAME_OVER;

            display_stats(game);
            regne_annees = INITIAL_YEARS;
            initialize_resources(game);
            load_next_card(game->database);
        }
        sqlite3_finalize(stmt);
    }
}

void handle_game_over_image(Game *game, const char *image_path) {
    if (king_image) {
        SDL_FreeSurface(king_image);
        king_image = NULL;
    }

    SDL_Surface *temp = IMG_Load(image_path);
    if (temp) {
        float scale = fmin((float)DEFAULT_CARD_SIZE / temp->w, (float)DEFAULT_CARD_SIZE / temp->h);
        int target_width = (int)(temp->w * scale);
        int target_height = (int)(temp->h * scale);

        king_image = SDL_CreateRGBSurface(SDL_SWSURFACE, target_width, target_height,
                                          temp->format->BitsPerPixel,
                                          temp->format->Rmask,
                                          temp->format->Gmask,
                                          temp->format->Bmask,
                                          temp->format->Amask);

        if (king_image) {
            SDL_Rect src = {COORD_ORIGIN, COORD_ORIGIN, temp->w, temp->h};
            SDL_Rect dst = {COORD_ORIGIN, COORD_ORIGIN, target_width, target_height};
            SDL_SoftStretch(temp, &src, king_image, &dst);
        }
        SDL_FreeSurface(temp);
    }

    SDL_Rect dest = {
            IMAGE_CENTER_X,
            IMAGE_CENTER_Y,
            king_image ? king_image->w : DEFAULT_CARD_SIZE,
            king_image ? king_image->h : DEFAULT_CARD_SIZE
    };

    dest.y = ((window_height - (king_image ? king_image->h : DEFAULT_CARD_SIZE))) - IMAGE_VERTICAL_OFFSET;

    SDL_FillRect(game->screen, NULL, SDL_MapRGB(game->screen->format, COLOR_R, COLOR_G, COLOR_B));

    if (king_image) {
        SDL_BlitSurface(king_image, NULL, game->screen, &dest);
    }

    SDL_Rect message_rect = {
            dest.x,
            dest.y,
            dest.w,
            MESSAGE_RECT_HEIGHT
    };

    SDL_FillRect(game->screen, &message_rect, SDL_MapRGB(game->screen->format, COORD_ORIGIN, COORD_ORIGIN, COORD_ORIGIN));
    SDL_Flip(game->screen);
}


void display_game_over_message(Game *game, const char *message) {
    SDL_Rect text_area = {
            COORD_ORIGIN,
            (window_height - DEFAULT_CARD_SIZE) / 2 - TEXT_VERTICAL_OFFSET,
            window_width,
            STORY_BUTTON_HEIGHT
    };

    SDL_FillRect(game->screen, &text_area,
                 SDL_MapRGB(game->screen->format, COLOR_R, COLOR_G, COLOR_B));

    SDL_Color redColor = COLOR_RED;
    char final_message[MESSAGE_BUFFER_SIZE];

    snprintf(final_message, sizeof(final_message),
             "%s\nVotre règne a duré %d ans",
             message, regne_annees - 1);

    char *line = strtok(final_message, "\n");
    int y_offset = (window_height - DEFAULT_CARD_SIZE) / 2 - TEXT_VERTICAL_OFFSET;

    while (line != NULL) {
        SDL_Surface *text_surface = TTF_RenderUTF8_Solid(font, line, redColor);
        if (text_surface) {
            SDL_Rect text_dest = {
                    (window_width - text_surface->w) / 2,
                    y_offset,
                    text_surface->w,
                    text_surface->h
            };
            SDL_BlitSurface(text_surface, NULL, game->screen, &text_dest);
            SDL_FreeSurface(text_surface);
            y_offset += LINE_SPACING;
        }
        line = strtok(NULL, "\n");
    }

    SDL_Flip(game->screen);
    SDL_Delay(DELAY_TIME);
}

void render_story_select(Game *game) {
    fprintf(stderr, "Entrée dans render_story_select\n");
    fprintf(stderr, "État du jeu: %d\n", game->state);

    if (!game || !game->screen) {
        return;
    }

    SDL_FillRect(game->screen, NULL, SDL_MapRGB(game->screen->format, COLOR_R, COLOR_G, COLOR_B));

    int x1 = (window_width - 2 * STORY_BUTTON_WIDTH - STORY_BUTTON_PADDING) / 2;
    int x2 = x1 + STORY_BUTTON_WIDTH + STORY_BUTTON_PADDING;
    int y = (window_height - STORY_BUTTON_HEIGHT) / 2;

    SDL_Rect button1 = {x1, y, STORY_BUTTON_WIDTH, STORY_BUTTON_HEIGHT};
    SDL_Rect button2 = {x2, y, STORY_BUTTON_WIDTH, STORY_BUTTON_HEIGHT};

    roundedBoxRGBA(game->screen, x1, y, x1 + STORY_BUTTON_WIDTH, y + STORY_BUTTON_HEIGHT,
                   BUTTON_CORNER_RADIUS,
                   BUTTON_COLOR_VALUE, BUTTON_COLOR_VALUE, BUTTON_COLOR_VALUE, BUTTON_ALPHA);

    roundedBoxRGBA(game->screen, x2, y, x2 + STORY_BUTTON_WIDTH, y + STORY_BUTTON_HEIGHT,
                   BUTTON_CORNER_RADIUS,
                   BUTTON_COLOR_VALUE, BUTTON_COLOR_VALUE, BUTTON_COLOR_VALUE, BUTTON_ALPHA);

    SDL_Color white = COLOR_WHITE;
    fprintf(stderr, "Ajout du texte\n");

    render_text_color(game, "Histoire 1",
                      x1 + STORY_BUTTON_WIDTH/2 - STORY_BUTTON_TEXT_OFFSET,
                      y + STORY_BUTTON_HEIGHT/2, white);

    render_text_color(game, "Histoire 2",
                      x2 + STORY_BUTTON_WIDTH/2 - STORY_BUTTON_TEXT_OFFSET,
                      y + STORY_BUTTON_HEIGHT/2, white);

    fprintf(stderr, "Mise à jour de l'écran\n");
    SDL_Flip(game->screen);
    fprintf(stderr, "Fin render_story_select\n");
}


void render_game(Game *game) {
    fprintf(stderr, "Entrée dans render_game\n");
    if (!game) {
        fprintf(stderr, "Error: game est NULL\n");
        return;
    }
    fprintf(stderr, "État du jeu: %d\n", game->state);

    if (game->state == GAME_STORY_SELECT) {
        render_story_select(game);
        return;
    }

    if (game->state == GAME_OVER) {
        SDL_FillRect(game->screen, NULL, SDL_MapRGB(game->screen->format, COLOR_R, COLOR_G, COLOR_B));

        if (king_image) {
            SDL_Rect dest;
            dest.w = DEFAULT_CARD_SIZE;
            dest.h = DEFAULT_CARD_SIZE;
            dest.x = (window_width - dest.w) / 2;
            dest.y = (window_height - dest.h) / 2;

            SDL_BlitSurface(king_image, NULL, game->screen, &dest);
        }

        SDL_Flip(game->screen);
        return;
    }

    Uint32 current_time = SDL_GetTicks();
    float delta_time = (current_time - last_frame_time) / TIME_DIVIDER;
    last_frame_time = current_time;
    float dx = (target_card_x - current_card_x) * ANIMATION_SPEED * delta_time;
    current_card_x += dx;

    float center_x = (window_width - DEFAULT_CARD_SIZE) / 2;
    float card_offset = current_card_x - center_x;

    SDL_FillRect(game->screen, NULL, SDL_MapRGB(game->screen->format, COLOR_R, COLOR_G, COLOR_B));

    SDL_Rect card_dest = {
            (int)current_card_x,
            (window_height - DEFAULT_CARD_SIZE) / 2,
            DEFAULT_CARD_SIZE,
            DEFAULT_CARD_SIZE
    };

    if (current_card.image) {
        SDL_SoftStretch(current_card.image, NULL, game->screen, &card_dest);
    }

    SDL_Color white = COLOR_WHITE;
    SDL_Color red = COLOR_RED;
    SDL_Color green = COLOR_GREEN;

    if (current_card.event && current_card.event->intitule) {
        int text_y = card_dest.y - TEXT_Y_OFFSET;
        int text_width;
        TTF_SizeText(font, current_card.event->intitule, &text_width, NULL);
        int text_x = (window_width - text_width) / 2;
        render_text_color(game, current_card.event->intitule, text_x, text_y, white);
    }

    char text[TEXT_BUFFER_SIZE];
    int margin = 50;
    int spacing = (window_width - 2*margin) / 4;
    int y = window_height * RESOURCE_Y_RATIO;

    if (current_card.dragging && permanent_bonus.vision_divine_active && current_card.event) {
        if (card_offset < SLIDE_LEFT) {
            snprintf(text, sizeof(text), "Argent: %d", game->resources.money + current_card.event->impact_gauche.money);
            render_text_color(game, text, margin, y, current_card.event->impact_gauche.money < 0 ? red : green);

            snprintf(text, sizeof(text), "Population: %d", game->resources.population + current_card.event->impact_gauche.population);
            render_text_color(game, text, margin + spacing, y, current_card.event->impact_gauche.population < 0 ? red : green);

            snprintf(text, sizeof(text), "Armee: %d", game->resources.army + current_card.event->impact_gauche.army);
            render_text_color(game, text, margin + 2*spacing, y, current_card.event->impact_gauche.army < 0 ? red : green);

            snprintf(text, sizeof(text), "Religion: %d", game->resources.religion + current_card.event->impact_gauche.religion);
            render_text_color(game, text, margin + 3*spacing, y, current_card.event->impact_gauche.religion < 0 ? red : green);
        }
        else if (card_offset > SLIDE_RIGHT) {
            snprintf(text, sizeof(text), "Argent: %d", game->resources.money + current_card.event->impact_droite.money);
            render_text_color(game, text, margin, y, current_card.event->impact_droite.money < 0 ? red : green);

            snprintf(text, sizeof(text), "Population: %d", game->resources.population + current_card.event->impact_droite.population);
            render_text_color(game, text, margin + spacing, y, current_card.event->impact_droite.population < 0 ? red : green);

            snprintf(text, sizeof(text), "Armee: %d", game->resources.army + current_card.event->impact_droite.army);
            render_text_color(game, text, margin + 2*spacing, y, current_card.event->impact_droite.army < 0 ? red : green);

            snprintf(text, sizeof(text), "Religion: %d", game->resources.religion + current_card.event->impact_droite.religion);
            render_text_color(game, text, margin + 3*spacing, y, current_card.event->impact_droite.religion < 0 ? red : green);
        }
    }
    else {

        snprintf(text, sizeof(text), "Argent: %d", game->resources.money);
        render_text_color(game, text, margin, y, white);

        snprintf(text, sizeof(text), "Population: %d", game->resources.population);
        render_text_color(game, text, margin + spacing, y, white);

        snprintf(text, sizeof(text), "Armee: %d", game->resources.army);
        render_text_color(game, text, margin + 2*spacing, y, white);

        snprintf(text, sizeof(text), "Religion: %d", game->resources.religion);
        render_text_color(game, text, margin + 3*spacing, y, white);
    }


    char year_text[TEXT_BUFFER_SIZE];
    SDL_Color gold = COLOR_GOLD;
    snprintf(year_text, sizeof(year_text), "An %d de votre regne", regne_annees);
    render_text_color(game, year_text,
                      YEAR_TEXT_X,
                      YEAR_TEXT_Y,
                      gold);

    int button_y = (window_height + DEFAULT_CARD_SIZE) / 2 + 20;
    render_text_color(game, "NON", left_area.x, button_y, white);
    render_text_color(game, "OUI", right_area.x, button_y, white);

    if (strlen(bonus_message) > 0 &&
        (permanent_bonus.vision_divine_active ||
         permanent_bonus.bouclier_active ||
         permanent_bonus.equilibre_active)) {
        Uint32 current_time = SDL_GetTicks();
        if (current_time - bonus_message_time < BONUS_MESSAGE_DURATION) {
            SDL_Color yellow = COLOR_YELLOW;

            int bonus_message_y = window_height - 70;

            int text_width;
            TTF_SizeText(font, bonus_message, &text_width, NULL);
            int bonus_message_x = (window_width - text_width) / 2;

            render_text_color(game, bonus_message,
                              bonus_message_x,
                              bonus_message_y,
                              yellow);
        } else {
            bonus_message[0] = '\0';
        }
    }

    SDL_Flip(game->screen);
}

        void handle_events(Game *game, SDL_Event *event) {
            if (!game || !event) return;

            switch (event->type) {
                case SDL_QUIT:
                    game->state = GAME_QUIT;
                    break;

                case SDL_MOUSEMOTION:
                    if (game->state != GAME_OVER && current_card.dragging) {
                        target_card_x = event->motion.x - DEFAULT_CARD_SIZE / 2;
                        target_card_y = (window_height - DEFAULT_CARD_SIZE) / 2;
                        if (abs(target_card_x - current_card_x) > SWIPE_SOUND_THRESHOLD) { // pour eviter que le son ne se repete ds des petites variations
                            Mix_PlayChannel(-1, game->swipe_sound, 0);
                        }
                    }
                    break;

                case SDL_MOUSEBUTTONDOWN:
                    if (event->button.button == SDL_BUTTON_LEFT && game->state != GAME_OVER) {
                        int mouse_x = event->button.x;
                        int mouse_y = event->button.y;

                        if (game->state == GAME_STORY_SELECT) {

                            int button_width = 300;
                            int button_height = 200;
                            int padding = 50;
                            int x1 = (window_width - 2 * button_width - padding) / 2;
                            int x2 = x1 + button_width + padding;
                            int y = (window_height - button_height) / 2;

                            if (mouse_x >= x1 && mouse_x <= x1 + button_width &&
                                mouse_y >= y && mouse_y <= y + button_height) {
                                game->state = GAME_RUNNING;

                                fprintf(stderr, "Clic sur Histoire 1\n");
                                selected_story = 1;
                                load_story(game, selected_story);
                            }
                            else if (mouse_x >= x2 && mouse_x <= x2 + button_width &&
                                     mouse_y >= y && mouse_y <= y + button_height) {
                                game->state = GAME_RUNNING;
                                selected_story = 2;
                                load_story(game, selected_story);                    }
                        }
                        else {
                            SDL_Rect card_rect = {
                                    (int)current_card_x,
                                    (int)current_card_y,
                                    DEFAULT_CARD_SIZE,
                                    DEFAULT_CARD_SIZE
                            };
                            if (mouse_x >= card_rect.x && mouse_x < card_rect.x + card_rect.w &&
                                mouse_y >= card_rect.y && mouse_y < card_rect.y + card_rect.h) {
                                current_card.dragging = 1;
                                current_card.start_x = mouse_x - (int)current_card_x;
                                current_card.card_start_x = (int)current_card_x;
                                Mix_PlayChannel(-1, game->swipe_sound, 0);
                            }
                        }
                    }
                    break;



                case SDL_MOUSEBUTTONUP:
                    if (current_card.dragging && game->state != GAME_OVER) {
                        current_card.dragging = 0;
                        float dx = target_card_x - ((window_width - DEFAULT_CARD_SIZE) / 2);

                        if (dx < -DRAG_THRESHOLD) {
                            apply_consequences(game, CHOICE_LEFT);
                            load_next_card(game->database);
                        } else if (dx > DRAG_THRESHOLD) {
                            apply_consequences(game, CHOICE_RIGHT);
                            load_next_card(game->database);
                        }

                        target_card_x = (window_width - DEFAULT_CARD_SIZE) / 2;
                        target_card_y = (window_height - DEFAULT_CARD_SIZE) / 2;
                    }
                    break;
            }
        }

        void save_stats(Game *game) {
            sqlite3_stmt *stmt;
            int current_total = 0;

            const char *sql_get = "SELECT annees_total FROM stats WHERE id = 1";
            if (sqlite3_prepare_v2(game->database, sql_get, -1, &stmt, NULL) == SQLITE_OK) {
                if (sqlite3_step(stmt) == SQLITE_ROW) {
                    current_total = sqlite3_column_int(stmt, COLUMN_1); // colonne 1 = annees_total
                }
                sqlite3_finalize(stmt);
            }

            game_stats.annees_total = current_total + (regne_annees - 1);

            const char *sql = "INSERT OR REPLACE INTO stats (id, annees_total, meilleur_regne, nb_regnes) VALUES (1, ?, ?, ?)";
            if (sqlite3_prepare_v2(game->database, sql, -1, &stmt, NULL) == SQLITE_OK) {
                sqlite3_bind_int(stmt, COLUMN_2, game_stats.annees_total);
                sqlite3_bind_int(stmt, COLUMN_3, game_stats.meilleur_regne);
                sqlite3_bind_int(stmt, COLUMN_4, game_stats.nb_regnes);
                sqlite3_step(stmt);
                sqlite3_finalize(stmt);
            }
        }


        void load_stats(Game *game) {
            sqlite3_stmt *stmt;
            int has_stats = 0;

            const char *sql_check = "SELECT COUNT(*) FROM stats WHERE id = 1";
            if (sqlite3_prepare_v2(game->database, sql_check, -1, &stmt, NULL) == SQLITE_OK) { // jusqu'à caractère nul
                if (sqlite3_step(stmt) == SQLITE_ROW) {
                    has_stats = sqlite3_column_int(stmt, COLUMN_1);
                }
                sqlite3_finalize(stmt);
            }

            if (!has_stats) {
                const char *sql_init = "INSERT INTO stats (id, annees_total, meilleur_regne, nb_regnes) VALUES (1, 0, 0, 0)";
                sqlite3_exec(game->database, sql_init, NULL, NULL, NULL);
            }

            const char *sql = "SELECT annees_total, meilleur_regne, nb_regnes FROM stats WHERE id = 1";
            if (sqlite3_prepare_v2(game->database, sql, -1, &stmt, NULL) == SQLITE_OK) {
                if (sqlite3_step(stmt) == SQLITE_ROW) {
                    game_stats.annees_total = sqlite3_column_int(stmt, COLUMN_1);
                    game_stats.meilleur_regne = sqlite3_column_int(stmt, COLUMN_2);
                    game_stats.nb_regnes = sqlite3_column_int(stmt, COLUMN_3);
                }
                sqlite3_finalize(stmt);
            }
        }

void display_stats(Game *game) {
    SDL_FillRect(game->screen, NULL, SDL_MapRGB(game->screen->format, COLOR_BLACK_R, COLOR_BLACK_G, COLOR_BLACK_B));

    SDL_Color gold = {COLOR_GOLD_R, COLOR_GOLD_G, COLOR_GOLD_B, COLOR_GOLD_A};
    SDL_Color white = {COLOR_WHITE_R, COLOR_WHITE_G, COLOR_WHITE_B, COLOR_WHITE_A};

    SDL_Rect statsBox = {
            window_width/STATS_BOX_WIDTH_DIVISOR - STATS_BOX_MARGIN,
            window_height/STATS_BOX_HEIGHT_DIVISOR - STATS_BOX_MARGIN,
            STATS_BOX_WIDTH,
            STATS_BOX_HEIGHT
    };

    SDL_FillRect(game->screen, &statsBox, SDL_MapRGB(game->screen->format, COLOR_BLACK_R, COLOR_BLACK_G, COLOR_BLACK_B));

    char stats_text[TEXT_BUFFER_SIZE];

    snprintf(stats_text, sizeof(stats_text), "Total des annees de regne: %d", game_stats.annees_total);
    render_text_color(game, stats_text,
                      window_width/STATS_TEXT_WIDTH_DIVISOR - STATS_LEFT_MARGIN,
                      window_height/STATS_TEXT_HEIGHT_DIVISOR - STATS_TOP,
                      gold);

    snprintf(stats_text, sizeof(stats_text), "Plus long regne: %d ans", game_stats.meilleur_regne -1);
    render_text_color(game, stats_text,
                      window_width/STATS_TEXT_WIDTH_DIVISOR - STATS_LEFT_MARGIN,
                      window_height/STATS_TEXT_HEIGHT_DIVISOR - STATS_TOP + STATS_LINE_SPACING,
                      white);

    snprintf(stats_text, sizeof(stats_text), "Nombre de regnes: %d", game_stats.nb_regnes);
    render_text_color(game, stats_text,
                      window_width/STATS_TEXT_WIDTH_DIVISOR - STATS_LEFT_MARGIN,
                      window_height/STATS_TEXT_HEIGHT_DIVISOR - STATS_TOP + (2 * STATS_LINE_SPACING),
                      white);

    SDL_Flip(game->screen);

    SDL_Rect button_bg = {
            window_width/2 - BUTTON_WIDTH/2,
            window_height/2 + BUTTON_VERTICAL_OFFSET,
            BUTTON_WIDTH,
            BUTTON_HEIGHT
    };

    SDL_FillRect(game->screen, &button_bg, SDL_MapRGB(game->screen->format, COLOR_BLACK_R, COLOR_BLACK_G, COLOR_BLACK_B));

    SDL_Rect border = button_bg;
    border.h = BORDER_THICKNESS;
    SDL_FillRect(game->screen, &border, SDL_MapRGB(game->screen->format, COLOR_WHITE_R, COLOR_WHITE_G, COLOR_WHITE_B));

    border.y = button_bg.y + button_bg.h - BORDER_THICKNESS;
    SDL_FillRect(game->screen, &border, SDL_MapRGB(game->screen->format, COLOR_WHITE_R, COLOR_WHITE_G, COLOR_WHITE_B));

    border = button_bg;
    border.w = BORDER_THICKNESS;
    SDL_FillRect(game->screen, &border, SDL_MapRGB(game->screen->format, COLOR_WHITE_R, COLOR_WHITE_G, COLOR_WHITE_B));

    border.x = button_bg.x + button_bg.w - BORDER_THICKNESS;
    SDL_FillRect(game->screen, &border, SDL_MapRGB(game->screen->format, COLOR_WHITE_R, COLOR_WHITE_G, COLOR_WHITE_B));

    int text_width;
    int text_height;
    TTF_SizeText(font, "RECOMMENCER", &text_width, &text_height);

    render_text_color(game, "RECOMMENCER",
                      button_bg.x + (BUTTON_WIDTH - text_width)/2,
                      button_bg.y + (BUTTON_HEIGHT - text_height)/2,
                      white);

    SDL_Flip(game->screen);

    SDL_Event event;
    int waiting_for_click = 1;
    while (waiting_for_click) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouse_x = event.button.x;
                int mouse_y = event.button.y;
                if (mouse_x >= button_bg.x && mouse_x <= button_bg.x + button_bg.w &&
                    mouse_y >= button_bg.y && mouse_y <= button_bg.y + button_bg.h) {
                    regne_annees = INITIAL_YEARS;
                    game->state = GAME_STORY_SELECT;
                    initialize_resources(game);
                    waiting_for_click = 0;
                }
            }
            else if (event.type == SDL_QUIT) {
                game->state = GAME_QUIT;
                waiting_for_click = 0;
            }
        }
        SDL_Delay(FRAME_DELAY);
    }
}

        void cleanup_game(Game *game) {
            if (log_file) {
                fclose(log_file);
                log_file = NULL;
            }

            if (king_image) {
                SDL_FreeSurface(king_image);
                king_image = NULL;
            }

            if (game) {
                if (game->swipe_sound) {
                    Mix_FreeChunk(game->swipe_sound);
                    game->swipe_sound = NULL;
                }

                if (game->screen) {
                    SDL_FreeSurface(game->screen);
                    game->screen = NULL;
                }

                if (game->database) {
                    sqlite3_close(game->database);
                    game->database = NULL;
                }
            }
        }