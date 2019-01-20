#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "raylib.h"
#include "screens.h"

#define GRID_WIDTH  (WIDTH - PADDING * 2)
#define SPACING     (GRID_WIDTH * 0.03)  // Spacing size between tiles

#define TILE_MAX_BUFFER_SIZE  12
#define TILE_WIDTH            ((GRID_WIDTH - 5 * SPACING) / SIZE)
#define TILE_FONT_SIZE        (TILE_WIDTH * 0.5)
#define TILE_BORDER_RADIUS    (TILE_WIDTH * 0.05)

#define MILISEC  0.000001

#define ANIMATION_MOVING_SIZE  (TILE_WIDTH + SPACING)                      // Move animation duration in pixels
#define ANIMATION_MOVING_STEP  ((ANIMATION_MOVING_SIZE / 0.15) * MILISEC)  // Move animation speed in pixels per miliseconds
#define ANIMATION_APPEAR_SIZE  (SPACING * 0.5)                             // Appear animation duration in pixels
#define ANIMATION_APPEAR_STEP  ((ANIMATION_APPEAR_SIZE / 0.05) * MILISEC)  // Appear animation speed in pixels per miliseconds

#define SAVE_DIR   "/Library/Application Support/2048"
#define SAVE_FILE  "/save.dat"

//-------------------------------------------------------------------------------------------------
// Gameplay Types and Structures Definition
//-------------------------------------------------------------------------------------------------
typedef enum {
    DIRECT_NONE,
    DIRECT_LEFT,
    DIRECT_RIGHT,
    DIRECT_UP,
    DIRECT_DOWN,
} Direction;

static enum {
    ANIMATION_NONE,
    ANIMATION_MOVE,
    ANIMATION_APPEAR,
} animation;

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

typedef Tile Grid[GRID_SIZE];  // Game grid type

struct Game {
    unsigned int score;      // Current score value
    unsigned int best;       // Best score value
    unsigned int bestValue;  // Highest tile value
    unsigned int moves;      // Current moves counter
    
    bool win;                // Set true if tile 2048 was achieved

    Grid grid;
    Direction direction;     // Current game move direction
};

//-------------------------------------------------------------------------------------------------
// Gameplay Variables Definition (local to this module)
//-------------------------------------------------------------------------------------------------
static Color tileColors[] = {
    COLOR_2, COLOR_4, COLOR_8, COLOR_16, COLOR_32, COLOR_64, COLOR_128,
    COLOR_256, COLOR_512, COLOR_1024, COLOR_2048, COLOR_4096, COLOR_8192
};

static struct Game game;            // A local game variable
static bool moved;                  // Set true if tiles was moved
static unsigned long moveTime;      // Move animation timer
static unsigned long appearTime;    // Appear animation timer

clock_t moveStartTime;
clock_t appearStartTime;

//-------------------------------------------------------------------------------------------------
// Gameplay Frame Variables Definition
//-------------------------------------------------------------------------------------------------
static Rectangle board;
static Rectangle title;
static Rectangle score;
static Rectangle best;
static Rectangle purpose;

static int titleFontSize;
static int scoreTextFontSize;
static int scoreValueFontSize;
static int bestTextFontSize;
static int bestValueFontSize;
static int purposeFontSize;

static const char *textTitle = "2048";
static const char *textScore = "SCORE";
static const char *textBest  = "BEST";
static const char *textPurpose = "Join the numbers and get to the 2048 tile!";

static char *saveFilePath;

//-------------------------------------------------------------------------------------------------
// Gameplay Local Draw Functions Declaration
//-------------------------------------------------------------------------------------------------
static void DrawTitle(void);
static void DrawScore(void);
static void DrawBest(void);
static void DrawPurpose(void);
static void DrawTile(Tile *tile);
static void DrawBoard(void);

//-------------------------------------------------------------------------------------------------
// Gameplay Local Functions Declaration
//-------------------------------------------------------------------------------------------------
static void AddTile(void);
static bool Move(int vx, int vy);
static bool GridIsFull(void);
static bool Available(void);
static Color NumToColor(int value);
static Rectangle GetTileRec(const CellVector *v);
static int SaveGame(void);
static char* StrConcat(const char *s1, const char *s2);

//-------------------------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//-------------------------------------------------------------------------------------------------
void InitGameplayScreen(void)
{
    char * saveDirPath;

    /* Define title position */
    title.x = PADDING;
    title.y = PADDING;
    title.width = (WIDTH - PADDING * 4) * 0.4;
    title.height = title.width;
    titleFontSize = title.height * 0.35;

    /* Define score position */
    score.x = title.x + title.width + PADDING;
    score.y = PADDING;
    score.width = (WIDTH - PADDING * 4) * 0.3;
    score.height = WIDTH * 0.15;
    scoreTextFontSize = score.height * 0.35;
    scoreValueFontSize = score.height * 0.2;

    /* Define best position */
    best.x = score.x + score.width + PADDING;
    best.y = PADDING;
    best.width = (WIDTH - PADDING * 4) * 0.3;
    best.height = WIDTH * 0.15;
    bestTextFontSize = scoreTextFontSize;
    bestValueFontSize = scoreValueFontSize;

    /* Define game purpose position */
    purpose.x = PADDING;
    purpose.y = title.y + title.height + PADDING * 0.5;
    purpose.width = WIDTH - PADDING * 2;
    purpose.height = purpose.width * 0.1;
    purposeFontSize = purpose.height * 0.45;

    /* Define board position */
    board.width = (TILE_WIDTH * SIZE) + (SPACING * (SIZE + 1));
    board.height = board.width;
    board.x = PADDING;
    board.y = purpose.y + purpose.height + PADDING * 0.5;

    /* Initialize saving and load game*/
    /* Initialize saving and load game*/
    saveDirPath = StrConcat(getenv("HOME"), SAVE_DIR);
    saveFilePath = StrConcat(saveDirPath, SAVE_FILE);

    mkdir(StrConcat(getenv("HOME"), SAVE_DIR), S_IRWXU | S_IRGRP | S_IROTH);
    free(saveDirPath);

    TraceLog(LOG_DEBUG, "[GAME] Init game play screen");
}

/* Handle gameplay screen input */
static void HandleInput(void)
{
    if (IsKeyPressed(KEY_RIGHT) && Move(1, 0)) 
    {
        game.direction = DIRECT_RIGHT;
        animation = ANIMATION_MOVE;
        moveStartTime = clock();
    }

    if (IsKeyPressed(KEY_LEFT) && Move(-1, 0)) 
    {
        game.direction = DIRECT_LEFT;
        animation = ANIMATION_MOVE;
        moveStartTime = clock();
    }

    if (IsKeyPressed(KEY_UP) && Move(0, -1)) 
    {
        game.direction = DIRECT_UP;
        animation = ANIMATION_MOVE;
        moveStartTime = clock();
    }

    if (IsKeyPressed(KEY_DOWN) && Move(0, 1)) 
    {
        game.direction = DIRECT_DOWN;
        animation = ANIMATION_MOVE;
        moveStartTime = clock();
    }

    if (IsKeyPressed(KEY_ESCAPE)) 
        TransitionToScreen(SCREEN_GAME_MENU);
}

void UpdateGameplayScreen(void)
{
    if (animation == ANIMATION_NONE && game.direction == DIRECT_NONE) 
        HandleInput();

    else if (animation == ANIMATION_MOVE) 
    {
        /* Update tiles move animation and add new tile to the grid.
         * Applies only if the grid tiles was moved.
         */
        moveTime = ANIMATION_MOVING_STEP * (clock() - moveStartTime);
        if (moveTime > ANIMATION_MOVING_SIZE) 
        {
            moveTime = 0;
            animation = ANIMATION_APPEAR;
            appearStartTime = clock();

            AddTile();
        }
    }

    else if (animation == ANIMATION_APPEAR) 
    {
        /* Update tiles appear animation and save game state.
         * Applies after move animation.
         */
        if (appearTime > ANIMATION_APPEAR_SIZE) 
        {
            appearTime = 0;
            moved = false;
            animation = ANIMATION_NONE;
            game.direction = DIRECT_NONE;

            game.moves++;

            if (GridIsFull() && !Available())
            {
                /* Check that game can be continue and display the game
                 * over screen.
                 */
                TransitionToScreen(SCREEN_GAME_OVER);
            }
            else if (!game.win && game.bestValue == 11)
            {
                /* Check that game is won and display a specified screen,
                 * this screen should be displayed once when the best tile
                 * score is 2048.
                 */
                TransitionToScreen(SCREEN_GAME_WIN);
                game.win = true;
            }

            SaveGame();
        }
        else
            appearTime = ANIMATION_APPEAR_STEP * (clock() - appearStartTime);
    }
}

void DrawGameplayScreen(void)
{
    ClearBackground(SCREEN_BACKGROUND_COLOR);
    DrawTitle();
    DrawScore();
    DrawBest();
    DrawPurpose();
    DrawBoard();
}

/* 
 * Gameplay Screen Unload logic
 */
void UnloadGameplayScreen(void)
{
    free(saveFilePath);
    TraceLog(LOG_INFO, "[GAME] Unload game play screen");
}

void NewGame(void)
{ 
    int row, col;
    Tile *tile;

    moved = false;
    animation = ANIMATION_NONE;

    game.score = 0;
    game.bestValue = 0;
    game.direction = DIRECT_NONE;
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

    TraceLog(LOG_INFO, "[GAME] Start new game");
}

static void DrawTitle(void)
{
    float x, y;

    x = title.x + (title.width * 0.5) - MeasureText(textTitle, titleFontSize) * 0.5;
    y = title.y + (title.height * 0.5) - (titleFontSize * 0.5);

    DrawRoundedRectangleRec(title, TILE_BORDER_RADIUS, NumToColor(11));
    DrawText(textTitle, x, y, titleFontSize, TILE_FONT_WHITE_COLOR);
}

static void DrawScore(void)
{
    float x, y;
    char buffer[MAX_SCORE_BUFFER_SIZE];

    DrawRoundedRectangleRec(score, TILE_BORDER_RADIUS, CELL_BACKGROUND_COLOR);

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

    DrawRoundedRectangleRec(best, TILE_BORDER_RADIUS, CELL_BACKGROUND_COLOR);

    /* Draw text in the top */
    sprintf(buffer, "%d", game.best);
    x = best.x + (best.width * 0.5) - MeasureText(buffer, bestTextFontSize) * 0.5;
    y = best.y + best.height * 0.9 - bestTextFontSize;

    DrawText(buffer, x, y, bestTextFontSize, WHITE);

    /* Draw best value in the bottom */
    x = best.x + (best.width * 0.5) - MeasureText(textBest, bestValueFontSize) * 0.5;
    y = best.y + best.height * 0.1;

    DrawText(textBest, x, y, bestValueFontSize, WHITE);
}

static void DrawPurpose(void)
{
    float x, y;

    x = PADDING;
    y = purpose.y + (purpose.height * 0.5) - (purposeFontSize * 0.5);

    DrawText(textPurpose, x, y, purposeFontSize, LIGHTGRAY);
}

static void DrawBoard(void)
{
    /* Draw board background */
    DrawRectangleRec(board, GRID_BACKGROUND_COLOR);

    /* Draw grid cells */
    for (int i = 0; i < GRID_SIZE; i++)
    {
        Rectangle rec = GetTileRec(&game.grid[i].oldPosition);
        DrawRoundedRectangleRec(rec, TILE_BORDER_RADIUS, CELL_BACKGROUND_COLOR);
    }

    /* Draw grid tiles */
    for (int i = 0; i < GRID_SIZE; i++)
    {
        DrawTile(&game.grid[i]);
    }
}

/* 
 * Linear interpolation functions
 */
static inline float lerp(float v0, float v1)
{
   return (v0 - v1) * moveTime;
}

/* 
 * Draw tile on the grid if value more then 0
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
    if (tile->oldValue != tile->value && animation == ANIMATION_APPEAR)
        tile->oldValue = tile->value;

    if (tile->oldValue > 0)
    {
        int font;

        font = (tile->oldValue < 10) ? TILE_FONT_SIZE: TILE_FONT_SIZE * 0.8;
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
        color = (tile->oldValue <= 2) ? TILE_FONT_GREY_COLOR : TILE_FONT_WHITE_COLOR;

        /* Draw tile */
        DrawRoundedRectangleRec(rec, TILE_BORDER_RADIUS, NumToColor(tile->oldValue));
        DrawText(buffer, x, y, font, color);
    }
}

static Rectangle GetTileRec(const CellVector *v)
{
    Rectangle rec;

    /* Add grid and tails offset */
    rec.x = board.x + (TILE_WIDTH * (v->x % SIZE));
    rec.y = board.y + (TILE_WIDTH * (v->y % SIZE));

    /* Add spacing offset */
    rec.x += SPACING * ((v->x % SIZE) + 1);
    rec.y += SPACING * ((v->y % SIZE) + 1);

    rec.width = TILE_WIDTH;
    rec.height = rec.width;

    return rec;
}

/* 
 * Move grid tiles in the recived vector
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

                if (next->value > game.bestValue)
                    game.bestValue = next->value;
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
static Color NumToColor(int value)
{
    if (value > sizeof(tileColors) / sizeof(Color))
        return BLACK;
    return tileColors[--value];
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
static bool Available(void)
{
    int row, col;
    int value;

    for (row = 0; row < SIZE; row++)
    {
        for (col = 0; col < SIZE; col++)
        {
            value = game.grid[row * SIZE + col].value;
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
        TraceLog(LOG_ERROR, "error open file");
        return 1;
    }

    if (fwrite(&game, sizeof(struct Game), 1, file) == 0)
    {
        TraceLog(LOG_ERROR, "error writing file");
        return 1;
    }

    fclose(file);
    TraceLog(LOG_INFO, "[GAME] game was saved successfully");

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
        TraceLog(LOG_WARNING, "error open file");
        return 1;
    }

    if (fread(&game, sizeof(struct Game), 1, file) == 0)
    {
        TraceLog(LOG_WARNING, "error reading file");
        return 1;
    }

    fclose(file);
    TraceLog(LOG_INFO, "[GAME] game was loaded successfully");

    return 0;
}

static char* StrConcat(const char *s1, const char *s2)
{
    char *buffer;

    buffer = malloc(strlen(s1) + strlen(s2) + 1);
    if (!buffer)
        TraceLog(LOG_ERROR, "can't allocate memory");

    strcpy(buffer, s1);
    strcat(buffer, s2);
    return buffer;
}

/*
 * Public function, returns score
 */
unsigned int GetScore(void)
{
    return game.score;
}

/*
 * Public function, returns moves counter
 */
unsigned int GetMoves(void)
{
    return game.moves;
}
