# Life
Observe Conway's Game of Life through a terminal window.


## Conway's Game of Life
[Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) -- or the Game of
Life -- is a zero-player game designed by John Conway; that is, it isn't so
much played as it is observed.

The rules of the game along with its initial state dictate its evolution.

    - Any live cell with two or three live neighbours survives.
    - Any dead cell with three live neighbours becomes a live cell.
    - All other cells remain unchanged.

In this implementation, the initial state can be customized using the `-c`
flag. The user is then presented an unpopulated grid with the ability to toggle
the state of any cell with a mouse click. Without customization, it plants a
[glider](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) in the center
of the grid. A glider is a particular figure that moves across the game's grid.
It is also the smallest spaceship, which is any group of live cells that
present a pattern after a given number of iterations.
