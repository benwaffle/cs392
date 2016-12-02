#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <term.h>
#include <termios.h>

void rawmode(bool enable)
{
    static struct termios old;
    static bool init = false;
    if (!init) {
        init = true;
        tcgetattr(0, &old);
    }

    if (enable) {
        struct termios raw = old;

        raw.c_lflag &= ~(ICANON | IEXTEN); // disable canonical mode
        raw.c_lflag &= ~(ECHO | ECHONL); // no echo
        raw.c_lflag &= ~ISIG; // disable signals (INT, QUIT, TSTP, ...)

        raw.c_iflag |= ICRNL; // CR -> NL
        raw.c_iflag &= ~INLCR; // no NL -> CR

        raw.c_oflag |= OPOST; // implementation-defined output processing
        raw.c_oflag |= ONLCR; // NL -> CR,NL
        raw.c_oflag &= ~(OCRNL | ONOCR | ONLRET); // output CR

        raw.c_cc[VMIN] = 1; // minimum number of chars for read()
        raw.c_cc[VTIME] = 0; // read() timeout
        tcsetattr(0, TCSANOW, &raw);
    } else {
        tcsetattr(0, TCSANOW, &old);
    }
}

void debug(int line, const char *fmt, ...)
{
#ifdef DEBUG
    va_list args;
    va_start(args, fmt);

    putp(tigetstr("sc")); // save cursor
    // clang-format off
    putp(tparm(tigetstr("cup"), tigetnum("lines") - line, 0)); // move cur to y, x
    // clang-format on
    putp(tigetstr("dl1")); // delete line

    vprintf(fmt, args);
    va_end(args);

    putp(tigetstr("rc")); // restore cursor
#endif
}

void clr2eol(int pos)
{
    if (pos > 0)
        putp(tparm(tigetstr("cub"), pos)); // move left `pos' spaces
    putp(tigetstr("el")); // delete to eol
}
