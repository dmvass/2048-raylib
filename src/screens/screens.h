#ifndef SCREENS_H
#define SCREENS_H

#define TITLE "2048"

#define WIDTH  500
#define HEIGHT 750

enum {
    SCREEN_LOGO,
    SCREEN_MENU,
    SCREEN_GAMEPLAY,
    SCREEN_WON,
    SCREEN_GAME_OVER,
} GameScreen;

//-----------------------------------------------------------------------------
// Gameplay Screen Functions Declaration
//-----------------------------------------------------------------------------
void InitGameplayScreen(void);
void UpdateGameplayScreen(void);
void DrawGameplayScreen(void);
void UnloadGameplayScreen(void);
void NewGame(void);

#endif // SCREENS_H