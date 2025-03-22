#ifndef CONSTANTS_H
#define CONSTANTS_H

#define FRAME_DELAY 16

#define DEFAULT_FONT_PATH "assets/fonts/default.ttf"
#define FONT_PATH "C:/Windows/Fonts/javatext.ttf"
#define DEFAULT_MUSIC_PATH "assets/music/test_music/test1.mp3"
#define SOUND_SWIPE "assets/sounds/swipe.wav"

#define MIN_WINDOW_WIDTH 800
#define MAX_WINDOW_WIDTH 1920
#define MIN_WINDOW_HEIGHT 600
#define MAX_WINDOW_HEIGHT 1080

#define MIN_VOLUME 0
#define MAX_VOLUME 100
#define MAX_SOUND_VOLUME 128

#define COMPARE_STRINGS(a,b) (strcmp(a,b) == 0)

// Valeurs initiales
#define INITIAL_TIME 0
#define INITIAL_MESSAGE ""
#define INITIAL_STATS {0, 0, 0}
#define INITIAL_YEARS 0
#define INITIAL_STORY 0
#define INITIAL_DRAG_STATE 0

#define COORD_ORIGIN 0
#define SCREEN_MARGIN 50
#define LINE_SPACING 30
#define DELAY_TIME 3000

#define CARD_CENTER_X ((window_width - DEFAULT_CARD_SIZE) / 2)
#define CARD_CENTER_Y ((window_height - DEFAULT_CARD_SIZE) / 2)
#define IMAGE_CENTER_X ((window_width - (king_image ? king_image->w : DEFAULT_CARD_SIZE)) / 2)
#define IMAGE_CENTER_Y ((window_height - (king_image ? king_image->h : DEFAULT_CARD_SIZE)) / 2)
#define YEAR_TEXT_X (window_width/2 - 100)
#define YEAR_TEXT_Y (window_height - 100)

#define TIME_DIVIDER 1000.0f
#define ANIMATION_SPEED 12.0f
#define SWIPE_SOUND_THRESHOLD 5
#define BONUS_MESSAGE_DURATION 3000

#define SCREEN_HEIGHT_RATIO 0.15f
#define SCREEN_Y_OFFSET 0.05f
#define IMPACT_Y_OFFSET 25
#define TEXT_Y_OFFSET 40
#define TEXT_VERTICAL_OFFSET 40
#define RESOURCE_Y_RATIO 0.05f
#define IMAGE_VERTICAL_OFFSET 60

#define MESSAGE_RECT_HEIGHT 0
#define TEXT_BUFFER_SIZE 100
#define MESSAGE_BUFFER_SIZE 256

#define COLOR_WHITE (SDL_Color){255, 255, 255, 255}
#define COLOR_RED (SDL_Color){255, 0, 0, 255}
#define COLOR_GREEN (SDL_Color){0, 255, 0, 255}
#define COLOR_GOLD (SDL_Color){255, 215, 0, 255}
#define COLOR_YELLOW (SDL_Color){255, 255, 0, 255}

extern float target_card_x;
extern float current_card_x;
extern float target_card_y;
extern float current_card_y;

typedef enum {
    DEFAULT_WINDOW_WIDTH = 800,
    DEFAULT_WINDOW_HEIGHT = 600
} WindowDimensions;

typedef enum {
    DEFAULT_CARD_SIZE = 300,
    DRAG_THRESHOLD = 100
} CardSettings;

typedef enum {
    FONT_SIZE = 24,
    GAME_OVER_DELAY_MS = 2000,
    DEFAULT_VOLUME = SDL_MIX_MAXVOLUME / 2,
    MAX_LINE_LENGTH = 256
} GameSettings;

typedef enum {
    DRAG_NONE = 0,
    DRAG_ACTIVE = 1
} DragState;

typedef enum {
    INIT_FAILURE = 0,
    INIT_SUCCESS = 1
} InitStatus;

typedef enum {
    DEFAULT_FPS = 60,
    DEFAULT_MUSIC_VOLUME = 128
} AudioSettings;

typedef struct {
    int width;
    int height;
    int fps;
    int volume;
    char music_path[256];
    int sound_volume;

} Config;



#endif