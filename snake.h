#include <stdint.h>
#include <stdatomic.h>
#include <stdbool.h>

typedef enum {UP,DOWN,RIGHT,LEFT} direction_t;
typedef char* bitmap_t;

typedef struct coordinate {
    int16_t x;
    int16_t y;
} coordinate_t;

typedef struct board {
    bitmap_t board;
    uint8_t width;
    uint8_t height;
    coordinate_t food;
    coordinate_t head;
    coordinate_t *segments;
    uint16_t size;
    direction_t prev;
} board_t;

void shift_snake(atomic_bool *exit_game, board_t *board, direction_t dir);

int update(atomic_bool *exit_game, board_t *board, direction_t dir);

void input(atomic_bool *wait, direction_t *dir);

int check_food(atomic_bool *exit_game, board_t *board);

void grow_snake(atomic_bool *exit_game, board_t *board);

int spawn_food(board_t *board);

void draw_board(board_t board);

void editorRefreshScreen(void);

void disableRaw(void);

void enableRaw(void);