#include <stdlib.h>

#include "raylib.h"
#include "transition.h"
#include "screens/screens.h"

const char *windowTitle = "2048";

//-------------------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//-------------------------------------------------------------------------------------------------
static void UpdateGame(void);  // Update game (one frame)
static void DrawGame(void);    // Draw game (one frame)

//-------------------------------------------------------------------------------------------------
// Game main entry point
//-------------------------------------------------------------------------------------------------
int main()
{
    // Initialization
    //---------------------------------------------------------------------------------------------
    GameScreen = SCREEN_GAMEPLAY;

    InitWindow(WIDTH, HEIGHT, windowTitle);
    InitGameplayScreen();
    /* Create new game if game was't loaded */
    if (LoadGame() != 0) 
        NewGame();

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
    CloseWindow();  // Close window and OpenGL context
    //---------------------------------------------------------------------------------------------

    return 0;
}

void UpdateGame(void)
{
    switch (GameScreen)
    {
        case SCREEN_GAMEPLAY: 
            UpdateGameplayScreen();
            break;
        case SCREEN_GAME_WIN:
            UpdateGameWinScreen();
            break;
        case SCREEN_GAME_OVER:
            UpdateGameOverScreen();
            break;
        case SCREEN_GAME_MENU:
            UpdateGameMenuScreen();
            break;
    }

    if (OnTransition)
        UpdateTransition();
}

void DrawGame(void)
{
    BeginDrawing();
    switch (GameScreen)
    {
        case SCREEN_GAMEPLAY:
            DrawGameplayScreen();
            break;
        case SCREEN_GAME_WIN:
            DrawGameWinScreen();
            break;
        case SCREEN_GAME_OVER:
            DrawGameOverScreen();
            break;
        case SCREEN_GAME_MENU:
            DrawGameMenuScreen();
            break;
    }

    if (OnTransition)
        DrawTransition(SCREEN_BACKGROUND_COLOR);

#ifdef DEBUG
    DrawFPS(5, 5);
#endif

    EndDrawing();
}
