#include <stdlib.h>
#include <stdio.h>

#include "raylib.h"

#include "screens.h"
#include "../shapes.c"

#define SCREEN_BACKGROUND CLITERAL{ 250, 248, 239, 255 }
#define GRID_BACKGROUND   CLITERAL{ 186, 173, 161, 255 }
#define CELL_BACKGROUND   CLITERAL{ 204, 193, 181, 245 }
#define TILE_FONT_WHITE   CLITERAL{ 249, 246, 242, 255 }
#define TILE_FONT_GREY    CLITERAL{ 119, 110, 102, 255 }

#define SIZE 4
#define GRID_SIZE (SIZE * SIZE)

#define MAX_SCORE_LENGTH      12
#define MAX_TILE_VALUE_LENGTH 12

#define PADDING    (WIDTH * 0.065)
#define GRID_WIDTH (WIDTH - PADDING * 2)
#define SPACING    (GRID_WIDTH * 0.2 * 0.15)
#define TILE_SIZE  ((GRID_WIDTH - 5 * SPACING) * 0.25)
#define TILE_FONT  (TILE_SIZE * 0.5)
#define TILE_ROUND (TILE_SIZE * 0.05)

#define MOVE_C   (TILE_SIZE + SPACING)
#define MOVE_S   (MOVE_C * 0.15)
#define APPEAR_C (SPACING * 0.6)
#define APPEAR_S (APPEAR_C * 0.15)

//-----------------------------------------------------------------------------
// Gameplay Types and Structures Definition
//-----------------------------------------------------------------------------
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
    int x;
    int y;
} CellVector;

typedef struct tile {
    int value;
    int oldValue;

    CellVector position;
    CellVector oldPosition;

    struct tile *source;
} Tile;

typedef Tile Grid[GRID_SIZE];

typedef struct {
    int score;
    int best;
    int moves;

    Grid      grid;
    Direction direction;
} Game;

//-----------------------------------------------------------------------------
// Gameplay Variables Definition (local to this module)
//-----------------------------------------------------------------------------
static Color tileColors[] = {
    { 238, 228, 218, 255 },  // 2
    { 237, 224, 200, 255 },  // 4
    { 242, 177, 121, 255 },  // 8
    { 245, 149, 99, 255 },   // 16
    { 246, 124, 95, 255 },   // 32
    { 246, 94, 59, 255 },    // 64
    { 237, 207, 114, 255 },  // 128
    { 237, 204, 97, 255 },   // 256
    { 237, 200, 80, 255 },   // 512
    { 237, 197, 63, 255 },   // 1024
    { 237, 112, 46, 255 },   // 2048
    { 237, 112, 46, 255 },   // 4096
    { 237, 76, 46, 255 },    // 8192
};

static Game game;
static bool moved;
static int moveTime;
static int appearTime;

static Rectangle gridRec;
static Rectangle titleRec;
static Rectangle scoreRec;
static Rectangle bestRec;
static Rectangle purposeRec;

//-----------------------------------------------------------------------------
// Gameplay Local Functions Declaration
//-----------------------------------------------------------------------------
static void drawInfo(void);
static void drawGrid(void);
static void drawTile(Tile *);
static void addTile(void);
static bool move(int, int);
static bool gridIsFull(void);
static bool available(void);
static Color numToColor(int);
static Rectangle tileRec(CellVector *);

//-----------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//-----------------------------------------------------------------------------
void InitGameplayScreen(void)
{
    /* define title position */
    titleRec.x = PADDING;
    titleRec.y = PADDING;
    titleRec.width = (WIDTH - PADDING * 4) * 0.4;
    titleRec.height = titleRec.width;

    /* define score position */
    scoreRec.x = titleRec.x + titleRec.width + PADDING;
    scoreRec.y = PADDING;
    scoreRec.width = (WIDTH - PADDING * 4) * 0.3;
    scoreRec.height = WIDTH * 0.15;

    /* define best score position */
    bestRec.x = scoreRec.x + scoreRec.width + PADDING;
    bestRec.y = PADDING;
    bestRec.width = (WIDTH - PADDING * 4) * 0.3;
    bestRec.height = WIDTH * 0.15;

    /* define game purpose position */
    purposeRec.x = PADDING;
    purposeRec.y = titleRec.y + titleRec.height + PADDING / 2;
    purposeRec.width = WIDTH - PADDING * 2;
    purposeRec.height = purposeRec.width * 0.1;

    /* define grid position */
    gridRec.width = (TILE_SIZE * SIZE) + (SPACING * (SIZE + 1));
    gridRec.height = gridRec.width;
    gridRec.x = PADDING;
    gridRec.y = purposeRec.y + purposeRec.height + PADDING / 2;

    NewGame();
}

static void handleInput(void)
{
    if (IsKeyPressed(KEY_RIGHT)) 
    {
        game.direction = DIRECT_RIGHT;
        move(1, 0);
    }

    if (IsKeyPressed(KEY_LEFT)) 
    {
        game.direction = DIRECT_LEFT;
        move(-1, 0);
    }

    if (IsKeyPressed(KEY_UP)) 
    {
        game.direction = DIRECT_UP;
        move(0, -1);
    }

    if (IsKeyPressed(KEY_DOWN)) 
    {
        game.direction = DIRECT_DOWN;
        move(0, 1);
    }
}

void UpdateGameplayScreen(void)
{
    if (animation == ANIMATION_NONE && game.direction == DIRECT_NONE) {
        handleInput();
        if (moved)
            animation = ANIMATION_MOVE;
        else
            game.direction = DIRECT_NONE;
    }
    else if (animation == ANIMATION_APPEAR) {
        if (appearTime > APPEAR_C) {
            appearTime = 0;
            moved      = false;
            animation  = ANIMATION_NONE;
        
            game.direction = DIRECT_NONE;
            game.moves++;

            /* Check that game can be continue */
            if (gridIsFull() && !available()) {
                GameScreen = SCREEN_GAME_OVER;
            }
        }
        else
            appearTime += APPEAR_S; 
    }
    else if (animation == ANIMATION_MOVE) {
        moveTime += MOVE_S;
        if (moveTime > MOVE_C) {
            moveTime = 0;
            animation = ANIMATION_APPEAR;
            addTile();
        }
    }
}

void DrawGameplayScreen(void)
{
    ClearBackground(SCREEN_BACKGROUND);
    drawGrid();
    drawInfo();
}

void UnloadGameplayScreen(void);

void NewGame(void)
{ 
    int row, col;
    Tile *tile;

    moved = false;
    animation = ANIMATION_NONE;

    game.score     = 0;
    game.direction = DIRECT_NONE;

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
    addTile();
    addTile();
}

static void drawInfo(void)
{
    char score[MAX_SCORE_LENGTH];
    char *purpose = "Join the numbers and get to the 2048 tile!";
    float x, y;
    float font;

    /* Draw title block */
    font = (titleRec.height / 2) * 0.7;
    x = titleRec.x + (titleRec.width / 2) - MeasureText("2048", font) / 2;
    y = titleRec.y + (titleRec.height / 2) - (font / 2);
    DrawRoundedRectangleRec(titleRec, TILE_ROUND, tileColors[9]);
    DrawText("2048", x, y, font, TILE_FONT_WHITE);

    /* Draw score block */
    DrawRoundedRectangleRec(scoreRec, TILE_ROUND, CELL_BACKGROUND);
    font = (scoreRec.height / 2) * 0.7;

    sprintf(score, "%d", game.score);
    x = scoreRec.x + (scoreRec.width / 2) - MeasureText(score, font) / 2;
    y = scoreRec.y + scoreRec.height * 0.9 - font;
    DrawText(score, x, y, font, WHITE);

    x = scoreRec.x + (scoreRec.width / 2) - MeasureText("SCORE", font) / 2;
    y = scoreRec.y + scoreRec.height * 0.1;
    DrawText("SCORE", x, y, font, WHITE);

    /* Draw best score block */
    DrawRoundedRectangleRec(bestRec, TILE_ROUND, CELL_BACKGROUND);
    font = (bestRec.height / 2) * 0.7;

    sprintf(score, "%d", game.best);
    x = bestRec.x + (bestRec.width / 2) - MeasureText(score, font) / 2;
    y = bestRec.y + bestRec.height * 0.9 - font;
    DrawText(score, x, y, font, WHITE);

    x = bestRec.x + (bestRec.width / 2) - MeasureText("BEST", font) / 2;
    y = scoreRec.y + scoreRec.height * 0.1;
    DrawText("BEST", x, y, font, WHITE);

    /* Draw purpose block */
    font = (purposeRec.height / 2) * 0.9;
    y = purposeRec.y + (purposeRec.height / 2) - (font / 2);
    DrawText(purpose, PADDING, y, font, LIGHTGRAY);
}

static void drawGrid(void)
{
    /* Draw grid board */
    DrawRectangleRec(gridRec, GRID_BACKGROUND);

    /* Draw grid cells */
    for (int i = 0; i < GRID_SIZE; i++)
    {
        Rectangle rec = tileRec(&game.grid[i].oldPosition);
        DrawRoundedRectangleRec(rec, TILE_ROUND, CELL_BACKGROUND);
    }

    /* Draw grid tiles */
    for (int i = 0; i < GRID_SIZE; i++)
    {
        drawTile(&game.grid[i]);
    }
}

/* Interpolation functions */
static float lerp(float v0, float v1)
{
   return (v0 - v1) * moveTime;
}

static void drawTile(Tile *tile)
{
    float x, y;
    char text[MAX_TILE_VALUE_LENGTH];
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

        font = (tile->oldValue < 10) ? TILE_FONT: TILE_FONT * 0.8;
        rec = tileRec(&tile->oldPosition);

        rec.x += lerp(tile->position.x, tile->oldPosition.x);
        rec.y += lerp(tile->position.y, tile->oldPosition.y);

        /* Draw appear animation if tile was merged */
        if (appearTime && tile->source)
        {
            rec.x -= appearTime;
            rec.y -= appearTime;
            rec.height += appearTime * 2;
            rec.width  += appearTime * 2;
            font += appearTime;
        }

        /* Convert an integer value to a string */
        sprintf(text, "%d", 2 << (tile->oldValue - 1));

        /* Center a tile value text position */
        x = rec.x + (rec.height / 2) - MeasureText(text, font) / 2;
        y = rec.y + (rec.width / 2) - (font / 2);

        /* Define a tile value text color */
        color = (tile->oldValue <= 2) ? TILE_FONT_GREY : TILE_FONT_WHITE;

        /* Draw tile */
        DrawRoundedRectangleRec(rec, TILE_ROUND, numToColor(tile->oldValue));
        DrawText(text, x, y, font, color);
    }
}

static Rectangle tileRec(CellVector *v)
{
    Rectangle rec;

    /* Add grid and tails offset */
    rec.x = gridRec.x + (TILE_SIZE * (v->x % SIZE));
    rec.y = gridRec.y + (TILE_SIZE * (v->y % SIZE));

    /* Add spacing offset */
    rec.x += SPACING * ((v->x % SIZE) + 1);
    rec.y += SPACING * ((v->y % SIZE) + 1);

    rec.width  = TILE_SIZE;
    rec.height = TILE_SIZE;

    return rec;
}

static bool move(int vx, int vy)
{
    int i, j;
    int begin, end, inc;
    int row, col;

    // Move Right and Down
    if (vx == 1 || vy == 1)
    {
        begin = SIZE - 1;
        end   = -1;
        inc   = -1;
    }
    // Move Left and Up
    else
    {
        begin = 0;
        end   = SIZE;
        inc   = 1;
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
                game.score += 2 << next->value;
                next->value++;
                next->source = tile;
                tile->value = 0;
                tile->position = next->oldPosition;
                moved = true;
                if (game.score > game.best) {
                    game.best = game.score;
                }
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

static void addTile(void)
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

static Color numToColor(int value)
{
    if (value > sizeof(tileColors) / sizeof(Color))
        return BLACK;
    return tileColors[--value];
}

/* Returns true if grid has any empty cell */
static bool gridIsFull(void)
{
    for (int i = 0; i < GRID_SIZE; i++)
    {
        if (game.grid[i].value == 0)
            return false;
    }
    return true;
}

/* Returns true if any grid tile can be moved */
static bool available(void)
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
