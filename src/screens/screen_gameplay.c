#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>

#include "raylib.h"
#include "screens.h"

#define TILE_MAX_BUFFER_SIZE  12
#define TILE_MAX_COLOR_INDEX  12

#define COLOR_GRID_BG     CLITERAL{ 186, 173, 161, 255 }
#define COLOR_CELL_BG     CLITERAL{ 204, 193, 181, 245 }
#define COLOR_TILE_WHITE  CLITERAL{ 249, 246, 242, 255 }
#define COLOR_TILE_GREY   CLITERAL{ 119, 110, 102, 255 }

#define MILISEC  0.000001

#define SAVE_DIR   "/Library/Application Support/2048"
#define SAVE_FILE  "/save.dat"

//-------------------------------------------------------------------------------------------------
// Gameplay Types and Structures Definition
//-------------------------------------------------------------------------------------------------
typedef struct {
    unsigned int x;
    unsigned int y;
} CellVector;

typedef struct _Tile {
    unsigned int value;
    unsigned int oldValue;

    CellVector position;
    CellVector oldPosition;

    struct _Tile *source;  // Pointer on a merged tile
} Tile;

struct Game {
    unsigned int score;      // Current score value
    unsigned int best;       // Best score value
    unsigned int maxTile;    // Highest tile value
    unsigned int moves;      // Current moves counter
    bool win;                // Set true if tile 2048 was achieved
    Tile grid[GRID_SIZE];    // Grid array
};

static enum { DIRECT_NONE, DIRECT_LEFT, DIRECT_RIGHT, DIRECT_UP, DIRECT_DOWN } MoveDirect;
static enum { ANIMATION_NONE, ANIMATION_MOVE, ANIMATION_APPEAR } AnimationType;

//-------------------------------------------------------------------------------------------------
// Gameplay Variables Definition (local to this module)
//-------------------------------------------------------------------------------------------------
static Color tileColors[] = {
    COLOR_2, COLOR_4, COLOR_8, COLOR_16, COLOR_32, COLOR_64, COLOR_128,
    COLOR_256, COLOR_512, COLOR_1024, COLOR_2048, COLOR_4096, COLOR_8192
};

static struct Game game;  // A local game variable
static bool moved;        // Is true if tiles was moved

static float animationMoveDuration;
static float animationMoveSpeed;
static clock_t animationMoveBegin;
static unsigned long moveTime;  // Move animation timer

static float animationAppearDuration;
static float animationAppearSpeed;
static clock_t animationAppearBegin;
static unsigned long appearTime;  // Appear animation timer

static char *saveDirPath;
static char *saveFilePath;

static Rectangle board;
static Rectangle title;
static Rectangle score;
static Rectangle best;
static Rectangle purpose;

static int tileWidth;
static int tileFontSize;
static float tileBorderRadius;

static int spacing;

static int titleFontSize;
static int scoreTextFontSize;
static int scoreValueFontSize;
static int purposeFontSize;

static const char *textTitle = "2048";
static const char *textScore = "SCORE";
static const char *textBest  = "BEST";
static const char *textPurpose = "Join the numbers and get to the 2048 tile!";

//-------------------------------------------------------------------------------------------------
// Gameplay Local Draw Functions Declaration
//-------------------------------------------------------------------------------------------------
static void DrawScore(void);
static void DrawBest(void);
static void DrawTile(Tile *tile);
static void DrawBoard(void);

//-------------------------------------------------------------------------------------------------
// Gameplay Local Functions Declaration
//-------------------------------------------------------------------------------------------------
static void HandleInput(void);
static void AddTile(void);
static bool Move(int vx, int vy);
static bool GridIsFull(void);
static bool MoveIsAvailable(void);
static Color NumToColor(int value);
static Rectangle GetTileRec(const CellVector *v);
static int SaveGame(void);

//-------------------------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//-------------------------------------------------------------------------------------------------
void InitGameplayScreen(void)
{
    int width = GetScreenWidth();
    int height = GetScreenHeight();

    /* Define title */
    title = (Rectangle){ width*0.08, height*0.05, width*0.26, width*0.26 };
    titleFontSize = title.height * 0.35;

    /* Define score */
    score = (Rectangle){ width*0.42, height*0.05, width*0.21, height*0.1 };
    scoreTextFontSize = score.height * 0.35;
    scoreValueFontSize = score.height * 0.2;

    /* Define best */
    best = (Rectangle){ width*0.7, height*0.05, width*0.21, height*0.1 };

    /* Define purpose */
    purpose = (Rectangle){ width*0.08, title.height + height*0.1, width*0.84, height*0.04 };
    purposeFontSize = purpose.height * 0.7;

    /* Define board */
    board = (Rectangle){ width*0.08, height*0.34, width*0.84, width*0.84 };
    spacing = board.width * 0.03;

    tileWidth = (board.width - 5*spacing) / SIZE;
    tileFontSize = tileWidth * 0.5;
    tileBorderRadius = tileWidth * 0.05;

    /* Define animation */
    animationMoveDuration = tileWidth + spacing;                        // Move animation duration in pixels
    animationMoveSpeed = (animationMoveDuration / 0.1) * MILISEC;       // Move animation speed in pixels per miliseconds
    animationAppearDuration = spacing * 0.5;                            // Appear animation duration in pixels
    animationAppearSpeed = (animationAppearDuration / 0.05) * MILISEC;  // Appear animation speed in pixels per miliseconds

    /* Define save path */
    saveDirPath = StrConcat(getenv("HOME"), SAVE_DIR);
    saveFilePath = StrConcat(saveDirPath, SAVE_FILE);

    TraceLog(LOG_DEBUG, "Init game play screen");
}

void UpdateGameplayScreen(void)
{
    if (AnimationType == ANIMATION_NONE && MoveDirect == DIRECT_NONE) 
        HandleInput();

    else if (AnimationType == ANIMATION_MOVE) 
    {
        /* Update tiles move animation and add new tile to the grid.
         * Applies only if the grid tiles was moved.
         */
        moveTime = animationMoveSpeed * (clock() - animationMoveBegin);
        if (moveTime > animationMoveDuration) 
        {
            moveTime = 0;
            AnimationType = ANIMATION_APPEAR;
            animationAppearBegin = clock();

            AddTile();
        }
    }

    else if (AnimationType == ANIMATION_APPEAR) 
    {
        /* Update tiles appear animation and save game state.
         * Applies after move animation.
         */
        if (appearTime > animationAppearDuration) 
        {
            appearTime = 0;
            moved = false;
            AnimationType = ANIMATION_NONE;
            MoveDirect = DIRECT_NONE;

            game.moves++;

            if (GridIsFull() && !MoveIsAvailable())
            {
                /* Check that game can be continue and display the game
                 * over screen.
                 */
                nextScreen = GAME_OVER;
            }
            else if (!game.win && game.maxTile == 11)
            {
                /* Check that game is won and display a specified screen,
                 * this screen should be displayed once when the best tile
                 * score is 2048.
                 */
                nextScreen = GAME_WIN;
                game.win = true;
            }

            SaveGame();
        }
        else
            appearTime = animationAppearSpeed * (clock() - animationAppearBegin);
    }
}

void DrawGameplayScreen(void)
{
    float x, y;

    ClearBackground(COLOR_SCREEN_DEFAULT);

    /* Draw title */
    x = title.x + (title.width * 0.5) - MeasureText(textTitle, titleFontSize) * 0.5;
    y = title.y + (title.height * 0.5) - (titleFontSize * 0.5);

    DrawRoundedRectangleRec(title, tileBorderRadius, NumToColor(11));
    DrawText(textTitle, x, y, titleFontSize, COLOR_TILE_WHITE);

    /* Draw score */
    DrawScore();

    /* Draw best */
    DrawBest();

    /* Draw player purpose */
    x = purpose.x + (purpose.width * 0.5) - MeasureText(textPurpose, purposeFontSize) * 0.5;
    y = purpose.y + (purpose.height * 0.5) - (purposeFontSize * 0.5);

    DrawText(textPurpose, x, y, purposeFontSize, LIGHTGRAY);

    /* Draw game board */
    DrawBoard();
}

void UnloadGameplayScreen(void)
{
    free(saveDirPath);
    free(saveFilePath);
    TraceLog(LOG_DEBUG, "Unload game play screen");
}

/*
 * Public function which returns the game score
 */
unsigned int GetScore(void)
{
    return game.score;
}

/*
 * Public function which returns the game moves
 */
unsigned int GetMoves(void)
{
    return game.moves;
}

/* 
 * Begin a new game
 */
void NewGame(void)
{ 
    int row, col;
    Tile *tile;

    moved = false;
    AnimationType = ANIMATION_NONE;
    MoveDirect = DIRECT_NONE;

    game.score = 0;
    game.maxTile = 0;
    game.win = false;

    /* Initialize a grid */
    for (row = 0; row < SIZE; row++)
    {
        for (col = 0; col < SIZE; col++)
        {
            tile = &game.grid[row * SIZE + col];
            tile->value = 0;
            tile->oldValue = 0;
            tile->source = NULL;
            tile->position.x = row;
            tile->position.y = col;
            tile->oldPosition = tile->position;
        }
    }

    AddTile();
    AddTile();

    TraceLog(LOG_INFO, "Start new game");
}

static void HandleInput(void)
{
    if (IsKeyPressed(KEY_RIGHT) && Move(1, 0)) 
        MoveDirect = DIRECT_RIGHT;

    if (IsKeyPressed(KEY_LEFT) && Move(-1, 0)) 
        MoveDirect = DIRECT_LEFT;

    if (IsKeyPressed(KEY_UP) && Move(0, -1)) 
        MoveDirect = DIRECT_UP;

    if (IsKeyPressed(KEY_DOWN) && Move(0, 1)) 
        MoveDirect = DIRECT_DOWN;

    if (MoveDirect != DIRECT_NONE)
    {
        AnimationType = ANIMATION_MOVE;
        animationMoveBegin = clock();
    }

    if (IsKeyPressed(KEY_ESCAPE))
        nextScreen = MENU;
}

static void DrawScore(void)
{
    float x, y;
    char buffer[MAX_SCORE_BUFFER_SIZE];

    DrawRoundedRectangleRec(score, tileBorderRadius, COLOR_CELL_BG);

    /* Draw text in the top */
    sprintf(buffer, "%d", game.score);
    x = score.x + (score.width * 0.5) - MeasureText(buffer, scoreTextFontSize) * 0.5;
    y = score.y + score.height * 0.9 - scoreTextFontSize;

    DrawText(buffer, x, y, scoreTextFontSize, WHITE);

    /* Draw score value in the bottom */
    x = score.x + (score.width * 0.5) - MeasureText(textScore, scoreValueFontSize) * 0.5;
    y = score.y + score.height * 0.1;

    DrawText(textScore, x, y, scoreValueFontSize, WHITE);
}

static void DrawBest(void)
{
    float x, y;
    char buffer[MAX_SCORE_BUFFER_SIZE];

    DrawRoundedRectangleRec(best, tileBorderRadius, COLOR_CELL_BG);

    /* Draw text in the top */
    sprintf(buffer, "%d", game.best);
    x = best.x + (best.width * 0.5) - MeasureText(buffer, scoreTextFontSize) * 0.5;
    y = best.y + best.height * 0.9 - scoreTextFontSize;

    DrawText(buffer, x, y, scoreTextFontSize, WHITE);

    /* Draw best value in the bottom */
    x = best.x + (best.width * 0.5) - MeasureText(textBest, scoreValueFontSize) * 0.5;
    y = best.y + best.height * 0.1;

    DrawText(textBest, x, y, scoreValueFontSize, WHITE);
}

static void DrawBoard(void)
{
    /* Draw board background */
    DrawRectangleRec(board, COLOR_GRID_BG);

    /* Draw grid cells */
    for (int i = 0; i < GRID_SIZE; i++)
    {
        Rectangle rec = GetTileRec(&game.grid[i].oldPosition);
        DrawRoundedRectangleRec(rec, tileBorderRadius, COLOR_CELL_BG);
    }

    /* Draw grid tiles */
    for (int i = 0; i < GRID_SIZE; i++)
        DrawTile(&game.grid[i]);
}

/* 
 * Linear interpolation functions
 */
static inline float lerp(float v0, float v1)
{
   return (v0 - v1) * moveTime;
}

/* 
 * Draw tile if value more then 0
 */
static void DrawTile(Tile *tile)
{
    float x, y;
    char buffer[TILE_MAX_BUFFER_SIZE];
    Color color;
    Rectangle rec;

    /*
     * Set value as oldValue if move animation was finished.
     * 
     * Please note that tile will be draw with old value untile
     * move animation counter is less than MOVE_C.
     */
    if (tile->oldValue != tile->value && AnimationType == ANIMATION_APPEAR)
        tile->oldValue = tile->value;

    if (tile->oldValue > 0)
    {
        int font;

        font = (tile->oldValue < 10) ? tileFontSize: tileFontSize * 0.8;
        rec = GetTileRec(&tile->oldPosition);

        rec.x += lerp(tile->position.x, tile->oldPosition.x);
        rec.y += lerp(tile->position.y, tile->oldPosition.y);

        /* Draw appear animation if tile was merged */
        if (appearTime && tile->source)
        {
            rec.x -= appearTime;
            rec.y -= appearTime;
            rec.height += appearTime * 2;
            rec.width += appearTime * 2;
            font += appearTime;
        }

        /* Convert an integer value to a string */
        sprintf(buffer, "%d", 2 << (tile->oldValue - 1));

        /* Center a tile value text position */
        x = rec.x + (rec.height * 0.5) - MeasureText(buffer, font) * 0.5;
        y = rec.y + (rec.width * 0.5) - (font * 0.5);

        /* Define a tile value text color */
        color = (tile->oldValue <= 2) ? COLOR_TILE_GREY : COLOR_TILE_WHITE;

        /* Draw tile */
        DrawRoundedRectangleRec(rec, tileBorderRadius, NumToColor(tile->oldValue));
        DrawText(buffer, x, y, font, color);
    }
}

/* 
 * Returns a tile rectangle by cell position
 */
static Rectangle GetTileRec(const CellVector *v)
{
    Rectangle rec;

    /* Add grid and tails offset */
    rec.x = board.x + (tileWidth * (v->x % SIZE));
    rec.y = board.y + (tileWidth * (v->y % SIZE));

    /* Add spacing offset */
    rec.x += spacing * ((v->x % SIZE) + 1);
    rec.y += spacing * ((v->y % SIZE) + 1);

    rec.width = rec.height = tileWidth;

    return rec;
}

/* 
 * Move grid tiles by defined direction
 */
static bool Move(int vx, int vy)
{
    int i, j;
    int begin, end, inc;
    int row, col;

    // Move Right and Down
    if (vx == 1 || vy == 1)
    {
        begin = SIZE - 1;
        end = -1;
        inc = -1;
    }
    // Move Left and Up
    else
    {
        begin = 0;
        end = SIZE;
        inc = 1;
    }

    for (i = 0; i < SIZE; i++)
    {
        for (j = begin; j != end; j += inc)
        {
            /* Move by Y */
            if (vx == 0)
            {
                row = j;
                col = i;
            }
            /* Move by X */
            else if (vy == 0)
            {
                row = i;
                col = j;
            }

            Tile *tile = &game.grid[row * SIZE + col];
            tile->position = tile->oldPosition;
            tile->source = NULL;
            tile->oldValue = tile->value;
        }
    }

    for (i = 0; i < SIZE; i++)
    {
        for (j = begin; j != end; j += inc)
        {
            int nextRow, nextCol;
            Tile *tile, *next, *farthest;

            /* Move by Y */
            if (vx == 0)
            {
                row = i;
                col = j;
            }
            /* Move by X */
            else if (vy == 0)
            {
                row = j;
                col = i;
            }

            tile = &game.grid[row * SIZE + col];
            next = tile;

            if (!tile->value) continue;

            nextRow = row;
            nextCol = col;

            do
            {
                farthest = next;

                nextRow += vx;
                nextCol += vy;

                if (nextRow < 0 || nextCol < 0 || nextRow > 3 || nextCol > 3)
                    break;

                next = &game.grid[nextRow * SIZE + nextCol];
            } while (!next->value);

            if (next != tile && tile->value == next->value && !next->source) 
            {
                next->source = tile;
                tile->value = 0;
                tile->position = next->oldPosition;
                moved = true;

                game.score += (2 << next->value);
                next->value++;

                if (game.score > game.best)
                    game.best = game.score;

                if (next->value > game.maxTile)
                    game.maxTile = next->value;
            }
            else if (farthest != tile)
            {
                farthest->value = tile->value;
                tile->value = 0;
                tile->position = farthest->oldPosition;
                moved = true;
            }
        }
    }

    return moved;
}

/* 
 * Add new tile to the game grid
 */
static void AddTile(void)
{
    int i, j;
    Tile *empty_grid[GRID_SIZE];

    j = 0;
    for (i = 0; i < GRID_SIZE; i++)
    {
        empty_grid[j] = NULL;
        if (!game.grid[i].value) empty_grid[j++] = &game.grid[i];
    }

    if (j)
    {
        j = rand() % j;
        empty_grid[j]->source = empty_grid[j];
        empty_grid[j]->value = (rand() / RAND_MAX) < 0.9 ? 1 : 2;
        empty_grid[j]->oldValue = empty_grid[j]->value;
    }
}

/* 
 * Converts and returns tile value as a color
 */
static inline Color NumToColor(int value)
{
    return value <= TILE_MAX_COLOR_INDEX + 1 ? tileColors[--value]: BLACK;
}

/* 
 * Returns true if grid has any empty cell
 */
static bool GridIsFull(void)
{
    for (int i = 0; i < GRID_SIZE; i++)
    {
        if (game.grid[i].value == 0)
            return false;
    }
    return true;
}

/* 
 * Returns true if any grid tile can be moved 
 */
static bool MoveIsAvailable(void)
{
    for (int row = 0; row < SIZE; row++)
    {
        for (int col = 0; col < SIZE; col++)
        {
            int value = game.grid[row * SIZE + col].value;

            if ((row < SIZE - 1 && game.grid[(row + 1) * SIZE + col].value == value) ||
                (col < SIZE - 1 && game.grid[row * SIZE + (col + 1)].value == value) ||
                (row > 0 && game.grid[(row - 1) * SIZE + col].value == value) ||
                (col > 0 && game.grid[row * SIZE + (col - 1)].value == value))

                return true;
        }
    }
    return false;
}

/*
 * Save current game state to a file
 */
static int SaveGame(void)
{
    FILE *file;

    if ((file = fopen(saveFilePath, "wb")) == NULL)
    {
        TraceLog(LOG_WARNING, "Error open file");
        return -1;
    }

    if (fwrite(&game, sizeof(struct Game), 1, file) == 0)
    {
        TraceLog(LOG_WARNING, "Error writing file");
        return -1;
    }

    fclose(file);
    TraceLog(LOG_INFO, "Game was saved successfully");

    return 0;
}

/*
 * Load game state from a save file
 */
int LoadGame(void)
{
    FILE *file;

    if ((file = fopen(saveFilePath, "rb")) == NULL)
    {
        TraceLog(LOG_WARNING, "Error open file");
        return -1;
    }

    if (fread(&game, sizeof(struct Game), 1, file) == 0)
    {
        TraceLog(LOG_WARNING, "Error reading file");
        return -1;
    }

    fclose(file);
    TraceLog(LOG_INFO, "Game was loaded successfully");

    return 0;
}

/* 
 * Try to create the save directory if it's not exist
 */
int MakeSaveDir(void)
{
    errno = 0;

    if (mkdir(saveDirPath, S_IRWXU | S_IRGRP | S_IROTH) != 0)
    {
        switch (errno)
        {
            case EACCES:
                TraceLog(LOG_WARNING, "Permission denied");
                break;
            case ENOENT:
                TraceLog(LOG_WARNING, "No such directory");
                break;
            default:
                TraceLog(LOG_WARNING, "Save directory can't be created");
                break;
        }
        return -1;
    }

    TraceLog(LOG_DEBUG, "Save directory was created success");
    return 0;
}
