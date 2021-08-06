#include <locale.h>
#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CELL_ALIVE ACS_BLOCK
#define CELL_DEAD  ACS_BULLET
#define CELL_FUNC  (rand() % 50 ? CELL_DEAD : CELL_ALIVE)

static void
init_life(void)
{
    for (;;) {
        chtype ch = CELL_FUNC;
        if (addch(ch) == ERR) {
            break;
        }
    }
}

static void
iter_life(void)
{
    for (ssize_t line = 0; line < LINES; ++line) {
        for (ssize_t col = 0; col < COLS; ++col) {
            if (mvinch(line, col) == CELL_ALIVE) {
                /*
                 * mvinch() simply returns ERR if passed a position
                 * out-of-bounds -- it doesn't crash.
                 */
                if (mvinch(line + 1, col) == CELL_DEAD) {
                    addch(CELL_FUNC);
                } else if (mvinch(line, col + 1) == CELL_DEAD) {
                    addch(CELL_FUNC);
                } else if (mvinch(line - 1, col) == CELL_DEAD) {
                    addch(CELL_FUNC);
                } else if (mvinch(line, col - 1) == CELL_DEAD) {
                    addch(CELL_FUNC);
                }
            }
        }
    }
}

int
main(void)
{
    setlocale(LC_ALL, "");
    srand(time(NULL));

    ssize_t delay = 1;
    ssize_t iterations = 100;

    initscr();
    cbreak();
    noecho();

    char *msg = "Welcome to Conway's Game of Life! Press any key to observe.";
    mvprintw(LINES / 2, (COLS - strlen(msg)) / 2, msg);

    refresh();
    getch();
    clear();

    init_life();
    refresh();
    sleep(delay);

    for (ssize_t i = 0; i < iterations; ++i) {
        iter_life();
        refresh();
        sleep(delay);
    }

    endwin();
    exit(EXIT_SUCCESS);
}