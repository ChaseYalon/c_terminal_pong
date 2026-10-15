#include <windows.h>
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
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) return;

    // 2. Define the target cell coordinate (X = column, Y = row)
    COORD cellCoords = { x, y };

    // 3. Define the pure white background attribute (RGB + High Intensity)
    WORD whiteBackground = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY;

    // 4. Write the attribute to exactly 1 cell
    DWORD attributesWritten;
    WriteConsoleOutputAttribute(
        hConsole,          // Screen buffer handle
        &whiteBackground,  // Pointer to the color attribute
        1,                 // Number of cells to modify
        cellCoords,        // Coordinate of the cell
        &attributesWritten // Variable to store results
    );

    // Optional: If the cell is empty, you may also want to write a space character to it
    DWORD charsWritten;
    WriteConsoleOutputCharacter(hConsole, " ", 1, cellCoords, &charsWritten);

    // Move cursor out of the way for clean output
    COORD endCoords = { 0, 10 };
    SetConsoleCursorPosition(hConsole, endCoords);
}
void set_pixel_black(int x, int y){
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole == INVALID_HANDLE_VALUE) return;

    // 2. Define the target cell coordinate (X = column, Y = row)
    COORD cellCoords = { x, y };

    // 3. Define the pure white background attribute (RGB + High Intensity)
    WORD whiteBackground = 0;

    // 4. Write the attribute to exactly 1 cell
    DWORD attributesWritten;
    WriteConsoleOutputAttribute(
        hConsole,          // Screen buffer handle
        &whiteBackground,  // Pointer to the color attribute
        1,                 // Number of cells to modify
        cellCoords,        // Coordinate of the cell
        &attributesWritten // Variable to store results
    );

    // Optional: If the cell is empty, you may also want to write a space character to it
    DWORD charsWritten;
    WriteConsoleOutputCharacter(hConsole, " ", 1, cellCoords, &charsWritten);

    // Move cursor out of the way for clean output
    COORD endCoords = { 0, 10 };
    SetConsoleCursorPosition(hConsole, endCoords);
}