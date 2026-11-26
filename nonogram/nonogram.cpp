#include <iostream>
#include <fstream>

using namespace std;

#define BOARD_SIZE 25

enum CellState {
    EMPTY = 0,
    BLACK = 1,
    UNKNOWN = 2
};

struct Clue {
    int n; // number of clues
    int c[BOARD_SIZE]; // clues
};

struct Board {
    int width;
    int height;
    int *grid; // 1D array (1 byte per cell)
};

ifstream *input = nullptr; // input file
Clue RowClues[BOARD_SIZE];
Clue ColClues[BOARD_SIZE];

Board* CreateBoard() {
    
    Board *board = new Board();
    board->width = BOARD_SIZE;
    board->height = BOARD_SIZE;
    board->grid = new int[BOARD_SIZE * BOARD_SIZE];

    // init to unknown
    for (int i = 0; i < board->height; i++) {

        for (int j = 0; j < board->width; j++) {
            board->grid[i * board->width + j] = UNKNOWN;
        }

    }

    return board;
}

void FreeBoard(Board *board) {

    if (board) {
        delete[] board->grid;
        delete board;
    }

}