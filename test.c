#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#include "snake.h"
#include "bitmap.h"

atomic_bool exit_game = 0;;
atomic_bool wait = 0;;

static void *timer(void *arg)
{
    int fps = *((int*)arg);
    while(!exit_game) {
        usleep((useconds_t)((2/fps)*1000000));
        wait = !wait;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int ret = 0;
    int fps = 0;
    board_t *board;
    node_t *snake;
    direction_t dir;
    
    pthread_t thr;

    if (argc < 4) {
        printf("Usage: %s <width> <height> <FPS>\n", argv[0]);
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
    fps = atoi(argv[3]);

    srand(time(NULL));

    pthread_create(&thr, NULL, &timer, &fps);

    board->board = (bitmap_t)malloc(MAP_SIZE((board->width)*(board->height)));
    if (board->board == NULL) {
        perror("Allocation Error");
        ret = -1;
        goto exit;
    }
    memset(board->board, 0, MAP_SIZE((board->width)*(board->height)));

    snake = (node_t *)malloc(sizeof(node_t));
    if (snake == NULL) {
        perror("Allocation Error");
        ret = -1;
        goto exit;
    }

    snake->direction = UP;
    snake->location.x = (int16_t)((rand())%(board->width));
    snake->location.y = (int16_t)((rand())%(board->height));
    snake->next = NULL;

    printf("Snake start: (%d,%d)\nIndex: %d\n", snake->location.x, snake->location.y, ((snake->location.y*(board->width))+snake->location.x));

    set_bit(board->board, ((snake->location.y*(board->width))+snake->location.x));

    printf("Spawning initial food\n");
    spawn_food(board);
    printf("Food spawned at (%d,%d)\n", board->food.x, board->food.y);

    dir = UP;

    input(&wait, &dir);
    update(&exit_game, snake, board, dir);
    draw_board(*board);

exit:
    if (board->board)
        free(board->board);
    if (board)
        free(board);
    if (snake)
        freeList(snake);
    return ret;
}