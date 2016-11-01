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

int countcols(int count) {
    return count / getmaxy(stdscr) + 1;
}

void showlist(int count, char *list[], int selected) {
    int width, height;
    getmaxyx(stdscr, height, width);

    int cols = countcols(count);

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

        if (i == selected)
            attron(A_UNDERLINE);

        mvprintw(row, col, "%s", list[i]);

        if (i == selected)
            attroff(A_UNDERLINE);
    }
}

int main(int argc, char *argv[]) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    int selected = 0;
    argc--;
    argv++;
    showlist(argc, argv, selected);


    int c;
    while ((c = getch()) != ERR){
        if (c == KEY_DOWN) {
            selected++;
            if (selected >= argc)
                selected -= argc;
        } else if (c == KEY_UP) {
            selected--;
            if (selected < 0)
                selected += argc;
        } else if (c == KEY_RIGHT) {
            int height = getmaxy(stdscr);
            selected += height;
            if (selected >= argc)
                selected = argc-1;
        } else if (c == KEY_LEFT) {
            int height = getmaxy(stdscr);
            selected -= height;
            if (selected < 0)
                selected = 0;
        }

        showlist(argc, argv, selected);

        refresh();
    }

    endwin();
}
