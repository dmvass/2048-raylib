#include <stdio.h>      // fopen, fread, fwrite, fclose, fseek, SEEK_SET
#include <sys/param.h>  // PATH_MAX
#include "raylib.h"
#include "game.h"
#include "observer.h"
#include "resources.h"
#include "utils.h"

//-------------------------------------------------------------------------------------------------
// Local Variables Definition
//-------------------------------------------------------------------------------------------------
static Game game;
static FILE *file;

//-------------------------------------------------------------------------------------------------
// Local Functions Declaration
//-------------------------------------------------------------------------------------------------
static int SaveGame(void);
static int LoadGame(void);

//-------------------------------------------------------------------------------------------------
// Local Observer Functions Declaration
//-------------------------------------------------------------------------------------------------
static void SavingObserver(Event);
static void GameWinObserver(Event);
static void GameOverObserver(Event);

//-------------------------------------------------------------------------------------------------
// Functions Definition
//-------------------------------------------------------------------------------------------------
Game * GetGame(void) { return &game; }

void NewGame(void)
{
    TraceLog(LOG_INFO, "Start new game");

    GetGame()->score = 0;
    GetGame()->moves = 0;
    GetGame()->state = GAME_PLAY;

    ResetBoard(&GetGame()->board);
    SaveGame();
}

void InitGame(void)
{
    MakeSaveDir(saveDirPath);  // Create save data directory if not exist

    if ((file = fopen(saveFilePath, "rb+")) == NULL)
    {
        file = fopen(saveFilePath, "wb+");
    }

    if (LoadGame() != 0 || !MoveIsAvailable(&GetGame()->board))
    {
        NewGame();
    }

    AttachObserver(*SavingObserver);
    AttachObserver(*GameWinObserver);
    AttachObserver(*GameOverObserver);
}

void UnloadGame(void)
{
    TraceLog(LOG_DEBUG, "Unload Game");

    DetachObserver(*SavingObserver);
    DetachObserver(*GameWinObserver);
    DetachObserver(*GameOverObserver);

    fclose(file);
    TraceLog(LOG_INFO, "Close save file");
}

//-------------------------------------------------------------------------------------------------
// Local Functions Definition
//-------------------------------------------------------------------------------------------------
static int SaveGame(void)
{
    fseek(file, 0, SEEK_SET);

    if (fwrite(GetGame(), sizeof(Game), 1, file) == 0)
    {
        TraceLog(LOG_WARNING, "Error writing file");
        return -1;
    }
    TraceLog(LOG_INFO, "Game was saved successfully");
    return 0;
}

static int LoadGame(void)
{
    if (fread(GetGame(), sizeof(Game), 1, file) == 0)
    {
        TraceLog(LOG_WARNING, "Error reading file");
        return -1;
    }
    TraceLog(LOG_INFO, "Game was loaded successfully");
    return 0;
}

// Save tha Game if tiles in the Board were moved or merged.
static void SavingObserver(Event event)
{
    if (event == ADD_TILE_EVENT || event == GAME_OVER_EVENT)
    {
        SaveGame();
    }
}

/*
 * Check that game is won and display a specified screen,
 * this screen should be displayed once when the best tile
 * score is 2048.
 */
static void GameWinObserver(Event event)
{
    if (event == ADD_TILE_EVENT && !GetGame()->win && GetGame()->max == 11)
    {
        GetGame()->win   = true;
        GetGame()->state = GAME_WIN;
    }
}

// Change the Game state on Game Over.
static void GameOverObserver(Event event)
{
    if (event == GAME_OVER_EVENT && GetGame()->state != GAME_OVER)
    {
        GetGame()->state = GAME_OVER;
    }
}
