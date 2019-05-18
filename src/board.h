#ifndef BOARD_H
#define BOARD_H

#include "raylib.h"

#define SIZE 4
#define GRID_SIZE (SIZE * SIZE)

//-------------------------------------------------------------------------------------------------
// Types and Structures Definition
//-------------------------------------------------------------------------------------------------
typedef struct {
    unsigned int x;
    unsigned int y;
} CellVector;

typedef struct _tile {
    unsigned int value;
    unsigned int oldValue;
    CellVector position;
    CellVector oldPosition;
    struct _tile *source;    // Pointer on a merged tile
} Tile;

typedef struct {
    unsigned int moveFrames;
    unsigned int appearFrames;
    enum { BOARD_STATE_NONE, BOARD_STATE_MOVED, BOARD_STATE_MERGED } state;
    enum { ANIMATION_NONE, ANIMATION_MOVE, ANIMATION_APPEAR } animation;
    Tile grid[GRID_SIZE];
} Board;

//-------------------------------------------------------------------------------------------------
// Functions Declaration
//-------------------------------------------------------------------------------------------------
void InitBoard(Rectangle *rec);
void UpdateBoard(Board *board);
void DrawBoard(Board *board);
void HandleBoardInput(Board *board);
void ResetBoard(Board *board);
bool MoveIsAvailable(Board *board);

#endif  // BOARD_H
