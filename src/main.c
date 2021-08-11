#include <locale.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "matrix.h"

#define CELL_LIVE       ACS_BLOCK
#define CELL_DEAD       ACS_BULLET
#define USAGE(argv0)    fprintf(stderr, "usage: %s [-d nsecs] [-i niters]\n", \
                                argv0)

static size_t
count_neighbors(size_t row, size_t col)
{
    size_t neighbors = 0;

    neighbors += mvinch(row - 1, col) == CELL_LIVE;
    neighbors += mvinch(row - 1, col + 1) == CELL_LIVE;
    neighbors += mvinch(row, col + 1) == CELL_LIVE;
    neighbors += mvinch(row + 1, col + 1) == CELL_LIVE;
    neighbors += mvinch(row + 1, col) == CELL_LIVE;
    neighbors += mvinch(row + 1, col - 1) == CELL_LIVE;
    neighbors += mvinch(row, col - 1) == CELL_LIVE;
    neighbors += mvinch(row - 1, col - 1) == CELL_LIVE;

    return neighbors;
}

static void
initlife(struct matrix *state)
{
    size_t centerx = COLS / 2;
    size_t centery = LINES / 2;

    state->items[centery - 1][centerx] = 1;
    state->items[centery][centerx + 1] = 1;
    state->items[centery + 1][centerx + 1] = 1;
    state->items[centery + 1][centerx] = 1;
    state->items[centery + 1][centerx - 1] = 1;
}

static void
iterlife(struct matrix *state)
{
    size_t neighbors;

    /* Draw current state. */
    for (ssize_t row = 0; row < LINES; ++row) {
        for (ssize_t col = 0; col < COLS; ++col) {
            mvaddch(row, col, state->items[row][col] ? CELL_LIVE : CELL_DEAD);
        }
    }

    /* Update state for next iteration. */
    for (size_t row = 0; row < state->rows; ++row) {
        for (size_t col = 0; col < state->cols; ++col) {
            neighbors = count_neighbors(row, col);

            if (mvinch(row, col) == CELL_DEAD && neighbors == 3) {
                state->items[row][col] = 1;
            } else if (mvinch(row, col) == CELL_LIVE
                    && (neighbors == 2 || neighbors == 3)) {
                state->items[row][col] = 1;
            } else {
                state->items[row][col] = 0;
            }
        }
    }
}

int
main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");
    srand(time(NULL));

    int opt;
    size_t delay = 1;
    size_t iterations = 100;

    while ((opt = getopt(argc, argv, "d:i:")) != -1) {
        switch (opt) {
        case 'd':
            delay = atoi(optarg);
            break;
        case 'i':
            iterations = atoi(optarg);
            break;
        default:
            USAGE(argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (optind < argc) {
        USAGE(argv[0]);
        exit(EXIT_FAILURE);
    }

    initscr();
    cbreak();
    noecho();
    curs_set(0);

    struct matrix *state = initmatrix(LINES, COLS);

    char *msg = "Welcome to Conway's Game of Life! Press any key to observe.";
    mvprintw(LINES / 2, (COLS - strlen(msg)) / 2, msg);
    refresh();

    getch();
    clear();

    initlife(state);
    for (size_t i = 0; i < iterations; ++i) {
        iterlife(state);
        refresh();
        sleep(delay);
    }

    endwin();
    freematrix(state);

    exit(EXIT_SUCCESS);
}
