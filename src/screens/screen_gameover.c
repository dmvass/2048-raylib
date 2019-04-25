#include <stdio.h>

#include "screens.h"

#define COLOR_SCORE_BG  CLITERAL{ 204, 193, 181, 245 }

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
static void DrawScore(void);
static void DrawMoves(void);

//-------------------------------------------------------------------------------------------------
// Game Over Screen Functions Definition
//-------------------------------------------------------------------------------------------------
void InitGameOverScreen(void)
{
    int width = GetScreenWidth();
    int height = GetScreenHeight();

    /* Define title */
    title = (Rectangle){ width*0.08, height*0.1, width*0.84, height*0.15 };
    titleFontSize = title.height * 0.7;

    /* Define score */
    score = (Rectangle){ width*0.3, height*0.3, width*0.4, height*0.15 };
    scoreTextFontSize = score.height * 0.25;
    scoreValueFontSize = score.height * 0.4;
    scoreBorderRadius = score.width * 0.05;

    /* Define moves */
    moves = (Rectangle){ width*0.3, height*0.5, width*0.4, height*0.15 };
    movesTextFontSize = scoreTextFontSize;
    movesValueFontSize = scoreValueFontSize;
    movesBorderRadius = scoreBorderRadius;

    /* Define help text frame position and font size */
    help = (Rectangle){ width*0.08, height*0.8, width*0.84, height*0.05 };
    helpFontSize = help.height * 0.7;

    TraceLog(LOG_DEBUG, "Init game over screen");
}

void UpdateGameOverScreen(void)
{
    if (IsKeyPressed(KEY_ENTER)) 
    {
        nextScreen = GAME_PLAY;
        PlaySound(actionSound);
    }
}

void DrawGameOverScreen(void)
{
    float x, y;

    ClearBackground(COLOR_SCREEN_DEFAULT);

    /* Draw title */
    x = title.x + (title.width * 0.5) - MeasureText(textTitle, titleFontSize) * 0.5;
    y = title.y + (title.height * 0.5) - (titleFontSize * 0.5);

    DrawText(textTitle, x, y, titleFontSize, COLOR_TEXT_DEFAULT);

    /* Draw score */
    DrawScore();

    /* Draw moves */
    DrawMoves();

    /* Draw help text */
    x = help.x + (help.width * 0.5) - MeasureText(textHelp, helpFontSize) * 0.5;
    y = help.y + (help.height * 0.5) - (helpFontSize * 0.5);

    DrawText(textHelp, x, y, helpFontSize, COLOR_TEXT_DEFAULT);
}

void UnloadGameOverScreen(void)
{
    TraceLog(LOG_DEBUG, "Unload game over screen");
}


static void DrawScore(void)
{
    float x, y;
    char buffer[MAX_SCORE_BUFFER_SIZE];

    DrawRoundedRectangleRec(score, scoreBorderRadius, COLOR_SCORE_BG);

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

    DrawRoundedRectangleRec(moves, movesBorderRadius, COLOR_SCORE_BG);

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
