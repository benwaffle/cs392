#include <string.h>
#include <curses.h>
#include <assert.h>

#define MAX(a,b) ((a) > (b) ? (a) : (b))

int sum(int arr[], int size) {
    int s = 0;
    for (int i=0; i<size; ++i)
        s += arr[i];
    return s;
}

void showlist(int count, char *list[]) {
    int width, height;
    getmaxyx(stdscr, height, width);

    int cols = count / height + 1;

    clear();

    int maxwidth[cols];
    memset(maxwidth, 0, sizeof maxwidth);

    for (int i=0; i<count; ++i) {
        int col = i / height;
        maxwidth[col] = MAX(maxwidth[col], strlen(list[i]) + 1);
    }
    maxwidth[cols-1]--;

    if (sum(maxwidth, cols) > width) {
        char msg[] = "Please enlarge the window";
        mvprintw(height/2, width/2 - sizeof(msg)/2, "%s", msg);
        return;
    }

    for (int i=0; i<count; ++i) {
        int row = i % height;
        int curcol = i / height;
        int col = sum(maxwidth, curcol);

        mvprintw(row, col, "%s", list[i]);
    }
}

int main(int argc, char *argv[]) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    do {
        showlist(argc-1, argv+1);
        refresh();
    } while (getch() == KEY_RESIZE);

    endwin();
}
