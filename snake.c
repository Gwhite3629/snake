#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <unistd.h>
#include <termios.h>
#include "snake.h"
#include "bitmap.h"

struct termios def;

void input(atomic_bool *wait, direction_t *dir)
{
    char c;
    while (!(*wait)) {
        read(STDIN_FILENO,&c,1);
        switch(c) {
            case 'w':
                (*dir) = UP;
                break;
            case 's':
                (*dir) = DOWN;
                break;
            case 'd':
                (*dir) = RIGHT;
                break;
            case 'a':
                (*dir) = LEFT;
                break;
            default:
                break;
        }
    }
    while((*wait));
}

void shift_snake(atomic_bool *exit_game, board_t *board, direction_t dir)
{
    switch (dir) {
        case UP:
            if (board->prev == DOWN) dir = DOWN;
            break;
        case DOWN:
            if (board->prev == UP) dir = UP;
            break;
        case RIGHT:
            if (board->prev == LEFT) dir = LEFT;
            break;
        case LEFT:
            if (board->prev == RIGHT) dir = RIGHT;
            break;
    }
    board->prev = dir;

    clear_bit(board->board, (((board->segments[board->size-1].y)*board->width)+board->segments[board->size-1].x));

    switch (dir) {
        case UP:
            if (board->head.y+1 >= board->height) board->head.y = 0;
            else board->head.y++;
            if (get_bit(board->board, (((board->head.y+1)*(board->width))+board->head.x))) (*exit_game) = 1;
            break;
        case DOWN:
            if (board->head.y-1 < 0) board->head.y = board->height-1;
            else board->head.y--;
            if (get_bit(board->board, (((board->head.y-1)*(board->width))+board->head.x))) (*exit_game) = 1;
            break;
        case RIGHT:
            if (board->head.x+1 >= board->width) board->head.x = 0;
            else board->head.x++;
            if (get_bit(board->board, (((board->head.y)*(board->width))+board->head.x+1))) (*exit_game) = 1;
            break;
        case LEFT:
            if (board->head.x-1 < 0) board->head.x = board->width-1;
            else board->head.x--;
            if (get_bit(board->board, (((board->head.y)*(board->width))+board->head.x-1))) (*exit_game) = 1;
            break;
    }

    for (int i = board->size; i > 0; i--) {
        board->segments[i] = board->segments[i - 1];
    }

    board->segments[0].x = board->head.x;
    board->segments[0].y = board->head.y;

    //board->segments[board->size].x = 0;
    //board->segments[board->size].y = 0;

    set_bit(board->board, ((board->head.y*(board->width))+board->head.x));
}

int update(atomic_bool *exit_game, board_t *board, direction_t dir)
{
    int ret = 0;
    shift_snake(exit_game, board, dir);
    ret = check_food(exit_game, board);

    return ret;
}

int check_food(atomic_bool *exit_game, board_t *board)
{
    int ret = 0;
    if (get_bit(board->board, ((board->food.y*(board->width))+board->food.x))) {
        grow_snake(exit_game, board);
        if (ret != 0) goto exit;
        ret = spawn_food(board);
        if (ret != 0) goto exit;
    }

exit:
    return ret;
}

void grow_snake(atomic_bool *exit_game, board_t *board)
{

    board->size++;

    switch (board->prev) {
        case UP:
            if (board->head.y+1 >= board->height) board->head.y = 0;
            else board->head.y++;
            if (get_bit(board->board, (((board->head.y+1)*(board->width))+board->head.x))) (*exit_game) = 1;
            break;
        case DOWN:
            if (board->head.y-1 < 0) board->head.y = board->height-1;
            else board->head.y--;
            if (get_bit(board->board, (((board->head.y-1)*(board->width))+board->head.x))) (*exit_game) = 1;
            break;
        case RIGHT:
            if (board->head.x+1 >= board->width) board->head.x = 0;
            else board->head.x++;
            if (get_bit(board->board, (((board->head.y)*(board->width))+board->head.x+1))) (*exit_game) = 1;
            break;
        case LEFT:
            if (board->head.x-1 < 0) board->head.x = board->width-1;
            else board->head.x--;
            if (get_bit(board->board, (((board->head.y)*(board->width))+board->head.x-1))) (*exit_game) = 1;
            break;
    }

    for (int i = board->size; i > 0; i--) {
        board->segments[i] = board->segments[i - 1];
    }

    set_bit(board->board, ((board->head.y*(board->width))+board->head.x));

    board->segments[0].x = board->head.x;
    board->segments[0].y = board->head.y;
}

int spawn_food(board_t *board)
{
    int ret = 0;
    coordinate_t *clist = NULL;
    int count = 0;

    clist = (coordinate_t *)malloc(sizeof(coordinate_t)*((board->width)*(board->height)));
    if (clist == NULL) {
        perror("Allocation Error");
        ret = -1;
        goto exit;
    }

    for (int i = 0; i < board->width; i++) {
        for (int j = 0; j < board->height; j++) {
            if (!(get_bit(board->board, ((j*(board->width))+i)))) {
                clist[count].x = j;
                clist[count].y = i;
                count++;
            }
        }
    }

    int r = rand()%count;

    board->food.x = clist[r].x;
    board->food.y = clist[r].y;

exit:
    if (clist)
        free(clist);
    return ret;
}

void editorRefreshScreen(void) {
    write(STDOUT_FILENO, "\x1b[2J", 4);
}

void draw_board(board_t board)
{
    editorRefreshScreen();
    char c;
    printf("\n|");
    for (int i = 0; i < board.width; i++) {
        printf("-");
    }
    printf("|\n");
    for (int i = board.width-1; i >= 0; i--) {
        printf("|");
        for (int j = 0; j < board.height; j++) {
            if ((board.food.x == j) & (board.food.y == i))
                c = '0';
            else
                c = (get_bit(board.board, ((i*(board.width))+j))) ? 's': ' ';
            printf("%c", c);
        }
        printf("|\n");
    }
}

void disableRaw(void)
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &def);
}

void enableRaw(void)
{
    tcgetattr(STDIN_FILENO, &def);
    atexit(disableRaw);

    struct termios raw = def;
    raw.c_lflag &= ~(ECHO | ICANON);

    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}