
//You will inevitably forget this and cause a bug, but 0,0 is top left, not bottom left or middle
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <conio.h>
#include "flags.h"
#ifdef DEBUG
static char* speeds;
#endif
#ifdef _WIN64
    #include "pong_windows.h"
#endif
typedef struct {
    int x;
    int y;
    float vx;
    float vy;
    int size;
} Ball;
typedef struct {
    int y;
    bool is_left;
} Paddle;
typedef struct {
    float paddle_width;
    float paddle_height;
    float paddle_side_margin;
    float ball_width;
    float bottom_rule_offset;
} Config;
typedef struct{
    size_t width;
    size_t height;
    Ball* ball;
    Paddle* left;
    Paddle* right;
    Config* config;
} Board;
void draw_board(Board* board) {
    set_cursor_pos(0, 0);
    //clear background
    for(int i = 0; i < board->width; i++){
        for(int j = 0; j < board->height; j++){
            set_pixel_black(i, j);
        }
    }

    //left paddle
    int left_top_left_x = ceil(board->config->paddle_side_margin * board->width);
    int left_top_left_y = board->left->y;
    int left_bottom_right_x = left_top_left_x + ceil(board->config->paddle_width * board->width);
    int left_bottom_right_y = left_top_left_y + ceil(board->config->paddle_height * board->height);
    for(int i = left_top_left_x; i < left_bottom_right_x; i++){
        for(int j = left_top_left_y; j < left_bottom_right_y; j++){
            set_pixel_white(i, j);
        }
    }

    //right paddle
    int right_top_left_x = ceil(board->width - left_bottom_right_x);
    int right_top_left_y = ceil(board->right->y);
    int right_bottom_right_x = right_top_left_x + ceil(board->config->paddle_width * board->width);
    int right_bottom_right_y = right_top_left_y + ceil(board->config->paddle_height * board->height);
    for(int i = right_top_left_x; i < right_bottom_right_x; i++){
        for(int j = right_top_left_y; j < right_bottom_right_y; j++){
            set_pixel_white(i, j);
        }
    }

    //ball
    for(int i = board->ball->x; i < board->ball->x + board->ball->size; i++){
        for(int j = board->ball->y; j < board->ball->y + (board->ball->size / 2); j++){//cells are rectangular
            set_pixel_white(i, j);
        }
    }

    //bottom rule
    int rule_level = board->height;
    for(int i = 0; i < board->width; i++){
        set_pixel_char(i, rule_level, '_');
    }
    //horizontal separators 
    for (int i = rule_level + 1; i < board->height * (2-board->config->bottom_rule_offset); i++){
        set_pixel_char(board->width * 0.25, i, '|');
        set_pixel_char(board->width * 0.75, i, '|');
    }
    present();
}
Board* create_board(){
    Ball* ball = malloc(sizeof(Ball));
    Paddle* left_paddle = malloc(sizeof(Paddle));
    Paddle* right_paddle = malloc(sizeof(Paddle));
    Board* board = malloc(sizeof(Board));
    int height = 0;
    int width = 0;
    get_terminal_size(&width, &height);
    
    board->width = width;
    board->config = malloc(sizeof(Config));
    board->config->paddle_width = 0.01;
    board->config->ball_width = 0.01;
    board->config->paddle_side_margin = 0.05;
    board->config->paddle_height = 0.2;
    board->config->bottom_rule_offset = 0.85;
    board->height = height * board->config->bottom_rule_offset;

    ball->size = roundf(board->config->ball_width * width);
    ball->x = (width / 2) - (ball->size / 2);
    ball->y = (height / 2) - (ball->size / 2);
    ball->vx = -2.0;
    ball->vy = -1.0;

    board->ball = ball;

    left_paddle->is_left = true;
    left_paddle->y = (board->config->paddle_height) * board->height;
    board->left = left_paddle;

    right_paddle->is_left = false;
    right_paddle->y =left_paddle->y;
    board->right = right_paddle;
    
    return board;
}
int clamp(int num, int min, int max){
    if (num < min){
        return min;
    } else if (num > max){
        return max;
    }
    return num;
}
bool run_cycle(Board* board){
    if (GetAsyncKeyState('Q') & 0x8000) {
        return false;
    }
    if (GetAsyncKeyState('W') & 0x8000) board->left->y -= 1;
    if (GetAsyncKeyState('S') & 0x8000) board->left->y += 1;
    if (GetAsyncKeyState(VK_UP) & 0x8000) board->right->y -= 1;
    if (GetAsyncKeyState(VK_DOWN) & 0x8000) board->right->y += 1;
    board->left->y = clamp(board->left->y, 0, board->height);
    board->right->y = clamp(board->right->y, 0, board->height);

    board->ball->x += board->ball->vx;
    board->ball->y += board->ball->vy;

    if (board->ball->y <= 0){ //ball hits top
        board->ball->y = 0;
        board->ball->vy = -board->ball->vy;
    } else if (board->ball->y >= board->height){ //ball hits bottom
        board->ball->y = board->height;
        board->ball->vy = - board->ball->vy;
    }
    if (board->ball->x <= 0){ //ball hits left
        board->ball->x = 0;
        board->ball->vx = -board->ball->vx;
    } else if (board->ball->x >= board->width){ //ball hits right
        board->ball->x = board->width;
        board->ball->vx = -board->ball->vx;
    }
    int paddle_width = ceil(board->config->paddle_width * board->width);
    int paddle_height = ceil(board->config->paddle_height * board->height);
    int ball_height = board->ball->size / 2;
    int ball_right_x = board->ball->x + board->ball->size;
    int ball_bottom_y = board->ball->y + ball_height;

    int left_paddle_left_x = ceil(board->config->paddle_side_margin * board->width);
    int left_paddle_right_x = left_paddle_left_x + paddle_width;
    int left_paddle_bottom_y = board->left->y + paddle_height;

    if (board->ball->vx < 0 && board->ball->x <= left_paddle_right_x && ball_right_x >= left_paddle_left_x && board->ball->y < left_paddle_bottom_y && ball_bottom_y > board->left->y) {
        board->ball->x = left_paddle_right_x;
        int ph = (int)ceil(board->config->paddle_height * board->height);
        float ball_cy = board->ball->y + ball_height / 2.0f;
        float pad_cy  = board->left->y + ph / 2.0f;
        float offset  = (ball_cy - pad_cy) / (ph / 2.0f);  // -1..1
        if (offset > 1) offset = 1;
        if (offset < -1) offset = -1;

        float speed = hypotf(board->ball->vx, board->ball->vy) * 1.05f;  // optional speedup per hit
        float angle = offset * 1.0f;                       // max ~57 degrees, in radians
        float dir   = 1.0f;                                // send it away from the paddle

        board->ball->vx = dir * speed * cosf(angle);
        board->ball->vy = speed * sinf(angle);
    }

    int right_paddle_left_x = board->width - left_paddle_right_x;
    int right_paddle_right_x = right_paddle_left_x + paddle_width;
    int right_paddle_bottom_y = board->right->y + paddle_height;

    if (board->ball->vx > 0 && ball_right_x >= right_paddle_left_x && board->ball->x <= right_paddle_right_x && board->ball->y < right_paddle_bottom_y && ball_bottom_y > board->right->y) {
        board->ball->x = right_paddle_left_x - board->ball->size;
        int ph = (int)ceil(board->config->paddle_height * board->height);
        float ball_cy = board->ball->y + ball_height / 2.0f;
        float pad_cy  = board->right->y + ph / 2.0f;
        float offset  = (ball_cy - pad_cy) / (ph / 2.0f);  // -1..1
        if (offset > 1) offset = 1;
        if (offset < -1) offset = -1;

        float speed = hypotf(board->ball->vx, board->ball->vy) * 1.05f;  // optional speedup per hit
        float angle = offset * 1.0f;                       // max ~57 degrees, in radians
        float dir   = -1.0f;                               // send it away from the paddle

        board->ball->vx = dir * speed * cosf(angle);
        board->ball->vy = speed * sinf(angle);
    }
    

    draw_board(board);
    #ifdef DEBUG
    char buff[100];
    snprintf(buff, sizeof(buff), "VX: %f, VY: %f, Speed: %f\n", board->ball->vx, board->ball->vy, sqrt((float)pow((board->ball->vx), 2) + pow(board->ball->vy, 2)));
    strcat(speeds, buff);
    #endif
    return true;
}

int main(){
    #ifdef DEBUG
    speeds = calloc(100000, ' '); //some large buffer
    #endif
    Board* b = create_board();
    init_screen(b->width, b->height * (2 - b->config->bottom_rule_offset));
    while (run_cycle(b)){
        Sleep(50);
    }
    #ifdef DEBUG
    FILE* f = fopen("speeds.log", "w");
    fprintf(f, "%s", speeds);
    #endif
}