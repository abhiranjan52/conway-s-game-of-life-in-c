#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <windows.h>

#define ROWS 35
#define COLUMNS 50
#define ALIVE 1
#define DEAD  0

typedef struct {
    bool state;
} CELL;

void gotoxy(COORD c);
COORD getxy(HANDLE hConsoleOutput);
void wait(double milliseconds);
void print_board(CELL (*cells)[COLUMNS]);
void create_next_frame(CELL (*cells)[COLUMNS]);
void get_seed_from_terminal(CELL (*cells)[COLUMNS], COORD coordinates);
void get_seed_from_file(CELL (*cells)[COLUMNS], FILE *seed_fp);

int main(int argc, char *argv[]) {
    /*
    1. Any live cell with fewer than two live neighbours dies, as if by underpopulation.
    2. Any live cell with two or three live neighbours lives on to the next generation.
    3. Any live cell with more than three live neighbours dies, as if by overpopulation.
    4. Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
    */
    CELL cells[ROWS][COLUMNS];
    for (int i = 0; i < ROWS; ++i)
        for (int j = 0; j < COLUMNS; ++j)
            cells[i][j].state = DEAD;

    COORD coordinates = getxy(GetStdHandle(STD_OUTPUT_HANDLE));
    print_board(cells);

    char choice = 'f';
    printf("Get seed from terminal or file[t/f]? ");
    scanf("%c", &choice);
    if (choice == 't')
        get_seed_from_terminal(cells, coordinates);
    else {
        FILE *seed_fp = fopen("game_of_life_seed.txt", "r");
        get_seed_from_file(cells, seed_fp);
        fclose(seed_fp);
        gotoxy(coordinates);
        print_board(cells);
    }

    int max_gens;
    printf("\nenter number of generations to run for[MAX = 2147483647]: ");
    scanf("%d", &max_gens);
    for (int i = 0; i < max_gens; ++i) {
        gotoxy(coordinates);
        print_board(cells);
        create_next_frame(cells);
        wait(0);
    }

    return 0;
}
void gotoxy(COORD c) {
    static HANDLE h = NULL;  
    if(!h)
        h = GetStdHandle(STD_OUTPUT_HANDLE);  
    SetConsoleCursorPosition(h,c);
}
COORD getxy(HANDLE hConsoleOutput) {
    CONSOLE_SCREEN_BUFFER_INFO cbsi;
    if (GetConsoleScreenBufferInfo(hConsoleOutput, &cbsi)) {
        return cbsi.dwCursorPosition;
    } else {
        COORD invalid = {0, 0};
        return invalid;
    }
}
void wait(double milliseconds) {
    double start = (double) clock();
    while ((double) clock() - start < milliseconds);
}
void print_board(CELL (*cells)[COLUMNS]) {
    printf("0  ");
    for (int i = 0; i < COLUMNS; ++i)
        printf(" %d%c", i+1, i+1 < 10 ? ' ' : '\0');
    putchar('\n');
    for (int i = 0; i < ROWS; ++i) {
        printf("%d%s", i+1, i+1 < 10 ? "  " : " ");
        for (int j = 0; j < COLUMNS; ++j)
            printf("[%c]", cells[i][j].state ? 0xfe : ' ');
        putchar('\n');
    }
}
int get_alive_nbrs(CELL (*cells)[COLUMNS], int x, int y) {
    int res = 0;
    for (int i = x-1; i <= x+1; ++i) {
        if (i < 0 || i >= ROWS)
            continue;
        for (int j = y-1; j <= y+1; ++j) {
            if (j < 0 || j >= COLUMNS || i == x && j == y)
                continue;
            if (cells[i][j].state)
                ++res;
        }
    }
    return res;
}
void create_next_frame(CELL (*cells)[COLUMNS]) {
    static int alive[ROWS*COLUMNS][2];
    static int dead[ROWS*COLUMNS][2];
    int alive_ptr = 0, dead_ptr = 0;
    for (int i = 0; i < ROWS; ++i)
        for (int j = 0; j < COLUMNS; ++j) {
            int nbrs = get_alive_nbrs(cells, i, j);
            if (cells[i][j].state && (nbrs < 2 || nbrs > 3)) {
                dead[dead_ptr][0] = i;
                dead[dead_ptr][1] = j;
                ++dead_ptr;
            } else if (!cells[i][j].state && nbrs == 3) {
                alive[alive_ptr][0] = i;
                alive[alive_ptr][1] = j;
                ++alive_ptr;
            }
        }
    for (int i = 0; i < dead_ptr; ++i)
        cells[dead[i][0]][dead[i][1]].state = DEAD;
    for (int i = 0; i < alive_ptr; ++i)
        cells[alive[i][0]][alive[i][1]].state = ALIVE;
}
void get_seed_from_terminal(CELL (*cells)[COLUMNS], COORD coordinates) {
    int seed;
    printf("enter amount of live cells to start with: ");
    scanf("%d", &seed);
    for (int i = 0; i < seed; ++i) {
        int x, y;
        printf("enter row and column of %dth cell[row column]: ", i+1);
        scanf("%d %d", &x, &y);
        x -= 1, y -= 1;
        if (x < 0 || x >= ROWS || y < 0 || y >= COLUMNS) {
            printf("Out of bounds. Try again.\n");
            i -= 1;
            continue;
        }
        cells[x][y].state = ALIVE;
        gotoxy(coordinates);
        print_board(cells);
    }
}
void get_seed_from_file(CELL (*cells)[COLUMNS], FILE *seed_fp) {
    for (int i = 0; i < ROWS; ++i) {
        char chr;
        for (int j = 0; j < COLUMNS; ++j) {
            fscanf(seed_fp, "[%c]", &chr);
            cells[i][j].state = chr == '#' ? ALIVE : DEAD;
        }
        chr = getc(seed_fp);
    }
}
