#include <locale.h>
#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CELL_LIVE       ACS_BLOCK
#define CELL_DEAD       ACS_BULLET
#define CELL_EXPR       (rand() % 50 ? CELL_DEAD : CELL_LIVE)
#define CELL_EXPR_INV   (rand() % 50 ? CELL_LIVE : CELL_DEAD)

static size_t
count_neighbors(size_t line, size_t col)
{
    size_t neighbors = 0;

    neighbors += mvinch(line - 1, col) == CELL_LIVE;
    neighbors += mvinch(line - 1, col + 1) == CELL_LIVE;
    neighbors += mvinch(line, col + 1) == CELL_LIVE;
    neighbors += mvinch(line + 1, col + 1) == CELL_LIVE;
    neighbors += mvinch(line + 1, col) == CELL_LIVE;
    neighbors += mvinch(line + 1, col - 1) == CELL_LIVE;
    neighbors += mvinch(line, col - 1) == CELL_LIVE;
    neighbors += mvinch(line - 1, col - 1) == CELL_LIVE;

    return neighbors;
}

static void
init_life(void)
{
    size_t midline = LINES / 2;
    size_t midcol = COLS / 2;

    for (;;) {
        chtype ch = CELL_EXPR;
        if (addch(ch) == ERR) {
            break;
        }
    }

    mvaddch(midline + 1, midcol, CELL_LIVE);
    mvaddch(midline + 1, midcol - 1, CELL_LIVE);
    mvaddch(midline + 1, midcol + 1, CELL_LIVE);
    mvaddch(midline, midcol + 1, CELL_LIVE);
    mvaddch(midline - 1, midcol, CELL_LIVE);
}

static void
iter_life(void)
{
    chtype cell_state;
    size_t neighbors;

    for (ssize_t line = 0; line < LINES; ++line) {
        for (ssize_t col = 0; col < COLS; ++col) {
            neighbors = count_neighbors(line, col);
            cell_state = mvinch(line, col);

            if (cell_state == CELL_DEAD && neighbors == 3) {
                addch(CELL_LIVE);
            } else if (cell_state == CELL_LIVE
                    && (neighbors == 3 || neighbors == 2)) {
                addch(CELL_LIVE);
            }

            addch(CELL_DEAD);
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
