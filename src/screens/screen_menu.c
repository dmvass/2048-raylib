#include <stdio.h>
#include <stdlib.h>

#include "raylib.h"
#include "screens.h"

//-------------------------------------------------------------------------------------------------
// Game Menu Screen Variables Definition (local to this module)
//-------------------------------------------------------------------------------------------------
enum {
    BUTTON_CONTINUE,
    BUTTON_NEW_GAME,
    BUTTON_EXIT,
} ActiveButton;

static Rectangle title;
static Rectangle continueButton;
static Rectangle newGameButton;
static Rectangle exitButton;
static Rectangle footer;

static float buttonBorderRadius;

static int titleFontSize;
static int continueButtonFontSize;
static int newGameButtonFontSize;
static int exitButtonFontSize;
static int footerFontSize;

static const char *textTitle = "2048";
static const char *textContinueButton = "Continue";
static const char *textNewGameButton = "New game";
static const char *textExitButton = "Exit";
static const char *textFooter = "Power by Raylib";

//-------------------------------------------------------------------------------------------------
// Game Menu Screen Local Functions Declaration
//-------------------------------------------------------------------------------------------------
static void DrawTitle(void);
static void DrawContinueButton(void);
static void DrawNewGameButton(void);
static void DrawExitButton(void);
static void DrawFooter(void);
static inline Color GetButtonColor(int);

//-------------------------------------------------------------------------------------------------
// Game Menu Screen Functions Definition
//-------------------------------------------------------------------------------------------------
void InitGameMenuScreen(void)
{
    ActiveButton = BUTTON_CONTINUE;  /* Make active first button */

    /* Define title frame position and font size */
    title.x = PADDING;
    title.y = PADDING;
    title.width = (WIDTH - PADDING * 2);
    title.height = title.width * 0.4;
    titleFontSize = title.height * 0.7;

    /* Define ontinue game button position and font size */
    continueButton.width = WIDTH * 0.6;
    continueButton.height = title.width * 0.17;
    continueButton.x = (WIDTH - continueButton.width) * 0.5;
    continueButton.y = title.y + title.height + PADDING * 0.5;
    continueButtonFontSize = continueButton.height * 0.35;

    /* Define new game button position and font size */
    newGameButton.width = continueButton.width;
    newGameButton.height = continueButton.height;
    newGameButton.x = continueButton.x;
    newGameButton.y = continueButton.y + continueButton.height + PADDING * 0.5;
    newGameButtonFontSize = continueButtonFontSize;

    /* Define exit button position and font size */
    exitButton.width = continueButton.width;
    exitButton.height = continueButton.height;
    exitButton.x = continueButton.x;
    exitButton.y = newGameButton.y + newGameButton.height + PADDING * 0.5;
    exitButtonFontSize = continueButtonFontSize;

    /* Define footer frame position and font size */
    footer.width = (WIDTH - PADDING * 2);
    footer.height = footer.width * 0.1;
    footer.x = PADDING;
    footer.y = HEIGHT - footer.height - PADDING;
    footerFontSize = footer.height * 0.7;

    /* Define buttons general options */
    buttonBorderRadius = continueButton.width * 0.01;

    TraceLog(LOG_INFO, "[GAME] Init game menu screen");
}

void UpdateGameMenuScreen(void)
{
    if (IsKeyPressed(KEY_UP))
    {
        if (ActiveButton > BUTTON_CONTINUE)
            ActiveButton--;
        else
            ActiveButton = BUTTON_EXIT;
    }

    if (IsKeyPressed(KEY_DOWN)) 
    {
        if (ActiveButton < BUTTON_EXIT)
            ActiveButton++;
        else
            ActiveButton = BUTTON_CONTINUE;
    }

    if (IsKeyPressed(KEY_ENTER))
    {
        switch (ActiveButton)
        {
            case BUTTON_CONTINUE: 
                TransitionToScreen(SCREEN_GAMEPLAY);
                break;
            case BUTTON_NEW_GAME:
                NewGame();
                TransitionToScreen(SCREEN_GAMEPLAY);
                break;
            case BUTTON_EXIT:
                exit(EXIT_SUCCESS);
                break;
        }
    }

    if (IsKeyPressed(KEY_ESCAPE)) 
        TransitionToScreen(SCREEN_GAMEPLAY);
}

void DrawGameMenuScreen(void)
{
    ClearBackground(SCREEN_BACKGROUND_COLOR);
    DrawTitle();
    DrawContinueButton();
    DrawNewGameButton();
    DrawExitButton();
    DrawFooter();
}

void UnloadGameMenuScreen(void)
{
    TraceLog(LOG_INFO, "[GAME] Unload game menu screen");
}

static void DrawTitle(void)
{
    float x, y;

    x = title.x + (title.width * 0.5) - MeasureText(textTitle, titleFontSize) * 0.5;
    y = title.y + (title.height * 0.5) - (titleFontSize * 0.5);

    DrawText(textTitle, x, y, titleFontSize, DEFAULT_TEXT_COLOR);   
}

static void DrawContinueButton(void)
{
    float x, y;

    x = continueButton.x + (continueButton.width * 0.5) - MeasureText(textContinueButton, continueButtonFontSize) * 0.5;
    y = continueButton.y + (continueButton.height * 0.5) - (continueButtonFontSize * 0.5);

    DrawRoundedRectangleRec(continueButton, buttonBorderRadius, GetButtonColor(BUTTON_CONTINUE));
    DrawText(textContinueButton, x, y, continueButtonFontSize, TILE_FONT_WHITE_COLOR);
}

static void DrawNewGameButton(void)
{
    float x, y;

    x = newGameButton.x + (newGameButton.width * 0.5) - MeasureText(textNewGameButton, newGameButtonFontSize) * 0.5;
    y = newGameButton.y + (newGameButton.height * 0.5) - (newGameButtonFontSize * 0.5);

    DrawRoundedRectangleRec(newGameButton, buttonBorderRadius, GetButtonColor(BUTTON_NEW_GAME));
    DrawText(textNewGameButton, x, y, newGameButtonFontSize, TILE_FONT_WHITE_COLOR);
}

static void DrawExitButton(void)
{
    float x, y;

    x = exitButton.x + (exitButton.width * 0.5) - MeasureText(textExitButton, exitButtonFontSize) * 0.5;
    y = exitButton.y + (exitButton.height * 0.5) - (exitButtonFontSize * 0.5);

    DrawRoundedRectangleRec(exitButton, buttonBorderRadius, GetButtonColor(BUTTON_EXIT));
    DrawText(textExitButton, x, y, exitButtonFontSize, TILE_FONT_WHITE_COLOR);
}

static void DrawFooter(void)
{
    float x, y;

    x = footer.x + (footer.width * 0.5) - MeasureText(textFooter, footerFontSize) * 0.5;
    y = footer.y + (footer.height * 0.5) - (footerFontSize * 0.5);

    DrawText(textFooter, x, y, footerFontSize, DEFAULT_TEXT_COLOR);   
}


static inline Color GetButtonColor(int button)
{
    return (ActiveButton == button) ? BUTTON_ACTIVE_COLOR: BUTTON_COLOR;
}
