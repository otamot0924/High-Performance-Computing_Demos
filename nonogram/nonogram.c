// 25x25 nonogram solver

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define BOARD_SIZE 25
#define MAX_CLUES (BOARD_SIZE + 1) / 2 // 1 1 1 1 ... 1
#define PUZZLE_COUNT 1000

typedef enum {
    EMPTY = 0,
    BLACK = 1,
    UNKNOWN = 2
} CellState;

typedef struct {
    int count; // r = number of black segments in the line
    int clue[MAX_CLUES]; // d = lengths of the ith black segments
} Clue; // description

typedef struct {
    CellState board[BOARD_SIZE][BOARD_SIZE]; // i = row, j = col

    Clue rowClues[BOARD_SIZE];
    Clue colClues[BOARD_SIZE];
} Puzzle;

FILE *input;

Puzzle *CreatePuzzle() {

    Puzzle *puzzle = malloc(sizeof(*puzzle));

    // init board to unknown
    for (int row = 0; row < BOARD_SIZE; row++) {

        for (int col = 0; col < BOARD_SIZE; col++) {
            puzzle->board[row][col] = UNKNOWN;
        }

    }

    printf("puzzle created\n");

    return puzzle;

}

CellState GetCell(Puzzle *puzzle, int row, int col) {
    return puzzle->board[row][col];
}

void SetBoard(Puzzle *puzzle, int row, int col, int state) {
    puzzle->board[row][col] = state;
}

void CopyRowToLine(Puzzle *puzzle, int row, CellState line[BOARD_SIZE]) {

    for (int col = 0; col < BOARD_SIZE; col++) {
        line[col] = puzzle->board[row][col];
    }

}

void CopyColToLine(Puzzle *puzzle, int col, CellState line[BOARD_SIZE]) {

    for (int row = 0; row < BOARD_SIZE; row++) {
        line[row] = puzzle->board[row][col];
    }
    
}

// read clues for current puzzle, former 25 = up to down, later 25 = left to right
void ReadClues(int puzzleNum, Puzzle *puzzle) {

    printf("reading clues for puzzle number: %d\n", puzzleNum);

    char line[256];

    // read a line
    while (fgets(line, sizeof(line), input) != NULL) {

        // puzzle number found
        if (line[0] == '$') {

            int num = 0;

            if (sscanf(line + 1, "%d", &num) == 1) {

                if (num == puzzleNum) {

                    // store clues (first 25 lines are row clues, later 25 lines are column clues)
                    for (int i = 0; i < BOARD_SIZE; i++) {

                        // former 25 for row clues
                        if (fgets(line, sizeof(line), input) == NULL) {
                            break;
                        }

                        int clueCount = 0;

                        char *token = strtok(line, "\t\n"); // split line into tokens

                        while (token != NULL && clueCount < 25) {

                            puzzle->rowClues[i].clue[clueCount] = atoi(token);
                            clueCount++;
                            token = strtok(NULL, "\t\n");

                        }

                        puzzle->rowClues[i].count = clueCount;

                    }

                    // later 25 for column clues
                    for (int j = 0; j < BOARD_SIZE; j++) {

                        if (fgets(line, sizeof(line), input) == NULL) {
                            break;
                        }

                        int clueCount = 0;
                        char *token = strtok(line, "\t\n"); // split line into tokens

                        while (token != NULL && clueCount < BOARD_SIZE) {

                            puzzle->colClues[j].clue[clueCount] = atoi(token);
                            clueCount++;
                            token = strtok(NULL, "\t\n");

                        }

                        puzzle->colClues[j].count = clueCount;

                    }

                    printf("clues read successfully for puzzle number: %d\n", puzzleNum);
                    
                    break;

                }

            }

        }

    }

}

void PrintBoard(Puzzle *puzzle) {

    for (int row = 0; row < BOARD_SIZE; row++) {

        for (int col = 0; col < BOARD_SIZE; col++) {

            CellState cell = GetCell(puzzle, row, col);

            if (cell == EMPTY) {
                printf("0");
            } else if (cell == BLACK) {
                printf("1");
            } else {
                printf("u");
            }

            if (col != BOARD_SIZE - 1) {
                printf("\t");
            }
        }

        printf("\n");
    }

}

void PrintLine(CellState line[BOARD_SIZE]) {

    for (int i = 0; i < BOARD_SIZE; i++) {

        CellState cell = line[i];

        if (cell == EMPTY) {
            printf("0");
        } else if (cell == BLACK) {
            printf("1");
        } else {
            printf("u");
        }
        
    }

    printf("\n");

}

void ShowExeTime(time_t start, time_t end) {
    double exeTime = (double)(end - start) / CLOCKS_PER_SEC;
    printf("%6.3f s\n", exeTime);
}

int GetLen(int *s) {
    return sizeof(s) / sizeof(s[0]);
}

char* Status(int *G) {

}

int *Concat(int *s, int *t) { // concat 2 arrays

    int *result = malloc(GetLen(s) + GetLen(t));

    for (int i = 0; i < GetLen(s); i++) {
        result[i] = s[i];
    }

    for (int i = 0; i < GetLen(t); i++) {
        result[GetLen(s) + i] = t[i];
    }

    return result;
}

bool Fixable(int *s, int *d) { // u1u0 is fixable when d = [3] (1110)

}

int Fix(int *s, int *d, int i, int j) { // Si denote string s1s2...si; Dj denote desc <d1, d2, ..., dj>

    if (i == 0 && j == 0) {
        return true;
    }
    else if (i == 0 && j >= 1) {
        return false;
    }
    else {
        return Fix0(s, d, i, j) || Fix1(s, d, i, j);
    }

}

int Fix0(int *s, int *d, int i, int j) { // guess 0
    
    if (s[i] == 0 || s[i] == 2) {
        return Fix(s, d, i - 1, j);
    }
    else {
        return false;
    }

}

int Fix1(int *s, int *d, int i, int j) { // guess 1

    if (j >= 1 && i >= d[j] + 1 && s[i - d[j]]) { // matches that pack of things (?)
        return Fix(s, d, i - d[j] - 1, j - 1);
    }
    else {
        return false;
    }
}

int *Paint(int *s, int *d, int i, int j) {

    if (i == 0) {
        // no intersection
        return;
    }
    else {
        return PaintP(s, d, i, j);
    }

}

int *PaintP(int *s, int *d, int i, int j) {

    if (Fix0(s, d, i, j) && !Fix1(s, d, i, j)) {
        return Paint0(s, d, i, j);
    }
    else if (!Fix0(s, d, i, j) && Fix1(s, d, i, j)) {
        return Paint1(s, d, i, j);
    }
    else {
        return Merge(Paint0(s, d, i, j), Paint1(s, d, i, j));
    }

}

int *Paint0(int *s, int *d, int i, int j) {

    int empty[] = {EMPTY};

    return Concat(Paint(s, d, i - 1, j), empty);
}

int *Paint1(int *s, int *d, int i, int j) {

    int pack[] = Pack(d[j]);

    return Concat(Paint(s, d, i - d[j] - 1, j - 1), pack); // + pack(d[j])
}

int *Merge(int *s, int *t) {

    int m[BOARD_SIZE];

    for (int k = 0; k < GetLen(s); k++) {
        m[k] = MergeC(s[k], t[k]);
    }

    free(s);
    free(t);

    return m;

}

int MergeC(int s, int t) { // get intersection from 2 strings

    if (s == t) {

        if (t == EMPTY) {
            return EMPTY;
        }
        else if (t == BLACK) {
            return BLACK;
        }

    }
    else {
        return UNKNOWN;
    }

}

void Propagate(int *G) {

}

void main() {

    input = fopen("nonogram_input.txt", "r");
    
    if (input == NULL) {
        printf("error opening file\n");
        return;
    }

    time_t start = clock();

    Puzzle *puzzle = CreatePuzzle();
    CellState line[BOARD_SIZE];

    ReadClues(1, puzzle);
    PrintBoard(puzzle);
    CopyRowToLine(puzzle, 0, line);
    PrintLine(line);

    time_t end = clock();

    ShowExeTime(start, end);

    
    fclose(input);

}