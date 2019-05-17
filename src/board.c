#include <stdlib.h>  // rand
#include <stdio.h>   // sprintf
#include "board.h"
#include "game.h"
#include "observer.h"
#include "resources.h"
#include "shapes.h"
#include "utils.h"

#define MAX_COLOR_INDEX          12
#define BUFFER_SIZE              10
#define ANIMATION_MOVE_FRAMES    5
#define ANIMATION_APPEAR_FRAMES  5

#define COLOR_WHITE   (Color){ 249, 246, 242, 255 }
#define COLOR_GREY    (Color){ 119, 110, 102, 255 }
#define COLOR_BOARD   (Color){ 186, 173, 161, 255 }
#define COLOR_CELL    (Color){ 204, 193, 181, 245 }

//-------------------------------------------------------------------------------------------------
// Local Variables Definition
//-------------------------------------------------------------------------------------------------
static Rectangle boardRec;
static float spacing;
static float tileSize;
static float moveSpeed;
static float appearSpeed;

static Color tileColors[] = {
    (Color){ 238, 228, 218, 255 },    // 2
    (Color){ 237, 224, 200, 255 },    // 4
    (Color){ 242, 177, 121, 255 },    // 8
    (Color){ 245, 149,  99, 255 },    // 16
    (Color){ 246, 124,  95, 255 },    // 32
    (Color){ 246,  94,  59, 255 },    // 64
    (Color){ 237, 207, 114, 255 },    // 128
    (Color){ 237, 204,  97, 255 },    // 256
    (Color){ 237, 200,  80, 255 },    // 512
    (Color){ 237, 197,  63, 255 },    // 1024
    (Color){ 237, 194,  46, 255 },    // 2048
    (Color){ 237, 112,  46, 255 },    // 4096
    (Color){ 237,  76,  46, 255 },    // 8192
};

//-------------------------------------------------------------------------------------------------
// Local Functions Declaration
//-------------------------------------------------------------------------------------------------
static void ProcessPhisics(Board *board);
static void AddTile(Board *board);
static bool GridIsFull(Board *board);
static Rectangle GetTileRec(const CellVector *v);
static void Move(Board *board, int vx, int vy);
static inline Color NumToColor(int value);
static inline float lerp(float v0, float v1, int elapsed);

//-------------------------------------------------------------------------------------------------
// Functions Definition
//-------------------------------------------------------------------------------------------------
void ResetBoard(Board *board)
{
    TraceLog(LOG_DEBUG, "Create New Board");

    // Define board properties
    board->moveFrames   = 0;
    board->appearFrames = 0;
    board->state        = BOARD_STATE_NONE;
    board->animation    = ANIMATION_APPEAR;

    // Initialize the grid
    for (int row = 0; row < SIZE; row++)
    {
        for (int col = 0; col < SIZE; col++)
        {
            Tile *tile        = &board->grid[row * SIZE + col];
            tile->value       = 0;
            tile->oldValue    = 0;
            tile->position.x  = row;
            tile->position.y  = col;
            tile->source      = NULL;
            tile->oldPosition = tile->position;
        }
    }

    AddTile(board);
    AddTile(board);
}

void InitBoard(Rectangle *rec)
{
    TraceLog(LOG_DEBUG, "Init Board");

    // Define board rectangle
    boardRec = *rec;

    // Define board tile and spacing sizes
    spacing  = boardRec.width * 0.03;
    tileSize = (boardRec.width - 5*spacing) / SIZE;

    // Define board animations
    moveSpeed   = (tileSize + spacing) / ANIMATION_MOVE_FRAMES;
    appearSpeed = (spacing * 0.5) / ANIMATION_APPEAR_FRAMES;
}

void HandleBoardInput(Board *board)
{
    if (board->state == BOARD_STATE_NONE)
    {
        if (IsKeyPressed(KEY_RIGHT))        Move(board,  1,  0);
        else if (IsKeyPressed(KEY_LEFT))    Move(board, -1,  0);
        else if (IsKeyPressed(KEY_UP))      Move(board,  0, -1);
        else if (IsKeyPressed(KEY_DOWN))    Move(board,  0,  1);
    }
}

void UpdateBoard(Board *board)
{
    if (board->animation == ANIMATION_NONE)
    {
        if (board->state == BOARD_STATE_MOVED)
        {
            board->animation = ANIMATION_MOVE;
            PlaySound(moveSound);
        }
        else if (board->state == BOARD_STATE_MERGED)
        {
            if (GetGame()->score > GetGame()->best)
            {
                GetGame()->best = GetGame()->score;
            }

            board->animation = ANIMATION_MOVE;
            PlaySound(mergeSound);
        }
    }
    else
    {
        ProcessPhisics(board);
    }
}

void DrawBoard(Board *board)
{
    char buffer[BUFFER_SIZE];

    // Draw board background
    DrawRoundedRectangleRec(boardRec, boardRec.width * 0.015, COLOR_BOARD);

    // Draw grid cells
    for (int i = 0; i < GRID_SIZE; i++)
    {
        Tile *tile    = &board->grid[i];
        Rectangle rec = GetTileRec(&tile->oldPosition);
        DrawRoundedRectangleRec(rec, tileSize * 0.05, COLOR_CELL);
    }

    // Draw grid tiles
    for (int i = 0; i < GRID_SIZE; i++)
    {
        Tile *tile = &board->grid[i];

        if (tile->oldValue > 0)
        {
            int font = (tile->oldValue < 10) ? tileSize * 0.5: tileSize * 0.4;

            Rectangle rec = GetTileRec(&tile->oldPosition);

            rec.x += lerp(tile->position.x, tile->oldPosition.x, board->moveFrames);
            rec.y += lerp(tile->position.y, tile->oldPosition.y, board->moveFrames);

            // Draw appear animation if tile was merged
            if (board->appearFrames && tile->source)
            {
                float elapsed = board->appearFrames * appearSpeed;

                rec.x      -= elapsed;
                rec.y      -= elapsed;
                rec.height += elapsed * 2;
                rec.width  += elapsed * 2;
                font       += elapsed;
            }

            // Convert an integer value to a string
            sprintf(buffer, "%d", 2 << (tile->oldValue - 1));

            // Center a tile value text position
            Vector2 vector = (Vector2) {
                rec.x + (rec.height * 0.5) - MeasureText(buffer, font) * 0.5,
                rec.y + (rec.width * 0.5) - (font * 0.5)
            };

            // Define a tile value text color
            Color color = (tile->oldValue <= 2) ? COLOR_GREY : COLOR_WHITE;

            // Draw tile
            DrawRoundedRectangleRec(rec, tileSize * 0.05, NumToColor(tile->oldValue));
            DrawText(buffer, vector.x, vector.y, font, color);
        }
    }
}

bool MoveIsAvailable(Board *board)
{
    if (!GridIsFull(board))
    {
        return true;
    }

    Tile *grid = board->grid;
    for (int row = 0; row < SIZE; row++)
    {
        for (int col = 0; col < SIZE; col++)
        {
            int value = board->grid[row * SIZE + col].value;

            if (row < SIZE - 1 && grid[(row + 1) * SIZE + col].value == value) return true;
            if (col < SIZE - 1 && grid[row * SIZE + (col + 1)].value == value) return true;
            if (row > 0 && grid[(row - 1) * SIZE + col].value == value) return true;
            if (col > 0 && grid[row * SIZE + (col - 1)].value == value) return true;
        }
    }

    return false;
}

//-------------------------------------------------------------------------------------------------
// Local Functions Definition
//-------------------------------------------------------------------------------------------------
static inline Color NumToColor(int value)
{
    return value <= MAX_COLOR_INDEX + 1 ? tileColors[--value]: BLACK;
}

static inline float lerp(float v0, float v1, int elapsed)
{
   return (v0 - v1) * elapsed * moveSpeed;
}

static void ProcessPhisics(Board *board)
{
    switch (board->animation)
    {
    case ANIMATION_MOVE:

        /*
         * Update tiles move animation and add new tile to the grid.
         * Applies only if the grid tiles was moved.
         */

        if (++board->moveFrames > ANIMATION_MOVE_FRAMES)
        {
            board->moveFrames = 0;
            board->animation  = ANIMATION_APPEAR;

            for (int i = 0; i < GRID_SIZE; i++)
            {
                Tile *tile = &board->grid[i];

                /*
                 * Set value as oldValue if move animation was finished.
                 *
                 * Please note that tile will be draw with old value untile
                 * move animation counter is less than moveFrames.
                 */

                if (tile->oldValue != tile->value && board->animation == ANIMATION_APPEAR)
                    tile->oldValue = tile->value;
            }

            AddTile(board);
        }
        break;

    case ANIMATION_APPEAR:

        /*
         * Update tiles appear animation and save game state.
         * Applies after move animation.
         */

        if (++board->appearFrames > ANIMATION_APPEAR_FRAMES)
        {
            Notify(ADD_TILE_EVENT);

            board->appearFrames = 0;
            board->animation    = ANIMATION_NONE;
            board->state        = BOARD_STATE_NONE;

            if (!MoveIsAvailable(board))
            {
                /*
                 * Check that game can be continue and display the game
                 * over screen.
                 */

                Notify(GAME_OVER_EVENT);
            }
        }
        break;

    default:
        break;
    }
}

static Rectangle GetTileRec(const CellVector *v)
{
    Rectangle rec;

    // Add grid and tails offset
    rec.x = boardRec.x + (tileSize * (v->x % SIZE));
    rec.y = boardRec.y + (tileSize * (v->y % SIZE));

    // Add spacing offset
    rec.x += spacing * ((v->x % SIZE) + 1);
    rec.y += spacing * ((v->y % SIZE) + 1);

    rec.width = rec.height = tileSize;

    return rec;
}

static void AddTile(Board *board)
{
    int i, j;
    Tile *empty_grid[GRID_SIZE];

    j = 0;
    for (i = 0; i < GRID_SIZE; i++)
    {
        empty_grid[j] = NULL;
        if (!board->grid[i].value) empty_grid[j++] = &board->grid[i];
    }

    if (j)
    {
        j = rand() % j;
        empty_grid[j]->source = empty_grid[j];
        empty_grid[j]->value = (rand() / RAND_MAX) < 0.9f ? 1 : 2;
        empty_grid[j]->oldValue = empty_grid[j]->value;
    }
}

static void Move(Board *board, int vx, int vy)
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
            // Move by Y
            if (vx == 0)
            {
                row = j;
                col = i;
            }
            // Move by X
            else if (vy == 0)
            {
                row = i;
                col = j;
            }

            Tile *tile = &board->grid[row * SIZE + col];
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

            // Move by Y
            if (vx == 0)
            {
                row = i;
                col = j;
            }
            // Move by X
            else if (vy == 0)
            {
                row = j;
                col = i;
            }

            tile = &board->grid[row * SIZE + col];
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

                next = &board->grid[nextRow * SIZE + nextCol];

            } while (!next->value);

            if (next != tile && tile->value == next->value && !next->source)
            {
                next->source     = tile;
                tile->value      = 0;
                tile->position   = next->oldPosition;
                GetGame()->score = GetGame()->score + (2 << next->value);
                next->value++;

                if (next->value > GetGame()->max)
                    GetGame()->max = next->value;

                if (board->state < BOARD_STATE_MERGED)
                    board->state = BOARD_STATE_MERGED;
            }
            else if (farthest != tile)
            {
                farthest->value = tile->value;
                tile->value     = 0;
                tile->position  = farthest->oldPosition;

                if (board->state < BOARD_STATE_MOVED)
                    board->state = BOARD_STATE_MOVED;
            }
        }
    }
}

static bool GridIsFull(Board *board)
{
    for (int i = 0; i < GRID_SIZE; i++)
    {
        if (board->grid[i].value == 0)
            return false;
    }
    return true;
}
