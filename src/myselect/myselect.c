#include <string.h>
#include <curses.h>
#include <assert.h>
#include <stdbool.h>

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

void showlist(int count, char *list[], int curline, bool selected[]) {
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

        if (i == curline)
            attron(A_UNDERLINE);

        if (selected[i])
            attron(A_STANDOUT);

        mvprintw(row, col, "%s", list[i]);

        if (i == curline)
            attroff(A_UNDERLINE);

        if (selected[i])
            attroff(A_STANDOUT);
    }
}

int main(int argc, char *argv[]) {
    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);
    set_escdelay(0);
    curs_set(0);

    argc--;
    argv++;

    int curline = 0;
    bool selected[argc];
    memset(selected, false, sizeof selected);

    showlist(argc, argv, curline, selected);

    int c;
    while ((c = getch()) != ERR){
        if (c == 0x1B) { // esc
            break;
        } else if (c == KEY_DOWN) {
            curline++;
            if (curline >= argc)
                curline -= argc;
        } else if (c == KEY_UP) {
            curline--;
            if (curline < 0)
                curline += argc;
        } else if (c == KEY_RIGHT) {
            int height = getmaxy(stdscr);
            curline += height;
            if (curline >= argc)
                curline = argc-1;
        } else if (c == KEY_LEFT) {
            int height = getmaxy(stdscr);
            curline -= height;
            if (curline < 0)
                curline = 0;
        } else if (c == ' ') {
            selected[curline] = !selected[curline];
        } else if (c == '\n') {
            break;
        }

        showlist(argc, argv, curline, selected);

        refresh();
    }

    endwin();

    if (c == '\n') {
        for (int i=0; i<argc; ++i)
            if (selected[i])
                printf("%s ", argv[i]);
        printf("\n");
    }
}
