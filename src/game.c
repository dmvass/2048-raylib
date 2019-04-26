#include <stdlib.h>
#include <string.h>
#include <sys/param.h>  // PATH_MAX

#include "raylib.h"

#include "utils.h"
#include "screens/screens.h"

// #define DEBUG  1

//-------------------------------------------------------------------------------------------------
// Module Variables Definition 
//-------------------------------------------------------------------------------------------------
const int screenWidth = 420;
const int screenHeight = 640;

const char *windowTitle = "2048";

static bool onTransition;
static bool transFadeOut;
static float transAlpha;
static int transFromScreen;
static int transToScreen;
static int framesCounter;

//-------------------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//-------------------------------------------------------------------------------------------------
void UpdateGame(void);  // Update game (one frame)
void DrawGame(void);    // Draw game (one frame)

void UpdateTransition(void);
void DrawTransition(void);
void TransitionToScreen(const int screen);
void LoadSFX(const char *absolutepath);
void UnloadSFX(void);

//-------------------------------------------------------------------------------------------------
// Game main entry point
//-------------------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
    // Initialization
    //---------------------------------------------------------------------------------------------
    char absolutepath[PATH_MAX];

    onTransition = false;
    transFadeOut = false;
    transAlpha = 0;
    transFromScreen = -1;
    transToScreen = -1;
    framesCounter = 0;
    currentScreen = nextScreen = GAME_PLAY;

    SetTraceLog(LOG_DEBUG | LOG_INFO | LOG_WARNING | LOG_ERROR);

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

    // Load game resources
    LoadSFX(absolutepath);

    // Initialize window and game screen
    InitWindow(screenWidth, screenHeight, windowTitle);
    InitAudioDevice();
    InitGameplayScreen();

    // Create new game if game was't loaded
    if (LoadGame() != 0)
    {
        MakeSaveDir();
        NewGame();
    } 

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
    UnloadSFX();
    //---------------------------------------------------------------------------------------------

    return 0;
}

void UpdateGame(void)
{
    if (!onTransition)
    {
        switch (currentScreen)
        {
            case GAME_PLAY: UpdateGameplayScreen(); break;
            case GAME_WIN: UpdateGameWinScreen(); break;
            case GAME_OVER: UpdateGameOverScreen(); break;
            case MENU: UpdateMenuScreen(); break;
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
        case GAME_PLAY: DrawGameplayScreen(); break;
        case GAME_WIN: DrawGameWinScreen(); break;
        case GAME_OVER: DrawGameOverScreen(); break;
        case MENU: DrawMenuScreen(); break;
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
    transFromScreen = currentScreen;
    transToScreen = screen;
}

void UpdateTransition(void)
{
    if (!transFadeOut)
    {
        if ((transAlpha += 0.05) >= 1.0)
        {
            switch (transFromScreen)
            {
                case GAME_PLAY: UnloadGameplayScreen(); break;
                case GAME_WIN: UnloadGameWinScreen(); break;
                case GAME_OVER: 
                    UnloadGameOverScreen(); 
                    NewGame();
                    break;
                case MENU: UnloadMenuScreen(); break;
                default: break;
            }

            switch (transToScreen)
            {
                case GAME_PLAY: InitGameplayScreen(); break;
                case GAME_WIN: InitGameWinScreen(); break;
                case GAME_OVER: InitGameOverScreen(); break;
                case MENU: InitMenuScreen(); break;
                default: break;
            }

            transAlpha = 1.0;
            transFadeOut = true;
            framesCounter = 0;
            currentScreen = transToScreen;
        }
    }
    else  // Transition fade out logic
    {
        if ((transAlpha -= 0.05) <= 0)
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
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(COLOR_SCREEN_DEFAULT, transAlpha));
}

void LoadSFX(const char *absolutepath)
{
#if defined(BUNDLE_OSX)
    char *soundpath;

    soundpath = StrConcat(absolutepath, "/../resources/audio/move.wav");
    moveSound = LoadSound(soundpath);
    free(soundpath);

    soundpath = StrConcat(absolutepath, "/../resources/audio/merge.wav");
    mergeSound = LoadSound(soundpath);
    free(soundpath);

    soundpath = StrConcat(absolutepath, "/../resources/audio/action.wav");
    actionSound = LoadSound(soundpath);
    free(soundpath);

    soundpath = StrConcat(absolutepath, "/../resources/audio/appear.wav");
    appearSound = LoadSound(soundpath);
    free(soundpath);
#else
    moveSound = LoadSound("resources/audio/move.wav");
    mergeSound = LoadSound("resources/audio/merge.wav");
    actionSound = LoadSound("resources/audio/action.wav");
    appearSound = LoadSound("resources/audio/appear.wav");
#endif
}

void UnloadSFX(void)
{
    UnloadSound(moveSound);
    UnloadSound(mergeSound);
    UnloadSound(actionSound);
    UnloadSound(appearSound);
}