#include "raylib.h"
#include "screens/screens.h"

#include <stdlib.h>

//-----------------------------------------------------------------------------
// Module Functions Declaration (local)
//-----------------------------------------------------------------------------
static void UpdateGame(void);    // Update game (one frame)
static void DrawGame(void);      // Draw game (one frame)

//-----------------------------------------------------------------------------
// Game main entry point
//-----------------------------------------------------------------------------
int main()
{
    // Initialization
    //-------------------------------------------------------------------------
    InitWindow(WIDTH, HEIGHT, TITLE);
    GameScreen = SCREEN_GAMEPLAY;
    InitGameplayScreen();

    SetTargetFPS(60);
    //-------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //---------------------------------------------------------------------
        UpdateGame();
        //---------------------------------------------------------------------

        // Draw
        //---------------------------------------------------------------------
        DrawGame();
        //---------------------------------------------------------------------
    }

    // De-Initialization
    //-------------------------------------------------------------------------  
    CloseWindow();        // Close window and OpenGL context
    //-------------------------------------------------------------------------

    return 0;
}

void UpdateGame(void)
{
    switch (GameScreen)
    {
        case SCREEN_LOGO: break;
        case SCREEN_MENU: break;
        case SCREEN_GAMEPLAY: DrawGameplayScreen(); break;
        case SCREEN_WON: break;
        case SCREEN_GAME_OVER: break;
    }
}

void DrawGame(void)
{
    BeginDrawing();
    switch (GameScreen)
    {
        case SCREEN_LOGO: break;
        case SCREEN_MENU: break;
        case SCREEN_GAMEPLAY: UpdateGameplayScreen(); break;
        case SCREEN_WON: break;
        case SCREEN_GAME_OVER: break;
    }
    EndDrawing();
}
