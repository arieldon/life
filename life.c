#include <locale.h>
#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CELL_ALIVE      ACS_BLOCK
#define CELL_DEAD       ACS_BULLET
#define CELL_EXPR       (rand() % 50 ? CELL_DEAD : CELL_ALIVE)
#define CELL_EXPR_INV   (rand() % 50 ? CELL_ALIVE : CELL_DEAD)

static size_t
count_neighbors(size_t line, size_t col)
{
    size_t neighbors = 0;

    neighbors += mvinch(line - 1, col) == CELL_ALIVE;
    neighbors += mvinch(line - 1, col + 1) == CELL_ALIVE;
    neighbors += mvinch(line, col + 1) == CELL_ALIVE;
    neighbors += mvinch(line + 1, col + 1) == CELL_ALIVE;
    neighbors += mvinch(line + 1, col) == CELL_ALIVE;
    neighbors += mvinch(line + 1, col - 1) == CELL_ALIVE;
    neighbors += mvinch(line, col - 1) == CELL_ALIVE;
    neighbors += mvinch(line - 1, col - 1) == CELL_ALIVE;

    return neighbors;
}

static void
init_life(void)
{
    for (;;) {
        chtype ch = CELL_EXPR;
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
                /* A cell on the edge can die. */
                if (count_neighbors(line, col) < 8) {
                    addch(CELL_EXPR_INV);
                    if (inch() == CELL_DEAD) {
                        continue;
                    }
                }

                /*
                 * mvinch() simply returns ERR if passed a position
                 * out-of-bounds -- it doesn't crash.
                 */
                if (mvinch(line + 1, col) == CELL_DEAD) {
                    addch(CELL_EXPR);
                } else if (mvinch(line, col + 1) == CELL_DEAD) {
                    addch(CELL_EXPR);
                } else if (mvinch(line - 1, col) == CELL_DEAD) {
                    addch(CELL_EXPR);
                } else if (mvinch(line, col - 1) == CELL_DEAD) {
                    addch(CELL_EXPR);
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

    size_t delay = 1;
    size_t iterations = 100;

    initscr();
    cbreak();
    noecho();
    curs_set(0);

    char *msg = "Welcome to Conway's Game of Life! Press any key to observe.";
    mvprintw(LINES / 2, (COLS - strlen(msg)) / 2, msg);
    refresh();

    getch();
    clear();

    init_life();
    refresh();
    sleep(delay);

    for (size_t i = 0; i < iterations; ++i) {
        iter_life();
        refresh();
        sleep(delay);
    }

    endwin();
    exit(EXIT_SUCCESS);
}
