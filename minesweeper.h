#pragma once

typedef enum _MSW_STATE_EN { FALSE, TRUE } MSW_BOOL;
typedef enum _MSW_BOOL_EN { MSW_WIN, MSW_LOSS, MSW_NONE } MSW_STATE;

typedef struct _CELL_ST
{
  unsigned char bomb : 1;
  unsigned char flag : 1;
  unsigned char clicked : 1;
  unsigned char hover : 1;
  unsigned char value : 4;
} MSW_CELL;

typedef struct _BOARD_ST
{
  int rows;
  int cols;
  int flags;
  int bombs;
  MSW_CELL **cells;
  int sx;
  int sy;
} MSW_BOARD;

typedef struct _CTX_ST
{
  MSW_BOOL bombstep;
  int difficulty;
  MSW_BOARD *board;
  MSW_CELL *cur_board;
} MSW_CTX;

/* Leaderboard doubly linked priority list */
typedef struct _LEADER_ST
{
  int time;
  char name[20];
  struct _LEADER_ST *next;
  struct _LEADER_ST *previous;
} LEADER;

MSW_STATE CheckWin (MSW_CTX *ctx);
void SelectDifficulty(MSW_CTX *ctx);
void SetBombs (MSW_CTX *ctx);
MSW_BOOL IsValidPos (MSW_BOARD *board, int x, int y);
void PlaceBombs (MSW_CTX *ctx);
void GenerateBoard (MSW_CTX *ctx);
void Display (MSW_CTX *ctx, int sx, int sy);
void Click (MSW_CTX *ctx, int x, int y);
void MultiClick (MSW_CTX *ctx, int x, int y);
void Flag(MSW_CTX *ctx, int x, int y);
void Move (MSW_CTX *ctx);
MSW_CTX *GetMswCtx (void);
MSW_BOOL NewBoard (MSW_CTX *ctx);

/* Leaderboard functions */
LEADER *ChooseLeader(int difficulty);
void InitLeader(int difficulty);
void InsertLeader(char name[], int time, int difficulty);
void DisplayLeader(int difficulty);
void WriteLeader(int difficulty);
