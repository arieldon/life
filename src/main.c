#include <locale.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "matrix.h"

#define CELL_LIVE           ACS_BLOCK
#define CELL_DEAD           ACS_BULLET
#define INBOUNDS(m, y, x)   (x < m->cols && x > 0 && y < m->rows && y > 0)
#define NEIGHBOR(m, y, x)   (INBOUNDS(m, y, x) && m->items[y][x])

static size_t
count_neighbors(struct matrix *m, size_t row, size_t col)
{
    size_t neighbors = 0;

    neighbors += NEIGHBOR(m, row - 1, col);
    neighbors += NEIGHBOR(m, row - 1, col + 1);
    neighbors += NEIGHBOR(m, row, col + 1);
    neighbors += NEIGHBOR(m, row + 1, col + 1);
    neighbors += NEIGHBOR(m, row + 1, col);
    neighbors += NEIGHBOR(m, row + 1, col - 1);
    neighbors += NEIGHBOR(m, row, col - 1);
    neighbors += NEIGHBOR(m, row - 1, col - 1);

    return neighbors;
}

static void
initlife(struct matrix *current)
{
    size_t centerx = COLS / 2;
    size_t centery = LINES / 2;

    current->items[centery - 1][centerx] = 1;
    current->items[centery][centerx + 1] = 1;
    current->items[centery + 1][centerx + 1] = 1;
    current->items[centery + 1][centerx] = 1;
    current->items[centery + 1][centerx - 1] = 1;
}

static void
iterlife(struct matrix *current, struct matrix *next)
{
    size_t *cell;
    size_t neighbors;

    for (size_t row = 0; row < current->rows; ++row) {
        for (size_t col = 0; col < current->cols; ++col) {
            cell = &current->items[row][col];
            neighbors = count_neighbors(current, row, col);

            if (!*cell && neighbors == 3) {
                next->items[row][col] = 1;
            } else if (*cell && (neighbors == 2 || neighbors == 3)) {
                next->items[row][col] = 1;
            } else {
                next->items[row][col] = 0;
            }
        }
    }
}

int
main(void)
{
    setlocale(LC_ALL, "");
    srand(time(NULL));

    initscr();
    cbreak();
    noecho();
    curs_set(0);

    size_t delay = 1;
    size_t iterations = 100;
    struct matrix *current = initmatrix(LINES, COLS);
    struct matrix *next = initmatrix(LINES, COLS);

    char *msg = "Welcome to Conway's Game of Life! Press any key to observe.";
    mvprintw(LINES / 2, (COLS - strlen(msg)) / 2, msg);
    refresh();

    getch();
    clear();

    initlife(current);
    for (size_t i = 0; i < iterations; ++i) {
        for (ssize_t line = 0; line < LINES; ++line) {
            for (ssize_t col = 0; col < COLS; ++col) {
                mvaddch(line, col,
                    current->items[line][col] ? CELL_LIVE : CELL_DEAD);
            }
        }

        iterlife(current, next);
        refresh();
        sleep(delay);

        struct matrix **tmp = &current;
        current = next;
        next = *tmp;
    }

    endwin();
    freematrix(current);
    freematrix(next);

    exit(EXIT_SUCCESS);
}
