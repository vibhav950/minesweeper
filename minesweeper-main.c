#include <stdio.h>
#include <stdlib.h>
#include "rand.h"

typedef struct cell {
  int bomb;
  int flag;
  int clicked;
  int value;
} CELL;

typedef struct board {
  int rows;
  int cols;
  CELL **cells;
  int flags;
  int bombs;
} BOARD;

CELL *currboard;
int bombstep = 0;

int checkwin(BOARD *board) {
  CELL *cells = *board->cells;
  if (bombstep)
    return -1;
  for (int i = 0; i < board->rows; i++) {
    for (int j = 0; j < board->cols; j++) {
      if (cells[i * board->cols + j].clicked == 0 &&
          cells[i * board->cols + j].bomb == 0) {
        return 0;
      }
    }
  }
  return 1;
}

void select_difficulty(BOARD *board) {
  printf("Select difficulty:\n");
  printf("1. Easy (9*9 board with 10 bombs)\n");
  printf("2. Medium (16*16 board with 40 bombs)\n");
  printf("3. Hard (16*30 board with 99 bombs)\n");
  printf("4. Custom\n");
  int difficulty;
  do {
    scanf("%d", &difficulty);
    if (difficulty < 1 || difficulty > 4)
      printf("Invalid difficulty. Try again.\n");
  } while (difficulty < 1 || difficulty > 4);
  switch (difficulty) {
  case 1:
    board->rows = 9;
    board->cols = 9;
    break;
  case 2:
    board->rows = 16;
    board->cols = 16;
    break;
  case 3:
    board->rows = 16;
    board->cols = 30;
  case 4:
    do {
      printf("Enter number of rows: ");
      scanf("%d", &board->rows);
      printf("Enter number of columns: ");
      scanf("%d", &board->cols);
      if (board->rows < 1 || board->cols < 1)
        printf("Invalid dimensions. Try again.\n");
    } while (board->rows < 1 || board->cols < 1);
    break;
  }
}

void num_bombs(BOARD *board) {
  if (board->rows == 9 && board->cols == 9) {
    board->bombs = 10;
    board->flags = 10;
  } else if (board->rows == 16 && board->cols == 16) {
    board->bombs = 40;
    board->flags = 40;
  } else if (board->rows == 16 && board->cols == 30) {
    board->bombs = 99;
    board->flags = 99;
  } else {
    int bomb_num = 0;
    printf("Enter custom number of bombs: ");
    do {
      scanf("%d", &bomb_num);
      if (bomb_num < 1 || bomb_num > (board->rows * board->cols))
        printf("Invalid number of bombs. Try again.\n");
    } while (bomb_num > (board->rows * board->cols));
    board->bombs = bomb_num;
    board->flags = bomb_num;
  }
}

int is_valid(BOARD *board, int x, int y) {
  if (x < 0 || x >= board->rows || y < 0 || y >= board->cols) {
    return 0;
  }
  return 1;
}

void place_bombs(BOARD *board) {
  int bombs = board->bombs;
  while (bombs > 0) {
    // int x = rand() % board->rows;
    int x = ranged(0, board->rows);
    // int y = rand() % board->cols;
    int y = ranged(0, board->cols);
    CELL *cells = *board->cells;
    if (cells[x * board->cols + y].bomb == 0) {
      cells[x * board->cols + y].bomb = 1;
      for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
          if (is_valid(board, x + i, y + j)) {
            cells[(x + i) * board->rows + (y + j)].value++;
          }
        }
      }
      bombs--;
    }
  }
}

void generateBoard(BOARD *board) {
  num_bombs(board);
  currboard = malloc(sizeof(CELL) * board->rows * board->cols);
  board->cells = &currboard;
  CELL *cells = *board->cells;
  int i, j;
  for (i = 0; i < board->rows; i++) {
    for (j = 0; j < board->cols; j++) {
      cells[i * board->cols + j].bomb = 0;
      cells[i * board->cols + j].flag = 0;
      cells[i * board->cols + j].clicked = 0;
      cells[i * board->cols + j].value = 0;
    }
  }
  place_bombs(board);
}

void display(BOARD *board, int winloss) {
  CELL *cells = *board->cells;

  printf("   ");
  for (int i = 0; i < board->cols; i++) {
    printf("%d ", i);
  }
  printf("\n");

  for (int i = 0; i < board->rows; i++) {
    printf("%d  ", i);
    for (int j = 0; j < board->cols; j++) {
      if (winloss != 0) {
        if (cells[i * board->cols + j].bomb == 1) {
          printf("B ");
        } else if (cells[i * board->cols + j].flag == 1) {
          printf("F ");
        } else if (cells[i * board->cols + j].value == 0) {
          printf("  ");
        } else {
          printf("%d ", cells[i * board->cols + j].value);
        }
      } else {
        if (cells[i * board->cols + j].flag == 1) {
          printf("F ");
        } else if (cells[i * board->cols + j].clicked == 0) {
          printf("X ");
        } else if (cells[i * board->cols + j].bomb == 1) {
          printf("B ");
        } else if (cells[i * board->cols + j].value == 0) {
          printf("  ");
        } else {
          printf("%d ", cells[i * board->cols + j].value);
        }
      }
    }
    printf("\n");
  }
  printf("Flags remaining: %d\n", board->flags);
}

void click_multi(BOARD *board, int x, int y);

void click(BOARD *board, int x, int y) {
  CELL *cells = *board->cells;

  if (cells[x * board->cols + y].bomb == 1) {
    bombstep = 1;
    return;
  }

  if (cells[x * board->cols + y].flag == 1 ||
      cells[x * board->cols + y].clicked == 1) {
    return;
  }

  cells[x * board->cols + y].clicked = 1;

  if (cells[x * board->cols + y].value == 0) {
    for (int i = -1; i <= 1; i++) {
      for (int j = -1; j <= 1; j++) {
        if (is_valid(board, x + i, y + j)) {
          click(board, x + i, y + j);
        }
      }
    }
  }
}

void click_multi(BOARD *board, int x, int y) {
  CELL *cells = *board->cells;
  int flagcount = 0;
  for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 1; j++) {
      if (is_valid(board, x + i, y + j)) {
        if (cells[(x + i) * board->cols + (y + j)].flag == 1)
          flagcount++;
      }
    }
  }
  if (flagcount != cells[x * board->cols + y].value) {
    printf("Incorect move!\n");
    return;
  }
  for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 1; j++) {
      if (is_valid(board, x + i, y + j) && cells[(x + i) * board->cols + (y + j)].flag == 0) {
        click(board, x + i, y + j);
      }
    }
  }
}

void flag(BOARD *board, int x, int y) {
  CELL *cells = *board->cells;

  if (cells[x * board->cols + y].clicked == 1) {
    printf("Cannot flag a clicked cell.\n");
    return;
  }

  if (cells[x * board->cols + y].flag == 1) {
    cells[x * board->cols + y].flag = 0;
    board->flags++;
  } else {
    cells[x * board->cols + y].flag = 1;
    board->flags--;
  }
}

void move(BOARD *board) {
  printf("Enter move (row, column, f/c) :\n");
  CELL *cells = *board->cells;
  int x, y;
  char f;
  do {
    scanf("%d %d %c", &x, &y, &f);
    if (x < 0 || x >= board->rows || y < 0 || y >= board->cols ||
        (f != 'c' && f != 'f'))
      printf("Invalid move. Try again.\n");
  } while (x < 0 || x >= board->rows || y < 0 || y >= board->cols ||
           (f != 'c' && f != 'f'));
  if (f == 'c') {
    if (cells[x * board->cols + y].flag == 1) {
      printf("Cannot click a flagged cell.\n");
      return;
    }
    if (cells[x * board->cols + y].clicked == 1) {
      click_multi(board, x, y);
    } else {
      click(board, x, y);
    }
  } else {
    flag(board, x, y);
  }
}

int main() {
  randctx();

  char c;
  do {
    BOARD *board = malloc(sizeof(BOARD));
    select_difficulty(board);
    generateBoard(board);
    do {
      display(board, checkwin(board));
      move(board);
    } while (checkwin(board) == 0);
    display(board, checkwin(board));
    if (checkwin(board) == 1) {
      printf("Congratulations! You win!\n");
    } else {
      printf("You lose!\n");
    }
    printf("Play again? (y/n): \n");
    getchar();
    scanf("%c", &c);
    if (c == 'n') {
      break;
    }
    bombstep=0;
    free(board);
  } while (1);
  return 0;
}
