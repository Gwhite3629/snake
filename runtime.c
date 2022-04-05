#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

#include "snake.h"
#include "bitmap.h"

atomic_bool exit_game = 0;;
atomic_bool wait = 0;;

static void *timer(void *arg)
{
    int fps = *((int*)arg);
    while(!exit_game) {
        usleep((2/fps)*1000000);
        wait = !wait;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int ret = 0;
    int fps = 0;
    board_t board;
    direction_t dir;
    
    pthread_t thr;

    if (argc < 4) {
        printf("Usage: %s <width> <height> <FPS>\n", argv[0]);
        ret = -1;
        goto exit;
    }

    board.width = (uint8_t) atoi(argv[1]);
    board.height = (uint8_t) atoi(argv[2]);
    fps = atoi(argv[3]);

    pthread_create(&thr, NULL, &timer, &fps);

    board.board = (bitmap_t)malloc(MAP_SIZE(board.width)*MAP_SIZE(board.height));
    if (board.board == NULL) {
        perror("Allocation Error");
        ret = -1;
        goto exit;
    }
    memset(board.board, 0, MAP_SIZE(board.width)*MAP_SIZE(board.height));

    printf("Spawning initial food\n");
    spawn_food(&board);

    while(!exit_game) {
        input(&wait, &dir);
        draw_board(board);
    }

exit:
    if (board.board)
        free(board.board);
    return ret;
}