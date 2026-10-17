#include <windows.h>
static CHAR_INFO* pixel_buffs;
static int width;
static int height;
void get_terminal_size(int *width, int *height);
void set_pixel_white(int x, int y);
void set_pixel_black(int x, int y);
void set_cursor_pos(int x, int y);
void move_cursor_to_bottom();
void init_screen(int w, int h);
void present(void);