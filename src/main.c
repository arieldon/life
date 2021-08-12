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

static int initcustom = 0;  /* Customize initial state via mouse. */

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
    if (initcustom) {
        for (ssize_t row = 0; row < LINES; ++row) {
            for (ssize_t col = 0; col < COLS; ++col) {
                mvaddch(row, col, CELL_DEAD);
            }
        }

        MEVENT event = {0};
        mousemask(BUTTON1_CLICKED, NULL);

        for (;;) {
            switch (getch()) {
            case KEY_MOUSE:
                if (getmouse(&event) == OK && event.bstate & BUTTON1_CLICKED) {
                    state->items[event.y][event.x] = 1;
                    mvaddch(event.y, event.x, CELL_LIVE);
                    refresh();
                }
                break;
            default:
                return;
            }
        }
    }

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

static void
notify(char *msg)
{
    mvprintw(LINES / 2, (COLS - strlen(msg)) / 2, msg);
    getch();
    clear();
}

int
main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");
    srand(time(NULL));

    int opt;
    size_t delay = 1;
    size_t iterations = 100;

    while ((opt = getopt(argc, argv, "cd:i:")) != -1) {
        switch (opt) {
        case 'c':
            initcustom = 1;
            break;
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
    keypad(stdscr, TRUE);
    curs_set(0);

    struct matrix *state = initmatrix(LINES, COLS);

    notify("Welcome to Conway's game of Life! Press any key to continue.");
    if (initcustom) {
        notify("To customize the intial state, toggle cells with a click.");
    }

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
