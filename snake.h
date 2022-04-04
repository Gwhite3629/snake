#include <stdint.h>
#include <stdatomic.h>

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
} board_t;

typedef struct node {
    coordinate_t location;
    direction_t direction;
    int key;
    struct node *next;
} node_t;

int insert(node_t *list, coordinate_t location, direction_t dir);

// void delete(node_t *list, int key);

void freeList(node_t *list);

void shift_snake(node_t *snake, board_t *board);

int update(atomic_bool *exit_game, node_t *snake, board_t *board, direction_t dir);

void input(atomic_bool *wait, direction_t *dir);

int check_food(node_t *snake, board_t *board);

int grow_snake(node_t *snake, board_t *board);

int spawn_food(board_t *board);

void check_collision(atomic_bool *exit_game, node_t *snake, board_t *board);

void draw_board(board_t board);