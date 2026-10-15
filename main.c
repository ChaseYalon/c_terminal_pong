#include <stdio.h>
#include <stdint.h>
typedef struct {
    uint32_t x;
    uint32_t y;
} Ball;
typedef enum {
    PADDLE = 1,
    BALL = 2,
    EMPTY = 3
} CellState;
typedef struct{
    size_t width;
    size_t height;
    
} Board;

int main(){
    printf("hello world");
}