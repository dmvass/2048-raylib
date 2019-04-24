#include <stdio.h>
#include <stdlib.h>

#include "raylib.h"
#include "screens.h"

#define COLOR_TEXT_WHITE     CLITERAL{ 249, 246, 242, 255 }
#define COLOR_BUTTON         CLITERAL{ 164, 147, 127, 255 }
#define COLOR_BUTTON_ACTIVE  COLOR_2048

//-------------------------------------------------------------------------------------------------
// Game Menu Screen Variables Definition (local to this module)
//-------------------------------------------------------------------------------------------------
enum { BUTTON_CONTINUE, BUTTON_NEW_GAME, BUTTON_EXIT } ActiveButton;

static Rectangle title;
static Rectangle continueButton;
static Rectangle newGameButton;
static Rectangle exitButton;

static float buttonBorderRadius;

static int titleFontSize;
static int buttonFontSize;

static Sound actionSound;

static const char *textTitle = "2048";
static const char *textContinueButton = "Continue";
static const char *textNewGameButton = "New game";
static const char *textExitButton = "Exit";

//-------------------------------------------------------------------------------------------------
// Game Menu Screen Local Functions Declaration
//-------------------------------------------------------------------------------------------------
static inline Color GetButtonColor(int);

//-------------------------------------------------------------------------------------------------
// Game Menu Screen Functions Definition
//-------------------------------------------------------------------------------------------------
void InitMenuScreen(void)
{
    float buttonPositionX, buttonWidth, buttonHeight;

    int width = GetScreenWidth();
    int height = GetScreenHeight();

    /* Define title */
    title = (Rectangle){ width*0.08, height*0.05, width*0.84, height*0.25 };
    titleFontSize = title.height * 0.7;

    /* Define buttons */
    buttonPositionX = width * 0.2;
    buttonWidth = width * 0.6;
    buttonHeight = height * 0.1;
    buttonFontSize = buttonHeight * 0.35;
    buttonBorderRadius = continueButton.width * 0.01;

    continueButton = (Rectangle){ buttonPositionX, height*0.34, buttonWidth, buttonHeight };
    newGameButton = (Rectangle){ buttonPositionX, height*0.46, buttonWidth, buttonHeight };
    exitButton = (Rectangle){ buttonPositionX, height*0.58, buttonWidth, buttonHeight };
    ActiveButton = BUTTON_CONTINUE;  // Make active first button by default

    actionSound = LoadSound("resources/action.wav");

    TraceLog(LOG_DEBUG, "Init game menu screen");
}

void UpdateMenuScreen(void)
{
    if (IsKeyPressed(KEY_UP))
    {
        if (ActiveButton > BUTTON_CONTINUE)
            ActiveButton--;
        else
            ActiveButton = BUTTON_EXIT;
        
        PlaySound(actionSound);
    }

    if (IsKeyPressed(KEY_DOWN)) 
    {
        if (ActiveButton < BUTTON_EXIT)
            ActiveButton++;
        else
            ActiveButton = BUTTON_CONTINUE;
        
        PlaySound(actionSound);
    }

    if (IsKeyPressed(KEY_ENTER))
    {
        switch (ActiveButton)
        {
            case BUTTON_CONTINUE: 
                nextScreen = GAME_PLAY;
                break;
            case BUTTON_NEW_GAME:
                NewGame();
                nextScreen = GAME_PLAY;
                break;
            case BUTTON_EXIT:
                exit(EXIT_SUCCESS);
                break;
        }

        PlaySound(actionSound);
    }

    if (IsKeyPressed(KEY_ESCAPE))
    {
        nextScreen = GAME_PLAY;
        PlaySound(actionSound);
    }
}

void DrawMenuScreen(void)
{
    float x, y;

    ClearBackground(COLOR_SCREEN_DEFAULT);

    /* Draw title */
    x = title.x + (title.width * 0.5) - MeasureText(textTitle, titleFontSize) * 0.5;
    y = title.y + (title.height * 0.5) - (titleFontSize * 0.5);

    DrawText(textTitle, x, y, titleFontSize, COLOR_TEXT_DEFAULT);

    /* Draw continue button */
    x = continueButton.x + (continueButton.width * 0.5) - MeasureText(textContinueButton, buttonFontSize) * 0.5;
    y = continueButton.y + (continueButton.height * 0.5) - (buttonFontSize * 0.5);

    DrawRoundedRectangleRec(continueButton, buttonBorderRadius, GetButtonColor(BUTTON_CONTINUE));
    DrawText(textContinueButton, x, y, buttonFontSize, COLOR_TEXT_WHITE);

    /* Draw new game button */
    x = newGameButton.x + (newGameButton.width * 0.5) - MeasureText(textNewGameButton, buttonFontSize) * 0.5;
    y = newGameButton.y + (newGameButton.height * 0.5) - (buttonFontSize * 0.5);

    DrawRoundedRectangleRec(newGameButton, buttonBorderRadius, GetButtonColor(BUTTON_NEW_GAME));
    DrawText(textNewGameButton, x, y, buttonFontSize, COLOR_TEXT_WHITE);

    /* Draw exit button */
    x = exitButton.x + (exitButton.width * 0.5) - MeasureText(textExitButton, buttonFontSize) * 0.5;
    y = exitButton.y + (exitButton.height * 0.5) - (buttonFontSize * 0.5);

    DrawRoundedRectangleRec(exitButton, buttonBorderRadius, GetButtonColor(BUTTON_EXIT));
    DrawText(textExitButton, x, y, buttonFontSize, COLOR_TEXT_WHITE);
}

void UnloadMenuScreen(void)
{
    UnloadSound(actionSound);
    TraceLog(LOG_DEBUG, "Unload game menu screen");
}


static inline Color GetButtonColor(int button)
{
    return (ActiveButton == button) ? COLOR_BUTTON_ACTIVE: COLOR_BUTTON;
}
