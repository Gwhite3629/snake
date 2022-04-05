#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#include "snake.h"
#include "bitmap.h"

atomic_bool exit_game = 0;;
atomic_bool wait = 0;;

static void *timer(void *arg)
{
    int poll = *(int *)(arg);
    while(!exit_game) {
        usleep(poll);
        wait = !wait;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int ret = 0;
    char c;
    int poll = 0;
    board_t *board = NULL;
    direction_t dir;
    
    pthread_t thr;

    if (argc < 4) {
        printf("Usage: %s <width> <height> <speed>\n\t<speed> S:slow M:medium F:fast\n", argv[0]);
        ret = -1;
        goto exit;
    }

    board = (board_t *)malloc(sizeof(board_t));
    if (board == NULL) {
        perror("Allocation Error");
        ret = -1;
        goto exit;
    }

    board->width = (uint8_t) atoi(argv[1]);
    board->height = (uint8_t) atoi(argv[2]);

    switch (c = argv[3][0]) {
        case 'S':
            poll = 100000;
            break;
        case 'M':
            poll = 75000;
            break;
        case 'F':
            poll = 50000;
            break;
        default:
            printf("Usage: %s <width> <height> <speed>\n\t<speed> S:slow M:medium F:fast\n", argv[0]);
            ret = -1;
            goto exit;
    }

    srand(time(NULL));

    pthread_create(&thr, NULL, &timer, &poll);

    enableRaw();

    board->board = (bitmap_t)malloc(MAP_SIZE((board->width)*(board->height)));
    if (board->board == NULL) {
        perror("Allocation Error");
        ret = -1;
        goto exit;
    }
    memset(board->board, 0, MAP_SIZE((board->width)*(board->height)));

    board->segments = (coordinate_t *)malloc(sizeof(coordinate_t)*(board->width)*(board->height));
    if (board->segments == NULL) {
        perror("Allocation Error");
        ret = -1;
        goto exit;
    }
    memset(board->segments, 0, (board->width)*(board->height));

    board->head.x = (int16_t)((rand())%(board->width));
    board->head.y = (int16_t)((rand())%(board->height));
    board->size = 1;
    board->prev = LEFT;

    set_bit(board->board, ((board->head.y*(board->width))+board->head.x));
    board->segments[0].x = board->head.x;
    board->segments[0].y = board->head.y;

    printf("Spawning initial food\n");
    spawn_food(board);
    printf("Food spawned at (%d,%d)\n", board->food.x, board->food.y);

    while(!exit_game) {
        input(&wait, &dir);
        update(&exit_game, board, dir);
        draw_board(*board);
    }

exit:
    if ((board) && (board->segments))
        free(board->segments);
    if ((board) && (board->board))
        free(board->board);
    if (board)
        free(board);
    return ret;
}