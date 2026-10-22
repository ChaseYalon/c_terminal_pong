
//You will inevitably forget this and cause a bug, but 0,0 is top left, not bottom left or middle
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <conio.h>
#include "flags.h"
#include "font.h"
#define SCORE_AREA_HEIGHT 12

#ifdef DEBUG
static char* speeds;
#endif
#ifdef _WIN64
    #include "pong_windows.h"
#endif
typedef struct {
    float x;
    float y;
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
    float number_spacing;
} Config;
typedef struct{
    size_t width;
    size_t height;
    Ball* ball;
    Paddle* left;
    Paddle* right;
    Config* config;
    uint32_t left_score;
    uint32_t right_score;
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
    int bx = round(board->ball->x);
    int by = round(board->ball->y);
    for(int i = bx; i < bx + board->ball->size; i++){
        for(int j = by; j < by + (board->ball->size / 2); j++){//cells are rectangular
            set_pixel_white(i, j);
        }
    }

    //bottom rule
    int rule_level = board->height;
    for(int i = 0; i < board->width; i++){
        set_pixel_char(i, rule_level, '_');
    }
    //horizontal separators 
    int screen_height = board->height + SCORE_AREA_HEIGHT;
    for (int i = rule_level + 1; i < screen_height; i++){
        set_pixel_char(board->width * 0.2, i, '|');
        set_pixel_char(board->width * 0.78, i, '|');
    }
    int score_y = (rule_level + (screen_height - rule_level) / 2) - 3;
    if (board->left_score < 10){
        large_char_at(board->width * 0.07, score_y, board->left_score + '0');
    } else {
        large_char_at(board->width * board->config->number_spacing, score_y, board->left_score / 10 + '0');
        large_char_at(board->width * (board->config->number_spacing + 0.07), score_y, board->left_score - 10 + '0');

    }
    if (board->right_score < 10){
        large_char_at(board->width * 0.87, score_y, board->right_score + '0');
    } else {
        large_char_at(board->width * (0.87 - board->config->number_spacing), score_y, board->right_score / 10 + '0');
        large_char_at(board->width * (0.87 + board->config->number_spacing), score_y, board->right_score - 10 + '0');
    }

    if (board->left_score >= 11 || board->right_score >= 11){
        const char* winner = "WINNER";
        int len = 6;

        float section_start = board->width * 0.25f;
        float section_width = board->width * 0.5f;
        float spacing = section_width / len;

        for (int i = 0; i < len; i++){
            int x = (int)(section_start + spacing * i);
            large_char_at(x, score_y, winner[i]);
        }
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
    board->config->number_spacing = 0.05;
    int proposed_height = height * board->config->bottom_rule_offset;
    if (height - proposed_height > 12){
        board->height = proposed_height;
    } else {
        board->height = height - 12;
    }
    if (height < 12){
        fprintf(stderr, "[ERROR] Screen must be at least 24 cells...");
        exit(-1);
    }

    ball->size = roundf(board->config->ball_width * width);
    ball->x = (width / 2.0) - (ball->size / 2.0);
    ball->y = (height / 2.0) - (ball->size / 2.0);
    ball->vx = -2.0;
    ball->vy = -1.0;

    board->ball = ball;

    left_paddle->is_left = true;
    left_paddle->y = (board->config->paddle_height) * board->height;
    board->left = left_paddle;

    right_paddle->is_left = false;
    right_paddle->y =left_paddle->y;
    board->right = right_paddle;
    
    board->left_score = 0;
    board->right_score = 0;
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
int rand_int(int min, int max){
    srand(time(NULL));
    int random_num = (rand() % (max - min + 1)) + min;
    return random_num;
}
bool run_cycle(Board* board){
    if (GetAsyncKeyState('Q') & 0x8000) {
        return false;
    }
    if (GetAsyncKeyState('W') & 0x8000) board->left->y -= 1;
    if (GetAsyncKeyState('S') & 0x8000) board->left->y += 1;
    if (GetAsyncKeyState(VK_UP) & 0x8000) board->right->y -= 1;
    if (GetAsyncKeyState(VK_DOWN) & 0x8000) board->right->y += 1;
    board->left->y = clamp(board->left->y, 0, board->height - (board->config->paddle_height * board->height));
    board->right->y = clamp(board->right->y, 0, board->height - (board->config->paddle_height * board->height));

    board->ball->x += board->ball->vx;
    board->ball->y += board->ball->vy;

    if (board->ball->y <= 0){ //ball hits top
        board->ball->y = 0;
        board->ball->vy = -board->ball->vy;
    } else if (board->ball->y >= board->height){ //ball hits bottom
        board->ball->y = board->height;
        board->ball->vy = - board->ball->vy;
    }
    bool reset = false;
    if (board->ball->x <= 0){ //ball hits left
        reset = true;
        board->right_score++;
    } else if (board->ball->x >= board->width){ //ball hits right
        reset = true;
        board->left_score++;
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
    
    if (reset){
        board->ball->x = (board->width / 2.0) - (board->ball->size / 2.0);
        board->ball->y = (board->height / 2.0) - (board->ball->size / 2.0);
        board->ball->vx = rand_int(-2, 2);
        board->ball->vy = rand_int(-2, 2);
    }
    if (board->left_score == 11 || board->right_score == 11){
        draw_board(board);
        while (true){
            if (GetAsyncKeyState('Q') & 0x8000) return false;
        }
    }
    //sanity check/weird bug fix
    if (board->ball->vx == 0){
        board->ball->vx = -1;
    }
    if (board->ball->vy ==0){
        board->ball->vy = -1;
    }
    draw_board(board);
    #ifdef DEBUG
    char buff[100];
    snprintf(buff, sizeof(buff), "VX: %f, VY: %f, Speed: %f\n", board->ball->vx, board->ball->vy, sqrt((float)pow((board->ball->vx), 2) + pow(board->ball->vy, 2)));
    strcat(speeds, buff);
    #endif
    return true;
}
int delay_curve(int64_t start, int64_t elapsed_ms){
    double t = elapsed_ms / 1000.0;
    double floor_delay = start * 0.25;
    double d = floor_delay + (start - floor_delay) * exp(-t / 20.0);
    return (int)d;
}
int64_t get_epoch_ms(void) {
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    return ((int64_t)ts.tv_sec * 1000) + ((int64_t)ts.tv_nsec / 1000000);
}
int main(){
    #ifdef DEBUG
    speeds = calloc(100000, ' '); //some large buffer
    #endif
    
    Board* b = create_board();
    init_screen(b->width, b->height + SCORE_AREA_HEIGHT);
    int64_t start_time = get_epoch_ms();
    int64_t start_delay = 50;
    while (run_cycle(b)){
        int64_t delta = get_epoch_ms() - start_time;
        int64_t delay = delay_curve(start_delay, delta);
        #ifdef DEBUG
        #endif
        Sleep(delay);
    }
    #ifdef DEBUG
    FILE* f = fopen("speeds.log", "w");
    fprintf(f, "%s", speeds);
    #endif
}