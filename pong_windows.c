#include <windows.h>
#include "pong_windows.h"
void get_terminal_size(int *width, int *height) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    if (hStdout != INVALID_HANDLE_VALUE && GetConsoleScreenBufferInfo(hStdout, &csbi)) {
        // Calculate the visible window dimensions
        *width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        *height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    } else {
        *width = 80;  // Default fallback width
        *height = 25; // Default fallback height
    }
}

void set_pixel_white(int x, int y){
    if (x < 0 || y < 0 || x >= width || y >= height) return;
    CHAR_INFO* c = &pixel_buffs[y * width + x];
    c->Char.AsciiChar = ' ';
    c->Attributes = BACKGROUND_RED | BACKGROUND_GREEN |
                    BACKGROUND_BLUE | BACKGROUND_INTENSITY;}
void set_pixel_char(int x, int y, char c) {
    if (x < 0 || y < 0 || x >= width || y >= height) {
        return;
    }

    CHAR_INFO *pixel = &pixel_buffs[y * width + x];
    pixel->Char.AsciiChar = c;
    pixel->Attributes = FOREGROUND_RED |
                        FOREGROUND_GREEN |
                        FOREGROUND_BLUE;
}
void set_pixel_black(int x, int y){
    return;
}
void set_cursor_pos(int x, int y){
    SetCursorPos(x, y);
}
void present(void) {
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD size = { (SHORT)width, (SHORT)height };
    COORD zero = { 0, 0 };
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(out, &csbi);
    SMALL_RECT r = csbi.srWindow;   // draw into the visible window
    WriteConsoleOutputA(out, pixel_buffs , size, zero, &r);
    for (int i = 0; i < width * height; i++) {
        pixel_buffs[i].Char.AsciiChar = ' ';
        pixel_buffs[i].Attributes = 0;
    }
}
void move_cursor_to_bottom(){
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if (GetConsoleScreenBufferInfo(h, &csbi)) {
        COORD pos = { 0, csbi.srWindow.Bottom };  // col 0, last visible row
        SetConsoleCursorPosition(h, pos);
    }
}
void init_screen(int w, int h) {
    pixel_buffs = calloc(w * h, sizeof(CHAR_INFO));
    width = w;
    height = h;
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO ci = { 1, FALSE };   // hide the blinking cursor
    SetConsoleCursorInfo(out, &ci);
}