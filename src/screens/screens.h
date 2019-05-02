#ifndef SCREENS_H
#define SCREENS_H

#define COLOR_SCREEN  (Color){ 250, 248, 239, 255 }

//-------------------------------------------------------------------------------------------------
// Types and Structures Definition
//-------------------------------------------------------------------------------------------------
typedef enum GameScreen { SCREEN_PLAY, SCREEN_WIN } GameScreen;

//-------------------------------------------------------------------------------------------------
// Global Variables Definition
//-------------------------------------------------------------------------------------------------
GameScreen currentScreen;
GameScreen nextScreen;

//-------------------------------------------------------------------------------------------------
// Gameplay Screen Functions Declaration
//-------------------------------------------------------------------------------------------------
void InitGameplayScreen(void);
void UpdateGameplayScreen(void);
void DrawGameplayScreen(void);
void UnloadGameplayScreen(void);

//-------------------------------------------------------------------------------------------------
// Game Win Screen Functions Declaration
//-------------------------------------------------------------------------------------------------
void InitGameWinScreen(void);
void UpdateGameWinScreen(void);
void DrawGameWinScreen(void);
void UnloadGameWinScreen(void);

#endif  // SCREENS_H
