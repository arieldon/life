#include <locale.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CELL_LIVE       ACS_BLOCK
#define CELL_DEAD       ACS_BULLET
#define USAGE(argv0)    fprintf(stderr, "usage: %s " \
                                "[-c] [-d nsecs] [-i niters]\n", \
                                argv0)

static int initcustom = 0;  /* Customize initial state via mouse. */

static size_t
count_neighbors(size_t row, size_t col)
{
    size_t neighbors = 0;

    /*
     * mvwinch simply returns ERR if passed a position out-of-bounds -- no
     * segmentation fault here.
     */
    neighbors += mvwinch(curscr, row - 1, col) == CELL_LIVE;
    neighbors += mvwinch(curscr, row - 1, col + 1) == CELL_LIVE;
    neighbors += mvwinch(curscr, row, col + 1) == CELL_LIVE;
    neighbors += mvwinch(curscr, row + 1, col + 1) == CELL_LIVE;
    neighbors += mvwinch(curscr, row + 1, col) == CELL_LIVE;
    neighbors += mvwinch(curscr, row + 1, col - 1) == CELL_LIVE;
    neighbors += mvwinch(curscr, row, col - 1) == CELL_LIVE;
    neighbors += mvwinch(curscr, row - 1, col - 1) == CELL_LIVE;

    return neighbors;
}

static void
initlife()
{
    for (ssize_t row = 0; row < LINES; ++row) {
        for (ssize_t col = 0; col < COLS; ++col) {
            mvaddch(row, col, CELL_DEAD);
        }
    }
    refresh();

    if (initcustom) {
        MEVENT event = {0};
        mousemask(BUTTON1_CLICKED, NULL);

        for (;;) {
            switch (getch()) {
            case KEY_MOUSE:
                if (getmouse(&event) == OK && event.bstate & BUTTON1_CLICKED) {
                    if (mvinch(event.y, event.x) == CELL_DEAD) {
                        addch(CELL_LIVE);
                    } else {
                        addch(CELL_DEAD);
                    }
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

    mvwaddch(newscr, centery - 1, centerx, CELL_LIVE);
    mvwaddch(newscr, centery, centerx + 1, CELL_LIVE);
    mvwaddch(newscr, centery + 1, centerx + 1, CELL_LIVE);
    mvwaddch(newscr, centery + 1, centerx, CELL_LIVE);
    mvwaddch(newscr, centery + 1, centerx - 1, CELL_LIVE);

    refresh();
}

static void
iterlife()
{
    size_t neighbors;

    for (ssize_t row = 0; row < LINES; ++row) {
        for (ssize_t col = 0; col < COLS; ++col) {
            neighbors = count_neighbors(row, col);

            if (mvwinch(curscr, row, col) == CELL_DEAD && neighbors == 3) {
                mvwaddch(newscr, row, col, CELL_LIVE);
            } else if (mvwinch(curscr, row, col) == CELL_LIVE
                    && (neighbors == 2 || neighbors == 3)) {
                mvwaddch(newscr, row, col, CELL_LIVE);
            } else {
                mvwaddch(newscr, row, col, CELL_DEAD);
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
        case 'h':
            USAGE(argv[0]);
            exit(EXIT_SUCCESS);
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

    notify("Welcome to Conway's game of Life! Press any key to continue.");
    if (initcustom) {
        notify("To customize the intial state, toggle cells with a click.");
    }

    initlife();
    for (size_t i = 0; i < iterations; ++i) {
        iterlife();
        sleep(delay);
        refresh();
    }

    endwin();
    exit(EXIT_SUCCESS);
}
