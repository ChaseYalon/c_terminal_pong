//You will invetably forget this and cause a bug, but 0,0 is top left, not bottom left or middle
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#ifdef _WIN64
    #include "pong_windows.h"
#endif
typedef struct {
    uint32_t x;
    uint32_t y;
    uint32_t size;
} Ball;
typedef struct {
    uint32_t y;
    bool is_left;
} Paddle;
typedef struct{
    size_t width;
    size_t height;
    Ball* ball;
    Paddle* left;
    Paddle* right;
    float paddle_width_pct;
    float paddle_height_pct;
    float paddle_side_margin_pct;
} Board;
void draw_board(Board* board) {
    //clear background
    for(int i = 0; i < board->width; i++){
        for(int j = 0; j < board->height; j++){
            set_pixel_black(i, j);
        }
    }

    //left paddle
    int left_top_left_x = ceil(board->paddle_side_margin_pct * board->width);
    int left_top_left_y = ceil(board->left->y);
    int left_bottom_right_x = left_top_left_x + ceil(board->paddle_width_pct * board->width);
    int left_bottom_right_y = left_top_left_y + ceil(board->paddle_height_pct * board->height);
    for(int i = left_top_left_x; i < left_bottom_right_x; i++){
        for(int j = left_top_left_y; j < left_bottom_right_y; j++){
            set_pixel_white(i, j);
        }
    }
}
Board* create_board(){
    Ball* ball = malloc(sizeof(Ball));
    Paddle* left_paddle = malloc(sizeof(Paddle));
    Paddle* right_paddle = malloc(sizeof(Paddle));
    Board* board = malloc(sizeof(Board));
    int height = 0;
    int width = 0;
    get_terminal_size(&width, &height);
    
    board->height = height;
    board->width = width;
    board->paddle_width_pct = 0.15;
    float ball_width_pct = 0.05;
    board->paddle_side_margin_pct = 0.05;
    board->paddle_height_pct = 0.25;

    ball->size = roundf(ball_width_pct * width);
    ball->x = (width / 2) - (ball->size / 2);
    ball->y = (height / 2) - (ball->size / 2);

    board->ball = ball;

    left_paddle->is_left = true;
    left_paddle->y = (board->paddle_height_pct) * board->height;
    board->left = left_paddle;

    right_paddle->is_left = false;
    right_paddle->y =left_paddle->y;
    board->right = right_paddle;
    
    return board;
}

int main(){
    Board* b = create_board();
    draw_board(b);
    printf("Width: %lld, Height: %lld", b->width, b->height);

}