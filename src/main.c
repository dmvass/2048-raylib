#include <stdlib.h>     // realpath
#include <string.h>     // strlen
#include <sys/param.h>  // PATH_MAX
#include "raylib.h"
#include "game.h"
#include "resources.h"
#include "screens/screens.h"

//-------------------------------------------------------------------------------------------------
// Module Variables Definition
//-------------------------------------------------------------------------------------------------
static const int screenWidth   = 420;
static const int screenHeight  = 640;

static const char *title = "2048";

static bool onTransition;
static bool transFadeOut;
static float transAlpha;
static int transToScreen;
static int framesCounter;

//-------------------------------------------------------------------------------------------------
// Local Module Functions Declaration
//-------------------------------------------------------------------------------------------------
void UpdateGame(void);  // Update game (one frame)
void DrawGame(void);    // Draw game (one frame)
void UpdateTransition(void);
void DrawTransition(void);
void TransitionToScreen(const int screen);

//-------------------------------------------------------------------------------------------------
// Game main entry point
//-------------------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
    // Initialization
    //---------------------------------------------------------------------------------------------
    char absolutepath[PATH_MAX];

    onTransition    = false;
    transFadeOut    = false;
    transAlpha      = 0;
    transToScreen   = -1;
    framesCounter   = 0;

    currentScreen = nextScreen = SCREEN_PLAY;

#ifdef DEBUG
    SetTraceLog(LOG_DEBUG | LOG_INFO | LOG_WARNING | LOG_ERROR);
#else
    SetTraceLog(LOG_WARNING | LOG_ERROR);
#endif

    // Define absolute path
    if (realpath(argv[0], absolutepath) == 0)
        TraceLog(LOG_ERROR, "realpath failed");

    for (int i = strlen(absolutepath); i > 0; i--)
    {
        if (absolutepath[i] == '/')
        {
            absolutepath[i] = 0;
            break;
        }
    }

    TraceLog(LOG_DEBUG, "Realpath: %s", absolutepath);

    // Initialize window and game screen
    InitWindow(screenWidth, screenHeight, title);
    InitAudioDevice();

    InitResources(absolutepath);

    InitGame();
    InitGameplayScreen();
    InitGameWinScreen();

    SetExitKey(0);
    SetTargetFPS(60);
    //---------------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())  // Detect window close button or ESC key
    {
        // Update
        //-----------------------------------------------------------------------------------------
        UpdateGame();
        //-----------------------------------------------------------------------------------------

        // Draw
        //-----------------------------------------------------------------------------------------
        DrawGame();
        //-----------------------------------------------------------------------------------------
    }

    // De-Initialization
    //---------------------------------------------------------------------------------------------
    CloseAudioDevice();
    CloseWindow();  // Close window and OpenGL context

    UnloadGameplayScreen();
    UnloadGameWinScreen();
    UnloadGame();
    UnloadResources();

    //---------------------------------------------------------------------------------------------

    return 0;
}

void UpdateGame(void)
{
    if (!onTransition)
    {
        switch (currentScreen)
        {
            case SCREEN_PLAY: UpdateGameplayScreen(); break;
            case SCREEN_WIN: UpdateGameWinScreen(); break;
            default: break;
        }

        if (currentScreen != nextScreen)
            TransitionToScreen(nextScreen);
    }
    else
        UpdateTransition();
}

void DrawGame(void)
{
    BeginDrawing();

    switch (currentScreen)
    {
        case SCREEN_PLAY: DrawGameplayScreen(); break;
        case SCREEN_WIN: DrawGameWinScreen(); break;
        default: break;
    }

    if (onTransition)
        DrawTransition();

#ifdef DEBUG
    DrawFPS(5, 5);
#endif

    EndDrawing();
}

void TransitionToScreen(const int screen)
{
    onTransition = true;
    transToScreen = screen;
}

void UpdateTransition(void)
{
    if (!transFadeOut)
    {
        if ((transAlpha += 0.05f) >= 1.0f)
        {
            transAlpha = 1.0f;
            transFadeOut = true;
            framesCounter = 0;
            currentScreen = transToScreen;
        }
    }
    else  // Transition fade out logic
    {
        if ((transAlpha -= 0.05f) <= 0)
        {
            transAlpha = 0;
            transFadeOut = false;
            transToScreen = -1;
            onTransition = false;
        }
    }
}

void DrawTransition(void)
{
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(COLOR_SCREEN, transAlpha));
}
