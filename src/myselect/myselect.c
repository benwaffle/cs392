#include <assert.h>
#include <curses.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

typedef struct {
    char *type; // rule to match
    int color; // ncurses color pair
    int attrs; // ncurses colors
} lscolor;

/**
 * LS_COLORS is a list of rules separated by :
 * Each rule is of the format type=attrs
 * The attrs are numbers separated by ;
 * e.g.
 * "*.mp4=40;34:*.jpg=04;32"
 */
lscolor **parsecolors()
{
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

    int i = 0;
    char *splitrules; // saveptr for strtok
    char *_rule = strtok_r(env, ":", &splitrules); // split by :
    do {
        char *splitcur, *splitcolors;
        char *rule = strdup(_rule);
        colors[i] = calloc(1, sizeof(lscolor));
        colors[i]->type = strtok_r(rule, "=", &splitcur); // split by =
        colors[i]->color = i + 1; // the color number can't be 0
        colors[i]->attrs = 0;
        char *attrs = strtok_r(NULL, "=", &splitcur);

        short f = -1, b = -1; // foreground and background colors
        char *color = strtok_r(attrs, ";", &splitcolors); // split by ;
        do {
            int attr = atoi(color);
            // clang-format off
                 if (attr ==  0) f = -1, b = -1;
            else if (attr ==  1) colors[i]->attrs |= A_BOLD;
            else if (attr ==  4) colors[i]->attrs |= A_UNDERLINE;
            else if (attr ==  5) colors[i]->attrs |= A_BLINK;
            else if (attr ==  7) colors[i]->attrs |= A_REVERSE;
            else if (attr ==  8) colors[i]->attrs |= A_INVIS;
            else if (attr == 30) f = COLOR_BLACK;
            else if (attr == 31) f = COLOR_RED;
            else if (attr == 32) f = COLOR_GREEN;
            else if (attr == 33) f = COLOR_YELLOW;
            else if (attr == 34) f = COLOR_BLUE;
            else if (attr == 35) f = COLOR_MAGENTA;
            else if (attr == 36) f = COLOR_CYAN;
            else if (attr == 37) f = COLOR_WHITE;
            else if (attr == 40) b = COLOR_BLACK;
            else if (attr == 41) b = COLOR_RED;
            else if (attr == 42) b = COLOR_GREEN;
            else if (attr == 43) b = COLOR_YELLOW;
            else if (attr == 44) b = COLOR_BLUE;
            else if (attr == 45) b = COLOR_MAGENTA;
            else if (attr == 46) b = COLOR_CYAN;
            else if (attr == 47) b = COLOR_WHITE;
            // clang-format on
        } while ((color = strtok_r(NULL, ";", &splitcolors)) != NULL);

        init_pair(colors[i]->color, f, b);

        i++;
    } while ((_rule = strtok_r(NULL, ":", &splitrules)) != NULL);

    free(env);

    return colors;
}

int sum(int arr[], int size)
{
    int s = 0;
    for (int i = 0; i < size; ++i)
        s += arr[i];
    return s;
}

int countcols(int count) { return count / getmaxy(stdscr) + 1; }

void showlist(int count, char *list[], int curline, bool selected[],
              lscolor **colors)
{
    int width, height;
    getmaxyx(stdscr, height, width);

    int cols = countcols(count);

    clear();

    int maxwidth[cols];
    memset(maxwidth, 0, sizeof maxwidth);

    for (int i = 0; i < count; ++i) {
        int col = i / height;
        maxwidth[col] = MAX(maxwidth[col], strlen(list[i]) + 1);
    }
    maxwidth[cols - 1]--;

    if (sum(maxwidth, cols) > width) {
        char msg[] = "Please enlarge the window";
        mvprintw(height / 2, width / 2 - sizeof(msg) / 2, "%s", msg);
        return;
    }

    for (int i = 0; i < count; ++i) {
        int row = i % height;
        int curcol = i / height;
        int col = sum(maxwidth, curcol);

        if (i == curline)
            attron(A_UNDERLINE);

        if (selected[i])
            attron(A_STANDOUT);

        bool isfile = true;
        struct stat sb;
        if (stat(list[i], &sb) < 0)
            isfile = false;

        // color
        int len = strlen(list[i]);
        for (lscolor **c = colors; *c; ++c) {
            char *rule = (*c)->type;
            int extlen = strlen(rule);
            if (
                // check *.ext
                (rule[0] == '*' &&
                 strcmp(list[i] + (len - extlen) + 1, rule + 1) == 0) ||
                // directories
                (isfile && strcmp(rule, "di") == 0 && S_ISDIR(sb.st_mode)) ||
                // symlink
                (isfile && strcmp(rule, "ln") == 0 && S_ISLNK(sb.st_mode)) ||
                // pipe
                (isfile && strcmp(rule, "pi") == 0 && S_ISFIFO(sb.st_mode)) ||
                // socket
                (isfile && strcmp(rule, "so") == 0 && S_ISSOCK(sb.st_mode)) ||
                // block device
                (isfile && strcmp(rule, "bd") == 0 && S_ISBLK(sb.st_mode)) ||
                // character device driver
                (isfile && strcmp(rule, "cd") == 0 && S_ISCHR(sb.st_mode)) ||
                // executable file
                (isfile && strcmp(rule, "ex") == 0 &&
                 (sb.st_mode & S_IXUSR || // user exec
                  sb.st_mode & S_IXGRP || // group exec
                  sb.st_mode & S_IXOTH)) // other exec
                ) {
                attron((*c)->attrs);
                color_set((*c)->color, NULL);
                break;
            }
        }

        mvprintw(row, col, "%s", list[i]);

        attrset(0);
        color_set(0, NULL);
    }
}

int main(int argc, char *argv[])
{
    int out = 1;
    if (!isatty(1)) {
        // save stdout for subshells
        out = dup(1);
        // stdout = tty
        dup2(open("/dev/tty", O_RDWR), 1);
    }

    initscr();
    start_color();
    use_default_colors();
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
    while ((c = getch()) != ERR) {
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
                curline = argc - 1;
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
            // only free type because color is part of the same string
            free((*c)->type);
            free(*c);
        }
        free(colors);
    }

    // restore stdout
    dup2(out, 1);

    if (c == '\n') {
        for (int i = 0; i < argc; ++i)
            if (selected[i])
                printf("%s ", argv[i]);
        printf("\n");
    }
}
