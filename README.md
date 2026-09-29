# Library Management Program

This is a command-file-driven library simulator written in C. It keeps track of
genres, books, members, loans, returns and ratings. It can also print genre/member 
information, search and rename books, show recommendations, and allocate display 
slots to genres.

The program reads commands from a plain-text file, one command per line. 
It does not prompt interactively. Lines beginning with `#` and blank lines
are ignored. Put double quotes around names and titles, especially when they 
contain spaces.

## Build and Run

Install GCC, then open a terminal in the project folder, where `main.c` and 
`example_commands.txt` are located.

Compile and run the example on Windows:

```powershell
gcc -std=c11 -Wall -Wextra main.c -o library.exe
.\library.exe .\example_commands.txt
```

On Linux or macOS:

```sh
gcc -std=c11 -Wall -Wextra main.c -o library
./library example_commands.txt
```

Run the compile command whenever you change the source code. 
Replace `example_commands.txt` with the path to your own command file when 
running the program. The program prints a result for each command as it 
processes the file. Successful changes usually print `DONE`; 
rejected changes usually print `IGNORED`. Printing and query commands have
their own output.

## Example Input File

Create a text file named `example_commands.txt` in the same folder as the 
program and paste in these lines. This repository also includes this example 
file, so you can run it directly after compiling.

```text
# Configure three display places
S 3

# Create genres: G genre_id "genre name"
G 10 "Fiction"
G 20 "Science"

# Add books: BK book_id genre_id "book title"
BK 101 10 "The Long Road"
BK 102 10 "Glass Harbor"
BK 201 20 "Small Worlds"

# Add a member, loan books, then return and rate one
M 1 "Avery Stone"
L 1 101
R 1 101 9 ok
L 1 201
R 1 201 NA ok

# Print/query commands
PG 10
PM 1
F "The Long Road"
TOP 3
AM

# Allocate the configured places, then print the display
D
PD

# Change a title and show the updated genre listing
U 102 "Harbor Glass"
PG 10
```

The example creates the genres and records before issuing commands that depend on them; use the same ordering in your own input file.

## Commands (If the code blocks or commands appear misaligned, zoom out in your browser or text viewer.)

| Command | Format                                       | What it does                                                                  |
| ------- | -------------------------------------------- | ----------------------------------------------------------------------------- |
| `S`     | `S <places>`                                 | Sets the number of display places used by `D`.                                |
| `G`     | `G <genre_id> "<name>"`                      | Creates a genre. Genre IDs must be unique.                                    |
| `BK`    | `BK <book_id> <genre_id> "<title>"`          | Adds a book to an existing genre. Book IDs must be unique across the library. |
| `M`     | `M <member_id> "<name>"`                     | Adds a member. Member IDs must be unique.                                     |
| `L`     | `L <member_id> <book_id>`                    | Loans a book to an existing member if that member does not already have it.   |
| `R`     | `R <member_id> <book_id> <score> ok`         | Returns a loan and records an integer rating from 0 through 10.               |
| `R`     | `R <member_id> <book_id> NA ok`              | Returns a loan without recording a rating.                                    |
| `R`     | `R <member_id> <book_id> <score-or-NA> lost` | Marks book as lost. Does not remove member's active loan in current version.  |
| `PG`    | `PG <genre_id>`                              | Prints the books in a genre as `book_id, average_rating`.                     |
| `PM`    | `PM <member_id>`                             | Prints IDs of member's active loans, or `Loans:.` if none or missing.         |
| `PD`    | `PD`                                         | Prints the display produced by the most recent `D` command.                   |
| `D`     | `D`                                          | Allocates places among genres based on valid ratings. Run before `PD`.        |
| `F`     | `F "<title>"`                                | Searches for exact title and prints matching book/rating or `NOT FOUND`.      |
| `TOP`   | `TOP <count>`                                | Prints up to requested books in recommendations structure (needs ratings).    |
| `AM`    | `AM`                                         | Prints the member or members with the highest recorded activity score.        |
| `U`     | `U <book_id> "<new title>"`                  | Changes a book's title.                                                       |
| `BF`    | `BF`                                         | Frees library data structures (not needed in normal input files).             |

## Notes

- Commands are case-sensitive and must use the formats shown above.
- A book must be added to a genre before it can be loaned. A member and an active loan must exist before a return can be processed.
- `F` and `U` currently search only the first genre in the genre list (the genre with the lowest ID). For reliable searches and title 
  changes, place the target book in that genre.
- `D` uses ratings recorded by `R ... <score> ok`. With no rated books, the display can be empty.
- The library exists only for one program run. The input file is not modified, and data is not saved between runs.