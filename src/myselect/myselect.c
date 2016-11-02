#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <curses.h>
#include <assert.h>

#define MAX(a,b) ((a) > (b) ? (a) : (b))

typedef struct {
    char *type;
    char *color;
} lscolor;

lscolor **parsecolors() {
    char *env = getenv("LS_COLORS");
    if (!env)
        return NULL;

    env = strdup(env); // because strtok modifies it

    // count ':' chars
    int count = 0;
    for (int i = 0; env[i] != '\0'; ++i)
        if (env[i] == ':')
            ++count;

    ++count; // a:b -> 1 ':', 2 elems

    lscolor **colors = calloc(count + 1, sizeof *colors);

    int i=0;
    char *splitrules; // saveptr for strtok
    char *_rule = strtok_r(env, ":", &splitrules); // split by :
    do {
        char *splitcur;
        char *rule = strdup(_rule);
        char *type = strtok_r(rule, "=", &splitcur); // split by =
        char *color = strtok_r(NULL, "=", &splitcur);
        colors[i] = malloc(sizeof(lscolor));
        colors[i]->type = type;
        colors[i]->color = color;
        i++;
    } while ((_rule = strtok_r(NULL, ":", &splitrules)) != NULL);

    free(env);

    return colors;
}

int sum(int arr[], int size) {
    int s = 0;
    for (int i=0; i<size; ++i)
        s += arr[i];
    return s;
}

int countcols(int count) {
    return count / getmaxy(stdscr) + 1;
}

void showlist(int count, char *list[], int curline, bool selected[], lscolor **colors) {
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

        attrset(0);
    }
}

int main(int argc, char *argv[]) {
    int out = 1;
    if (!isatty(1)) {
        // save stdout for subshells
        out = dup(1);
        // stdout = tty
        dup2(open("/dev/tty", O_RDWR), 1);
    }

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

    lscolor **colors = parsecolors();

    showlist(argc, argv, curline, selected, colors);

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
            if (selected[curline]) // was just highlighted
                ungetch(KEY_DOWN); // move down one
        } else if (c == '\n') {
            break;
        }

        showlist(argc, argv, curline, selected, colors);

        refresh();
    }

    endwin();

    if (colors) {
        for (lscolor **c = colors; *c; ++c) {
            free((*c)->type); // only free type because color is part of the same string
            free(*c);
        }
        free(colors);
    }

    // restore stdout
    dup2(out, 1);

    if (c == '\n') {
        for (int i=0; i<argc; ++i)
            if (selected[i])
                printf("%s ", argv[i]);
        printf("\n");
    }
}
