#include <stdio.h>

#include "raylib.h"
#include "screens.h"

//-------------------------------------------------------------------------------------------------
// Game Over Screen Variables Definition (local to this module)
//-------------------------------------------------------------------------------------------------
static Rectangle title;
static Rectangle score;
static Rectangle moves;
static Rectangle help;

static float scoreBorderRadius;
static float movesBorderRadius;

static int titleFontSize;
static int scoreTextFontSize;
static int scoreValueFontSize;
static int movesTextFontSize;
static int movesValueFontSize;
static int helpFontSize;

static const char *textTitle = "Game Over!";
static const char *textScore = "SCORE";
static const char *textMoves = "MOVES";
static const char *textHelp  = "Press Enter to Try again";

//-------------------------------------------------------------------------------------------------
// Game Over Screen Local Functions Declaration
//-------------------------------------------------------------------------------------------------
static void DrawTitle(void);
static void DrawScore(void);
static void DrawMoves(void);
static void DrawHelp(void);

//-------------------------------------------------------------------------------------------------
// Game Over Screen Functions Definition
//-------------------------------------------------------------------------------------------------
void InitGameOverScreen(void)
{
    /* Define title frame position and font size */
    title.x = PADDING;
    title.y = PADDING;
    title.width = (WIDTH - PADDING * 2);
    title.height = title.width * 0.25;
    titleFontSize = title.height * 0.7;

    /* Define score frame position and font size */
    score.width = WIDTH * 0.4;
    score.height = score.width * 0.5;
    score.x = (WIDTH - score.width) * 0.5;
    score.y = title.y + title.height + PADDING;
    scoreTextFontSize = score.height * 0.25;
    scoreValueFontSize = score.height * 0.4;
    scoreBorderRadius = score.width * 0.05;

    /* Define moves frame position and font size */
    moves.x = score.x;
    moves.y = score.y + score.height + PADDING;
    moves.width = score.width;
    moves.height = score.height;
    movesTextFontSize = scoreTextFontSize;
    movesValueFontSize = scoreValueFontSize;
    movesBorderRadius = scoreBorderRadius;

    /* Define help text frame position and font size */
    help.x = PADDING;
    help.y = moves.y + moves.height + PADDING;
    help.width = (WIDTH - PADDING * 2);
    help.height = help.width * 0.4;
    helpFontSize = help.height * 0.125;

    TraceLog(LOG_INFO, "[GAME] Init game over screen");
}

void UpdateGameOverScreen(void)
{
    if (IsKeyPressed(KEY_ENTER)) 
    {
        NewGame();
        TransitionToScreen(SCREEN_GAMEPLAY);
    }
}

void DrawGameOverScreen(void)
{
    ClearBackground(SCREEN_BACKGROUND_COLOR);
    DrawTitle();
    DrawScore();
    DrawMoves();
    DrawHelp();
}

void UnloadGameOverScreen(void)
{
    TraceLog(LOG_INFO, "[GAME] Unload game over screen");
}

static void DrawTitle(void)
{
    float x, y;

    x = title.x + (title.width * 0.5) - MeasureText(textTitle, titleFontSize) * 0.5;
    y = title.y + (title.height * 0.5) - (titleFontSize * 0.5);

    DrawText(textTitle, x, y, titleFontSize, DEFAULT_TEXT_COLOR);
}

static void DrawScore(void)
{
    float x, y;
    char buffer[MAX_SCORE_BUFFER_SIZE];

    DrawRoundedRectangleRec(score, scoreBorderRadius, CELL_BACKGROUND_COLOR);

    /* Draw text in the top */
    x = score.x + (score.width * 0.5) - MeasureText(textScore, scoreTextFontSize) * 0.5;
    y = score.y + score.height * 0.1;

    DrawText(textScore, x, y, scoreTextFontSize, WHITE);

    /* Draw score value in the bottom */
    sprintf(buffer, "%d", GetScore());
    x = score.x + (score.width * 0.5) - MeasureText(buffer, scoreValueFontSize) * 0.5;
    y = score.y + score.height * 0.9 - scoreValueFontSize;

    DrawText(buffer, x, y, scoreValueFontSize, WHITE);
}

static void DrawMoves(void)
{
    float x, y;
    char buffer[MAX_SCORE_BUFFER_SIZE];

    DrawRoundedRectangleRec(moves, movesBorderRadius, CELL_BACKGROUND_COLOR);

    /* Draw text in the top */
    x = moves.x + (moves.width * 0.5) - MeasureText(textMoves, movesTextFontSize) * 0.5;
    y = moves.y + moves.height * 0.1;

    DrawText(textMoves, x, y, movesTextFontSize, WHITE);

    /* Draw moves value in the bottom */
    sprintf(buffer, "%d", GetMoves());
    x = moves.x + (moves.width * 0.5) - MeasureText(buffer, movesValueFontSize) * 0.5;
    y = moves.y + moves.height * 0.9 - movesValueFontSize;

    DrawText(buffer, x, y, movesValueFontSize, WHITE);
}

static void DrawHelp(void)
{
    float x, y;

    x = help.x + (help.width * 0.5) - MeasureText(textHelp, helpFontSize) * 0.5;
    y = help.y + (help.height * 0.5) - (helpFontSize * 0.5);

    DrawText(textHelp, x, y, helpFontSize, DEFAULT_TEXT_COLOR);
}
