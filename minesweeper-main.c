#include "colors.h"
#include "minesweeper.h"
#include "rand.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef __WIN32
#include <conio.h>
#endif

#ifdef __linux__
#include "mswlinuxio.h"
#endif

/* ===================== DEV ===================== */

// Leaderboard doubly linked priority list
typedef struct leader {
  int time;
  char name[20];
  struct leader *next;
  struct leader *previous;
} LEADER;

// function to check if a file exists
int exists(const char *fname) {
  FILE *file;
  if ((file = fopen(fname, "r"))) {
    fclose(file);
    return 1;
  }
  return 0;
}

// Global leaderboard variable
LEADER lboard = {.name = "", .time = 0, .next = NULL, .previous = NULL};

// function to initialize the leaderboard from files
void init_leader() {
  FILE *infile;
  LEADER *temp = &lboard;
  int i = 0;
  while (1) {
    char filename[32];
    snprintf(filename, 32, "leader%d.csv", i);

    if (exists(filename)) {
      infile = fopen(filename, "r");
      if (i == 0) {
        if (temp->next == NULL) {
          temp->next = malloc(sizeof(LEADER));
          temp->next->previous = NULL;
          temp = temp->next;
        }
        fscanf(infile, "%[^,],%d\n", temp->name, &temp->time);
      } else {
        temp->next = malloc(sizeof(LEADER));
        temp->next->previous = temp;
        temp = temp->next;
        fscanf(infile, "%[^,],%d\n", temp->name, &temp->time);
        temp->next = NULL;
      }
      fclose(infile);
    } else {
      break;
    }
    i++;
  }
}

// function to insert a new entry into the leaderboard
void insert_leader(char name[], int time) {
  LEADER *temp = &lboard;

  if (temp->next == NULL) {
    // If the list is empty, insert as the first node
    LEADER *new_leader = malloc(sizeof(LEADER));
    new_leader->time = time;
    strcpy(new_leader->name, name);
    new_leader->next = NULL;
    new_leader->previous = NULL;
    temp->next = new_leader;
    return;
  }

  while (temp->next != NULL && temp->next->time < time) {
    temp = temp->next;
  }

  LEADER *new_leader = malloc(sizeof(LEADER));
  new_leader->time = time;
  strcpy(new_leader->name, name);
  new_leader->next = temp->next;
  new_leader->previous = temp;

  if (temp->next != NULL) {
    temp->next->previous = new_leader;
  }

  temp->next = new_leader;
}

// function to display the leaderboard
void display_leader() {
  printf("\n\n Leaderboard\n");
  LEADER *temp = lboard.next;
  int i = 1;
  while (temp != NULL) {
    printf("%d. %s\t%d\n", i, temp->name, temp->time);
    temp = temp->next;
    i++;
  }
}

// writes all the leaderboard contents to csv files
void write_leader() {
  FILE *outfile;
  LEADER *temp = lboard.next;
  int i = 0;
  while (temp != NULL) {
    char filename[32];
    snprintf(filename, 32, "leader%d.csv", i);
    outfile = fopen(filename, "w");
    fprintf(outfile, "%s,%d\n", temp->name, temp->time);
    fclose(outfile);
    temp = temp->next;
    i++;
  }
}

/* ===================== DEV ===================== */

// Checks the win state. Win is when all the valid cells are clicked
MSW_STATE CheckWin(MSW_CTX *ctx) {
  MSW_CELL *cells = *(ctx->board)->cells;
  MSW_BOARD *board = ctx->board;

  if (ctx->bombstep) {
    return MSW_LOSS;
  }

  for (int i = 0; i < board->rows; i++) {
    for (int j = 0; j < board->cols; j++) {
      if (cells[i * board->cols + j].clicked == 0 &&
          cells[i * board->cols + j].bomb == 0) {
        return MSW_NONE;
      }
    }
  }

  return MSW_WIN;
}

// Prints the difficulty menu
void PrintDifficultyMenu(int diff) {
  system("clear");
  printf("\n\n\n");
  printf("███    ███ ██ ███    ██ ███████ ███████ ██     ██ ███████ ███████ "
         "██████  ███████ ██████\n"
         "████  ████ ██ ████   ██ ██      ██      ██     ██ ██      ██      ██ "
         "  ██ ██      ██   ██\n"
         "██ ████ ██ ██ ██ ██  ██ █████   ███████ ██  █  ██ █████   █████   "
         "██████  █████   ██████\n"
         "██  ██  ██ ██ ██  ██ ██ ██           ██ ██ ███ ██ ██      ██      ██ "
         "     ██      ██   ██\n"
         "██      ██ ██ ██   ████ ███████ ███████  ███ ███  ███████ ███████ ██ "
         "     ███████ ██   ██\n\n");

  switch (diff) {
  case 1:
    printf("%s%s EASY %s  MEDIUM   HARD ", BG_CYAN_LIGHT, FG_BLACK,
           FG_BG_CLEAR);
    break;
  case 2:
    printf(" EASY  %s%s MEDIUM %s  HARD ", BG_CYAN_LIGHT, FG_BLACK,
           FG_BG_CLEAR);
    break;
  case 3:
    printf(" EASY   MEDIUM  %s%s HARD %s", BG_RED_LIGHT, FG_BLACK, FG_BG_CLEAR);
    break;
  }
  printf("\n\n\n\n\n");
}

// Clears the screen
void Clr(void) { printf(" \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n "); }

// Function to select the difficulty based on user input
void SelectDifficulty(MSW_CTX *ctx) {
  char c;
  int difficulty = 1;

  PrintDifficultyMenu(difficulty);

  /* SPACE bar */
  while ((c = _getch()) != 32) {
    switch (c) {
    /* right arrow < */
    case 'a':
    case 'A':
      if (difficulty > 1) {
        difficulty--;
      }
      break;

    /* left arrow > */
    case 'd':
    case 'D':
      if (difficulty < 3) {
        difficulty++;
      }
      break;

    case 'q':
    case 'Q':
      exit(0);

    default:
      break;
    }
    PrintDifficultyMenu(difficulty);
  }

  printf("\n\n");

  MSW_BOARD *board = ctx->board;

  switch (difficulty) {
  case 1:
    board->rows = 9;
    board->cols = 9;
    board->bombs = 10;
    board->flags = 10;
    break;

  case 2:
    board->rows = 16;
    board->cols = 16;
    board->bombs = 40;
    board->flags = 40;
    break;

  case 3:
    board->rows = 16;
    board->cols = 30;
    board->bombs = 99;
    board->flags = 99;
    break;

  /* dummy */
  default:
    exit(0);
  }

  board->sx = board->rows / 2;
  board->sy = board->cols / 2;
  ctx->difficulty = difficulty;
}

// Checks if a move made is valid or not
MSW_BOOL IsValidPos(MSW_BOARD *board, int x, int y) {
  if (x < 0 || x >= board->rows || y < 0 || y >= board->cols) {
    return FALSE;
  } else {
    return TRUE;
  }
}

// Function to randomly place bombs on the board
void PlaceBombs(MSW_CTX *ctx) {
  MSW_BOARD *board = ctx->board;
  int i, j, x, y, bombs = board->bombs;

  while (bombs > 0) {
    x = ranged(0, board->rows - 1); // Generates a random x value
    y = ranged(0, board->cols - 1); // Generates a random y value

    MSW_CELL *cells = *board->cells;

    if (cells[x * board->cols + y].bomb ==
        0) { // Checks if a bomb already exists
      cells[x * board->cols + y].bomb = 1;

      for (i = -1; i <= 1; ++i) { // Iteratively increases the bomb value of all
                                  // cells around the bomb
        for (j = -1; j <= 1; ++j) {
          if (IsValidPos(board, x + i, y + j)) {
            cells[(x + i) * board->cols + (y + j)].value++;
          }
        }
      }
      bombs--;
    }
  }
}

// function to initialise the board with 0 values
void GenerateBoard(MSW_CTX *ctx) {
  MSW_BOARD *board = ctx->board;
  if (!board) {
    exit(2);
  }

  ctx->cur_board =
      (MSW_CELL *)malloc(sizeof(MSW_CELL) * board->rows * board->cols);
  board->cells = &(ctx->cur_board);
  MSW_CELL *cells = *board->cells;
  int i, j;

  for (i = 0; i < board->rows; ++i) {
    for (j = 0; j < board->cols; ++j) {
      cells[i * board->cols + j].bomb = 0;
      cells[i * board->cols + j].flag = 0;
      cells[i * board->cols + j].clicked = 0;
      cells[i * board->cols + j].value = 0;
    }
  }

  PlaceBombs(ctx);
}

// Function to display the board
void Display(MSW_CTX *ctx, int sx, int sy) {
  MSW_BOARD *board = ctx->board;
  MSW_CELL *cells = *board->cells;

  MSW_STATE state = CheckWin(ctx);

  system("clear");

  printf("\n\n\n███    ███ ██ ███    ██ ███████ ███████ ██     ██ ███████ "
         "███████ ██████  ███████ ██████\n"
         "████  ████ ██ ████   ██ ██      ██      ██     ██ ██      ██      ██ "
         "  ██ ██      ██   ██\n"
         "██ ████ ██ ██ ██ ██  ██ █████   ███████ ██  █  ██ █████   █████   "
         "██████  █████   ██████\n"
         "██  ██  ██ ██ ██  ██ ██ ██           ██ ██ ███ ██ ██      ██      ██ "
         "     ██      ██   ██\n"
         "██      ██ ██ ██   ████ ███████ ███████  ███ ███  ███████ ███████ ██ "
         "     ███████ ██   ██\n\n");

  char *bg;

  for (int i = 0; i < board->rows; i++) {
    printf("   ");
    for (int j = 0; j < board->cols; j++) {
      if (state != MSW_NONE) {
        if (cells[i * board->cols + j].bomb == 1) {
          printf("%s%s💣", BG_RED, FG_BLACK);
        } else if (cells[i * board->cols + j].flag == 1) {
          printf("%s%s⚑ ", BG_YELLOW, FG_BLACK);
        } else if (cells[i * board->cols + j].value == 0) {
          printf("%s  ", BG_GRAY);
        } else {
          printf("%s%s%d ", BG_CYAN_LIGHT, FG_MAGENTA_LIGHT,
                 cells[i * board->cols + j].value);
        }
      }

      else {
        if (cells[i * board->cols + j].flag == 1) {
          bg = (i == sx && j == sy) ? BG_BLUE_LIGHT : BG_YELLOW;
          printf("%s%s⚑ ", bg, FG_BLACK);
        } else if (cells[i * board->cols + j].clicked == 0) {
          bg = (i == sx && j == sy) ? BG_BLUE_LIGHT : BG_WHITE;
          printf("%s%s- ", bg, FG_GRAY);
        } else if (cells[i * board->cols + j].bomb == 1) {
          bg = (i == sx && j == sy) ? BG_BLUE_LIGHT : BG_RED;
          printf("%s%s💣", bg, FG_BLACK);
        } else if (cells[i * board->cols + j].value == 0) {
          bg = (i == sx && j == sy) ? BG_BLUE_LIGHT : BG_GRAY;
          printf("%s  ", bg);
        } else {
          bg = (i == sx && j == sy) ? BG_BLUE_LIGHT : BG_CYAN_LIGHT;
          printf("%s%s%d ", bg, FG_MAGENTA_LIGHT,
                 cells[i * board->cols + j].value);
        }
      }
    }
    printf("%s\n", FG_BG_CLEAR);
  }
  printf("\n    %s%sFLAGS%s %2d\t", BG_YELLOW, FG_BLACK, FG_BG_CLEAR,
         board->flags);
  printf("%s%sTIME%s %4ds\n", BG_GRAY, FG_BLACK, FG_BG_CLEAR, 0);
}

// Function that clicks cells recursively
void Click(MSW_CTX *ctx, int x, int y) {
  MSW_BOARD *board = ctx->board;
  MSW_CELL *cells = *board->cells;

  if (cells[x * board->cols + y].bomb == 1) {
    ctx->bombstep = 1;
    return;
  }

  if (cells[x * board->cols + y].flag == 1 ||
      cells[x * board->cols + y].clicked == 1) {
    return;
  }

  cells[x * board->cols + y].clicked = 1;
  // If the cell has no bombs around it, click all the cells around it
  // recursively
  if (cells[x * board->cols + y].value == 0) {
    for (int i = -1; i <= 1; i++) {
      for (int j = -1; j <= 1; j++) {
        if (IsValidPos(board, x + i, y + j)) {
          Click(ctx, x + i, y + j);
        }
      }
    }
  }
}

// Function to click all the cells around a cell, if all expected flags are met
void MultiClick(MSW_CTX *ctx, int x, int y) {
  MSW_BOARD *board = ctx->board;
  MSW_CELL *cells = *board->cells;
  int flagcount = 0;
  // counts the flags around the cell
  for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 1; j++) {
      if (IsValidPos(board, x + i, y + j)) {
        if (cells[(x + i) * board->cols + (y + j)].flag == 1)
          flagcount++;
      }
    }
  }
  if (flagcount != cells[x * board->cols + y].value) {
    return;
  }
  // clicks all the cells around the cell
  for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 1; j++) {
      if (IsValidPos(board, x + i, y + j) &&
          cells[(x + i) * board->cols + (y + j)].flag == 0) {
        Click(ctx, x + i, y + j);
      }
    }
  }
}

// function that places flags on user input
void Flag(MSW_CTX *ctx, int x, int y) {
  MSW_BOARD *board = ctx->board;
  MSW_CELL *cells = *board->cells;

  if (cells[x * board->cols + y].clicked == 1) {
    // printf("Cannot flag a clicked cell.\n");
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

// function to take user input for moves
void Move(MSW_CTX *ctx) {
  MSW_BOARD *board = ctx->board;
  MSW_CELL *cells = *board->cells;
  int sx = board->sx, sy = board->sy;
  char f = 0, ch = 0;

  Display(ctx, sx, sy);

  /* poll for keypresses */
  do {
    ch = _getch();
    switch (ch) {

    case 'w':
    case 'W':
      if (IsValidPos(board, sx - 1, sy)) {
        sx--;
      }
      break;

    case 'a':
    case 'A':
      if (IsValidPos(board, sx, sy - 1)) {
        sy--;
      }
      break;

    case 's':
    case 'S':
      if (IsValidPos(board, sx + 1, sy)) {
        sx++;
      }
      break;

    case 'd':
    case 'D':
      if (IsValidPos(board, sx, sy + 1)) {
        sy++;
      }
      break;

    case 'f':
    case 'F':
      f = 'f';
      break;

    case 'c':
    case 'C':
      f = 'c';
      break;

    case 'q':
    case 'Q':
      exit(0);
    }
    Display(ctx, sx, sy);
  } while (f != 'c' && f != 'f');

  board->sx = sx;
  board->sy = sy;

  /* at this point we have a valid cell and operation selected */

  if (f == 'c') {
    if (cells[sx * board->cols + sy].flag == 1) {
      // printf("Cannot click a flagged cell.\n");
      return;
    }
    if (cells[sx * board->cols + sy].clicked == 1) {
      MultiClick(ctx, sx, sy);
    } else {
      Click(ctx, sx, sy);
    }
  } else {
    Flag(ctx, sx, sy);
  }
}

// creates the minesweeper context
MSW_CTX *GetMswCtx(void) {
  MSW_CTX *newCtx = (MSW_CTX *)malloc(sizeof(MSW_CTX));
  if (!newCtx) {
    return NULL;
  }

  newCtx->board = NULL;
  newCtx->bombstep = 0;
  newCtx->cur_board = NULL;
  newCtx->difficulty = -1;

  return newCtx;
}

// Adds the board to the context
MSW_BOOL NewBoard(MSW_CTX *ctx) {
  if (ctx->board)
    free(ctx->board);
  ctx->board = (MSW_BOARD *)malloc(sizeof(MSW_BOARD));

  if (!(ctx->board)) {
    return FALSE;
  } else {
    return TRUE;
  }
}

// main function
int main(void) {
  int timer = 0; // sets the timer to 0
  init_leader(); // loads the leaderboard
  system("clear");
  randctx();

  MSW_CTX *ctx = GetMswCtx(); // creates the context
  if (!ctx) {
    exit(2);
  }
  char c;

  do {
    if (!NewBoard(ctx)) {
      exit(2);
    }
    SelectDifficulty(ctx);
    GenerateBoard(ctx);
    time_t start = time(NULL); // gets start time
    do {                       // game loop
      Move(ctx);
    } while (CheckWin(ctx) == MSW_NONE);

    Display(ctx, -1, -1);
    if (CheckWin(ctx) == MSW_WIN) { // win checker
      printf("\n\n"
             "██╗   ██╗ ██████╗ ██╗   ██╗    ██╗    ██╗██╗███╗   ██╗██╗\n"
             "╚██╗ ██╔╝██╔═══██╗██║   ██║    ██║    ██║██║████╗  ██║██║\n"
             " ╚████╔╝ ██║   ██║██║   ██║    ██║ █╗ ██║██║██╔██╗ ██║██║\n"
             "  ╚██╔╝  ██║   ██║██║   ██║    ██║███╗██║██║██║╚██╗██║╚═╝\n"
             "   ██║   ╚██████╔╝╚██████╔╝    ╚███╔███╔╝██║██║ ╚████║██╗\n"
             "   ╚═╝    ╚═════╝  ╚═════╝      ╚══╝╚══╝ ╚═╝╚═╝  ╚═══╝╚═╝\n\n");
      printf("Time taken is : %d seconds\n",
             (timer = (int)(time(NULL) - start))); // saves time taken
      char name[20];
      printf("Enter your name: ");
      scanf("%s", name);
      insert_leader(name, timer); // inserts into leaderboard
      write_leader();
      display_leader();
    } else {
      printf("\n\n"
             "▓██   ██▓ ▒█████   █    ██     ██▓     ▒█████    ██████ ▓█████\n"
             " ▒██  ██▒▒██▒  ██▒ ██  ▓██▒   ▓██▒    ▒██▒  ██▒▒██    ▒ ▓█   ▀ \n"
             "  ▒██ ██░▒██░  ██▒▓██  ▒██░   ▒██░    ▒██░  ██▒░ ▓██▄   ▒███   \n"
             "  ░ ▐██▓░▒██   ██░▓▓█  ░██░   ▒██░    ▒██   ██░  ▒   ██▒▒▓█  ▄ \n"
             "  ░ ██▒▓░░ ████▓▒░▒▒█████▓    ░██████▒░ ████▓▒░▒██████▒▒░▒████▒\n"
             "   ██▒▒▒ ░ ▒░▒░▒░ ░▒▓▒ ▒ ▒    ░ ▒░▓  ░░ ▒░▒░▒░ ▒ ▒▓▒ ▒ ░░░ ▒░ ░\n"
             " ▓██ ░▒░   ░ ▒ ▒░ ░░▒░ ░ ░    ░ ░ ▒  ░  ░ ▒ ▒░ ░ ░▒  ░ ░ ░ ░  ░\n"
             " ▒ ▒ ░░  ░ ░ ░ ▒   ░░░ ░ ░      ░ ░   ░ ░ ░ ▒  ░  ░  ░     ░   \n"
             " ░ ░         ░ ░     ░            ░  ░    ░ ░        ░     ░  ░\n"
             " ░ ░                                                           "
             "\n\n");
      printf("Time taken is : %.0f seconds\n", (double)(time(NULL) - start));
      display_leader();
    }

    fflush(stdin);
    printf("Play again? (Y/N): ");
    fflush(stdin);
    scanf("%c", &c);
    if (c == 'N' || c == 'n') {
      break;
    }
    ctx->bombstep = 0;
  } while (1);

  return 0;
}
