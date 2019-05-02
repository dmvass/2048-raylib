#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "raylib.h"
#include "screens.h"
#include "../board.h"
#include "../game.h"
#include "../resources.h"
#include "../shapes.h"
#include "../utils.h"

#define COLOR_TEXT           (Color){ 249, 246, 242, 255 }
#define COLOR_TILE           (Color){ 237, 194,  46, 255 }
#define COLOR_BUTTON         (Color){ 164, 147, 127, 255 }
#define COLOR_SCORE          (Color){ 204, 193, 181, 245 }
#define COLOR_GAMEOVER_TEXT  (Color){ 119, 110, 102, 255 }

#define SCORE_MAX_BUFFER_SIZE       12
#define ANIMATION_GAME_OVER_FRAMES  120

//-------------------------------------------------------------------------------------------------
// Local Variables Definition
//-------------------------------------------------------------------------------------------------
static Rectangle tileRec;
static Rectangle scoreRec;
static Rectangle bestRec;
static Rectangle purposeRec;
static Rectangle retryRec;
static Rectangle boardRec;

static unsigned int gameOverFrames;

//-------------------------------------------------------------------------------------------------
// Local Functions Declaration
//-------------------------------------------------------------------------------------------------
static void HandleInput(void);

static void DrawTile(void);
static void DrawScore(void);
static void DrawBest(void);
static void DrawRetry(void);
static void DrawPurpose(void);
static void DrawGameOver(void);

//-------------------------------------------------------------------------------------------------
// Functions Definition
//-------------------------------------------------------------------------------------------------
void InitGameplayScreen(void)
{
    TraceLog(LOG_DEBUG, "Init gameplay screen");

    int width  = GetScreenWidth();
    int height = GetScreenHeight();

    gameOverFrames = 0;

    tileRec    = (Rectangle){ width*0.08f, height*0.05f, width*0.26f, width*0.26f };
    scoreRec   = (Rectangle){ width*0.46f, height*0.05f, width*0.21f, height*0.085f };
    bestRec    = (Rectangle){ width*0.7f,  height*0.05f, width*0.21f, height*0.085f };
    retryRec   = (Rectangle){ width*0.64f, height*0.16f, width*0.27f, height*0.06f };
    purposeRec = (Rectangle){ width*0.08f, height*0.26f, width*0.84f, height*0.06f };
    boardRec   = (Rectangle){ width*0.08f, height*0.34f, width*0.84f, width*0.84f };

    InitBoard(&boardRec);
}

void UpdateGameplayScreen(void)
{
    HandleInput();

    switch (GetGame()->state)
    {
    case GAME_PLAY:

        /*
         * Update Board only on gameplay state.
         */

        HandleBoardInput(&GetGame()->board);
        UpdateBoard(&GetGame()->board);
        break;

    case GAME_OVER:

        /*
         * Update game over animation.
         */

        if (gameOverFrames < ANIMATION_GAME_OVER_FRAMES)
        {
            gameOverFrames += 2;
        }

        break;

    case GAME_WIN:
        nextScreen = SCREEN_WIN;
        break;

    default:
        break;
    }
}

void DrawGameplayScreen(void)
{
    ClearBackground(COLOR_SCREEN);

    // Draw all screen elements
    DrawTile();
    DrawScore();
    DrawBest();
    DrawRetry();
    DrawPurpose();
    DrawBoard(&GetGame()->board);

    if (GetGame()->state == GAME_OVER)
    {
        DrawGameOver();
    }
}

void UnloadGameplayScreen(void)
{
    TraceLog(LOG_DEBUG, "Unload gameplay screen");
}

//-------------------------------------------------------------------------------------------------
// Local Functions Definition
//-------------------------------------------------------------------------------------------------
static void HandleInput()
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        struct Vector2 mousePos = GetMousePosition();

        if ((mousePos.x > retryRec.x && mousePos.x < retryRec.x + retryRec.width) &&
            (mousePos.y > retryRec.y && mousePos.y < retryRec.y + retryRec.height))
        {
            gameOverFrames = 0;
            NewGame();
            PlaySound(actionSound);
        }
    }

    if (GetGame()->state == GAME_OVER && IsKeyPressed(KEY_ENTER))
    {
        gameOverFrames = 0;
        NewGame();
        PlaySound(actionSound);
    }
}

static void DrawTile(void)
{
    static const char *text = "2048";

    float font = tileRec.height * 0.35f;
    Vector2 vector = (Vector2) {
        tileRec.x + tileRec.width*0.5f - MeasureText(text, font)*0.5f,
        tileRec.y + tileRec.height*0.5f - font*0.5f
    };
    DrawRoundedRectangleRec(tileRec, tileRec.width*0.05, COLOR_TILE);
    DrawText(text, vector.x, vector.y, font, COLOR_TEXT);
}

static void DrawScore(void)
{
    float font;
    Vector2 vector;
    char buffer[SCORE_MAX_BUFFER_SIZE];
    static const char *text = "SCORE";

    DrawRoundedRectangleRec(scoreRec, scoreRec.width*0.05f, COLOR_SCORE);  // Draw background

    // Draw text 'SCORE'
    font = scoreRec.height * 0.35f;
    vector = (Vector2) {
        scoreRec.x + scoreRec.width*0.5f - MeasureTextEx(textFont, text, font, 0).x*0.5f,
        scoreRec.y + scoreRec.height*0.1f
    };
    DrawTextEx(textFont, text, vector, font, 0, WHITE);

    // Draw score value
    sprintf(buffer, "%d", GetGame()->score);
    font = scoreRec.height * 0.32f;
    vector = (Vector2) {
        scoreRec.x + scoreRec.width*0.5f - MeasureText(buffer, font)*0.5f,
        scoreRec.y + scoreRec.height*0.9f - font
    };
    DrawText(buffer, vector.x, vector.y, font, WHITE);
}

static void DrawBest(void)
{
    float font;
    Vector2 vector;
    char buffer[SCORE_MAX_BUFFER_SIZE];
    static const char *text = "BEST";

    DrawRoundedRectangleRec(bestRec, bestRec.width*0.05f, COLOR_SCORE);  // Draw background

    // Draw text 'BEST'
    font = scoreRec.height * 0.35f;
    vector = (Vector2) {
        bestRec.x + bestRec.width*0.5f - MeasureTextEx(textFont, text, font, 0).x*0.5f,
        bestRec.y + bestRec.height*0.1f
    };
    DrawTextEx(textFont, text, vector, font, 0, WHITE);

    // Draw best value
    sprintf(buffer, "%d", GetGame()->best);
    font = scoreRec.height * 0.32f;
    vector = (Vector2) {
        bestRec.x + bestRec.width*0.5f - MeasureText(buffer, font)*0.5f,
        bestRec.y + bestRec.height*0.9f - font
    };
    DrawText(buffer, vector.x, vector.y, font, WHITE);
}

static void DrawRetry(void)
{
    static const char *text = "New Game";

    float font = retryRec.height * 0.58f;
    Vector2 vector = (Vector2) {
        retryRec.x + retryRec.width*0.5f - MeasureTextEx(textFont, text, font, 0).x*0.5f,
        retryRec.y + retryRec.height*0.5f - font*0.5f
    };
    DrawRoundedRectangleRec(retryRec, retryRec.width*0.04f, COLOR_BUTTON);
    DrawTextEx(textFont, text, vector, font, 0, COLOR_TEXT);
}

static void DrawPurpose(void)
{
    char buffer[64];
    static const char *text = "Join the numbers and get to the %d tile!";

    float font = purposeRec.height * 0.64f;
    Vector2 vector = (Vector2) {
        purposeRec.x,
        purposeRec.y + purposeRec.height*0.5f - font*0.5f
    };
    sprintf(buffer, text, 2 << (MAX(GetGame()->max, 10)));
    DrawTextEx(textFont, buffer, vector, font, 0, LIGHTGRAY);
}

static void DrawGameOver(void)
{
    float font;
    Vector2 vector;
    static const char *text1 = "Game Over!";
    static const char *text2 = "Press Enter to Try again";

    unsigned short alpha = gameOverFrames < 90 ? gameOverFrames * 2 : 180;

    DrawRoundedRectangleRec(boardRec, boardRec.width * 0.015, (Color){ 238, 228, 218, alpha });

    if (gameOverFrames > 30)
    {
        // Draw text1
        font = boardRec.width * 0.2f;
        vector = (Vector2) {
            boardRec.x + boardRec.width*0.5f - MeasureTextEx(textFont, text1, font, 0).x*0.5f,
            boardRec.y + boardRec.height*0.65f - font*0.5f - gameOverFrames
        };
        DrawTextEx(textFont, text1, vector, font, 0, COLOR_GAMEOVER_TEXT);

        // Draw text2
        font = boardRec.width * 0.08f;
        vector = (Vector2) {
            boardRec.x + boardRec.width*0.5f - MeasureTextEx(textFont, text2, font, 0).x*0.5f,
            boardRec.y + boardRec.height*0.8f - font*0.5f - gameOverFrames
        };
        DrawTextEx(textFont, text2, vector, font, 0, COLOR_GAMEOVER_TEXT);
    }
}
