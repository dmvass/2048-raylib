#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include "board.h"

//-------------------------------------------------------------------------------------------------
// Types and Structures Definition
//-------------------------------------------------------------------------------------------------
typedef struct {
    unsigned int score;      // Current score value
    unsigned int best;       // Best score value
    unsigned int moves;      // Current moves counter
    unsigned int max;        // Highest tile value
    bool win;                // Set true if tile 2048 was achieved
    enum { GAME_PLAY, GAME_WIN, GAME_OVER } state;
    Board board;
} Game;

//-------------------------------------------------------------------------------------------------
// Functions Declaration
//-------------------------------------------------------------------------------------------------
Game * GetGame(void);
void NewGame(void);
void InitGame(void);
void UnloadGame(void);

#endif  //GAME_H
