#ifndef SCREENS_H
#define SCREENS_H

#include "../shapes.h"
#include "../transition.h"

#define WIDTH    500
#define HEIGHT   750
#define PADDING  (WIDTH * 0.08)

#define SIZE 4
#define GRID_SIZE (SIZE * SIZE)

#define COLOR_2     CLITERAL{ 238, 228, 218, 255 }
#define COLOR_4     CLITERAL{ 237, 224, 200, 255 }
#define COLOR_8     CLITERAL{ 242, 177, 121, 255 }
#define COLOR_16    CLITERAL{ 245, 149,  99, 255 }
#define COLOR_32    CLITERAL{ 246, 124,  95, 255 }
#define COLOR_64    CLITERAL{ 246,  94,  59, 255 }
#define COLOR_128   CLITERAL{ 237, 207, 114, 255 }
#define COLOR_256   CLITERAL{ 237, 204,  97, 255 }
#define COLOR_512   CLITERAL{ 237, 200,  80, 255 }
#define COLOR_1024  CLITERAL{ 237, 197,  63, 255 }
#define COLOR_2048  CLITERAL{ 237, 194,  46, 255 }
#define COLOR_4096  CLITERAL{ 237, 112,  46, 255 }
#define COLOR_8192  CLITERAL{ 237,  76,  46, 255 }

#define SCREEN_BACKGROUND_COLOR  CLITERAL{ 250, 248, 239, 255 }
#define DEFAULT_TEXT_COLOR       CLITERAL{ 119, 110, 102, 255 }
#define TILE_FONT_WHITE_COLOR    CLITERAL{ 249, 246, 242, 255 }
#define TILE_FONT_GREY_COLOR     CLITERAL{ 119, 110, 102, 255 }
#define GRID_BACKGROUND_COLOR    CLITERAL{ 186, 173, 161, 255 }
#define CELL_BACKGROUND_COLOR    CLITERAL{ 204, 193, 181, 245 }
#define BUTTON_COLOR             CLITERAL{ 164, 147, 127, 255 }
#define BUTTON_ACTIVE_COLOR      COLOR_2048

#define MAX_SCORE_BUFFER_SIZE  12

enum {
    SCREEN_GAMEPLAY,
    SCREEN_GAME_WIN,
    SCREEN_GAME_OVER,
    SCREEN_GAME_MENU,
} GameScreen;

//-------------------------------------------------------------------------------------------------
// Gameplay Screen Functions Declaration
//-------------------------------------------------------------------------------------------------
void InitGameplayScreen(void);
void UpdateGameplayScreen(void);
void DrawGameplayScreen(void);
void UnloadGameplayScreen(void);
void NewGame(void);
int LoadGame(void);
unsigned int GetScore(void);
unsigned int GetMoves(void);

//-------------------------------------------------------------------------------------------------
// Game Over Screen Functions Declaration
//-------------------------------------------------------------------------------------------------
void InitGameOverScreen(void);
void UpdateGameOverScreen(void);
void DrawGameOverScreen(void);
void UnloadGameOverScreen(void);

//-------------------------------------------------------------------------------------------------
// Game Won Screen Functions Declaration
//-------------------------------------------------------------------------------------------------
void InitGameWinScreen(void);
void UpdateGameWinScreen(void);
void DrawGameWinScreen(void);
void UnloadGameWinScreen(void);

//-------------------------------------------------------------------------------------------------
// Game Menu Screen Functions Declaration
//-------------------------------------------------------------------------------------------------
void InitGameMenuScreen(void);
void UpdateGameMenuScreen(void);
void DrawGameMenuScreen(void);
void UnloadGameMenuScreen(void);

#endif // SCREENS_H
