#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <unistd.h>
#include <termios.h>
#include "snake.h"
#include "bitmap.h"

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
}

void shift_snake(node_t *snake, board_t *board)
{
    node_t *p = snake;
    direction_t d = snake->direction;
    while (p) {
        p->direction = d;
        clear_bit(board->board,((p->location.y*(board->width))+p->location.x));
        switch(d = p->direction) {
            case UP:
                p->location.y++;
                if (p->location.y >= board->height)
                    p->location.y = 0;
                break;
            case DOWN:
                p->location.y--;
                if (p->location.y < 0)
                    p->location.y = board->height-1;
                break;
            case RIGHT:
                p->location.x++;
                if (p->location.x >= board->width)
                    p->location.x = 0;
                break;
            case LEFT:
                p->location.x--;
                if (p->location.y < 0)
                    p->location.x = board->width-1;
                break;
        }
        set_bit(board->board,((p->location.y*(board->width))+p->location.x));
        p = p->next;
    }
    clear_bit(board->board, ((snake->location.y*(board->width))+snake->location.x)); // Clear head bit
}

int update(atomic_bool *exit_game, node_t *snake, board_t *board, direction_t dir)
{
    int ret = 0;

    switch (dir) {
        case UP:
            if (snake->direction == DOWN) break;
            snake->direction = dir;
            break;
        case DOWN:
            if (snake->direction == UP) break;
            snake->direction = dir;
            break;
        case RIGHT:
            if (snake->direction == RIGHT) break;
            snake->direction = dir;
            break;
        case LEFT:
            if (snake->direction == LEFT) break;
            snake->direction = dir;
            break;
    }
    shift_snake(snake, board);
    ret = check_food(snake, board);
    if (ret != 0) goto exit;
    check_collision(exit_game, snake, board);

exit:
    return ret;
}

int check_food(node_t *snake, board_t *board)
{
    int ret = 0;
    set_bit(board->board, ((snake->location.y*(board->width))+snake->location.x)); // Temp set head bit
    if (get_bit(board->board, ((board->food.y*(board->width))+board->food.x))) {
        ret = grow_snake(snake, board);
        if (ret != 0) goto exit;
        ret = spawn_food(board);
        if (ret != 0) goto exit;
    }
    clear_bit(board->board, ((snake->location.y*(board->width))+snake->location.x)); // Clear head bit

exit:
    return ret;
}

void check_collision(atomic_bool *exit_game, node_t *snake, board_t *board)
{
    if (get_bit(board->board, ((snake->location.y*(board->width))+snake->location.x)))
        (*exit_game) = 1;
    else
        set_bit(board->board, ((snake->location.y*(board->width))+snake->location.x)); // Set head bit
}

int grow_snake(node_t *snake, board_t *board)
{
    int ret = 0;
    node_t *p = snake;
    coordinate_t c;

    while (p) {
        node_t *p = snake->next;;
        switch(p->direction) {
            case UP:
                c.y = p->location.y-1;
                c.x = p->location.x;
                if (c.y < 0)
                    c.y = board->height-1;
                break;
            case DOWN:
                c.y = p->location.y+1;
                if (c.y >= board->height)
                    c.y = 0;
                break;
            case RIGHT:
                c.x = p->location.x-1;
                c.y = p->location.y;
                if (c.x < 0)
                    c.x = board->width-1;
                break;
            case LEFT:
                c.x = p->location.x+1;
                c.y = p->location.y;
                if (c.x >= board->width)
                    c.x = 0;
                break;
        }
    }
    set_bit(board->board, ((c.y*(board->width))+c.x));
    ret = insert(p, c, p->direction);

    return ret;
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

void draw_board(board_t board)
{
    // fflush(stdout);
    char c;
    printf("\n");
    for (int i = board.width-1; i >= 0; i--) {
        for (int j = 0; j < board.height; j++) {
            if ((board.food.x == j) & (board.food.y == i))
                c = '0';
            else
                c = (get_bit(board.board, ((i*(board.width))+j))) ? 's': '*';
            printf("%c", c);
        }
        printf("\n");
    }
}

int insert(node_t *list, coordinate_t location, direction_t dir)
{
    int ret = 0;
    node_t *next;
    next = (node_t *)malloc(sizeof(node_t));
    if (next == NULL) {
        perror("Allocation error");
        ret = -1;
        goto exit;
    }
    next->direction = dir;
    next->location = location;
    next->next = NULL;
    list->next = next;
exit:
    return ret;
}

void freeList(node_t *list)
{
   node_t *temp;

   while (list != NULL)
    {
       temp = list;
       list = list->next;
       free(temp);
    }

}

void enableRaw()
{
    struct termios
}