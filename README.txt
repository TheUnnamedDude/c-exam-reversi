The solution is split into three main parts, its the board which has the header
board.h and a ncurses implementation called ncurses_board.c, its the game logic
which consists of gamelogic.h and gamelogic.c, the logging is handled by
filelog.c with the header filelog.h and everything is glued together in main.c.

board.h consists of structs and functions used to interact with a board
implementation, its made so you can create new board front-ends in the future
like a pure command-line version or a gtk/qt UI. To enable ncurses the program
must be compiled with -Dncurses as its enabled in board.h and main.c with a
check of variables to make it possible to compile it with a different backend
on a platform without ncurses. The ncurses backend can use colors and special
chars to display the different tiles, the plan was to check if the terminal had
support for colors and disable it if it didn’t, the board works without colors
but it needs to be disabled in the code. I also wanted to implement a way of
interacting with the board that didn’t use the mouse, but I never got around to
implementing it, although it should just be to add it to ncurses_board.c
without any changes in other files.

The game logic is pretty straight forward, it has a method that checks if it is
possible to place a tile of the type passed at the coordinates passed to the
function, and if it can be placed it adds the coordinates to a 2d array so
main.c can replace tiles in the board. And there is a method for checking if a
player can place a tile at all.

Logging is also pretty simple, filelog.c and filelog.h is a simple wrapper 
round FILE and fprintf methods, it saves it to a text file. The format of the
text file is made to be human readable and also easily parsable. It consists of
three wrapping tags, GameInfo, Game and Statistics, the start and end of these
tags are prefixed with Start and End, GameInfo is the information known at the
start of the game, so it contains how big the board is, how many tiles are
placed by default and the names of the two players. Game contains information
about what tiles were successfully clicked(UserClick x,y W/B) and what tiles
were changed(SetTile x,y W/B) where W is a white tile and B is a black tile.
Then you have the Statistics section which contains information about the
scores and how many moves were issued. Parsing can be done by reading all lines
and trimming whitespaces before and after a line. I’ve also added a example log
file in logfile_format.txt.

Everything is glued together in main.c, it handles initialization, the game
loop, input checking, ui updates and logging. The program does not need to
handle lifetimes of memory as everything is allocated before running and free’d
after, so I’ve chosen to not use malloc as everything is only used in the main
method, another reason is because the stack is way faster then the heap even
though it doesn’t really matter in such a small application.
