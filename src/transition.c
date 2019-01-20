#include <stdbool.h>

#include "transition.h"
#include "screens/screens.h"

//-------------------------------------------------------------------------------------------------
// Transition Variables Definition 
//-------------------------------------------------------------------------------------------------
bool OnTransition = false;  // Public variable

static float transAlpha = 0;
static bool transFadeOut = false;
static int transToScreen = -1;
static int framesCounter = 0;

//-------------------------------------------------------------------------------------------------
// Transition Functions Definition
//-------------------------------------------------------------------------------------------------
void TransitionToScreen(const int screen)
{
    OnTransition = true;
    transToScreen = screen;
}

void UpdateTransition(void)
{
    if (!transFadeOut)
    {
        transAlpha += 0.05f;

        if (transAlpha >= 1.0)
        {
            switch (GameScreen)
            {
                case SCREEN_GAMEPLAY: 
                    UnloadGameplayScreen();
                    break;
                case SCREEN_GAME_WIN:
                    UnloadGameWinScreen();
                    break;
                case SCREEN_GAME_OVER:
                    UnloadGameOverScreen();
                    break;
                case SCREEN_GAME_MENU:
                    UnloadGameMenuScreen();
                    break;
            }

            transAlpha    = 1.0;
            transFadeOut  = true;
            framesCounter = 0;
            GameScreen    = transToScreen;

            switch (GameScreen)
            {
                case SCREEN_GAMEPLAY: 
                    InitGameplayScreen();
                    break;
                case SCREEN_GAME_WIN:
                    InitGameWinScreen();
                    break;
                case SCREEN_GAME_OVER:
                    InitGameOverScreen();
                    break;
                case SCREEN_GAME_MENU:
                    InitGameMenuScreen();
                    break;
            }
        }
    }
    else  // Transition fade out logic
    {
        transAlpha -= 0.05f;

        if (transAlpha <= 0)
        {
            transAlpha    = 0;
            transFadeOut  = false;
            transToScreen = -1;
            OnTransition  = false;
        }
    }
}

void DrawTransition(const Color color)
{
    DrawRectangle(0, 0, WIDTH, HEIGHT, Fade(color, transAlpha));
}