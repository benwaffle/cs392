#include <curses.h>

int main() {
    initscr();
    printw("Hello world\n");
    refresh();
    int c;
    while ((c = getch()) == KEY_RESIZE)
        printw("resized\n");
    endwin();
}
