#include "raylib.h"
#include "screens.h"

#define COLOR_TILE_FONT  CLITERAL{ 249, 246, 242, 255 }

//-------------------------------------------------------------------------------------------------
// Game Win Screen Variables Definition (local to this module)
//-------------------------------------------------------------------------------------------------
static Rectangle title;
static Rectangle tile;
static Rectangle help;

static float tileBorderRadius;

static int titleFontSize;
static int tileFontSize;
static int helpFontSize;

static Sound appearSound;
static Sound actionSound;

static bool appearSoundPlayed;

static const char *textTitle = "You win!";
static const char *textTile = "2048";
static const char *textHelp = "Press Enter to continue";

//-------------------------------------------------------------------------------------------------
// Game Win Screen Functions Definition
//-------------------------------------------------------------------------------------------------
void InitGameWinScreen(void)
{
    int width = GetScreenWidth();
    int height = GetScreenHeight(); 

    /* Define title */
    title = (Rectangle){ width*0.08, height*0.15, width*0.84, height*0.2 };
    titleFontSize = title.height * 0.7;

    /* Define score */
    tile = (Rectangle){ width*0.35, height*0.4, width*0.3, width*0.3 };
    tileFontSize = tile.height * 0.35;
    tileBorderRadius = tile.width * 0.05;

    /* Define help text */
    help = (Rectangle){ width*0.08, width*0.3 + height*0.45, width*0.84, height*0.05 };
    helpFontSize = help.height * 0.7;

    appearSound = LoadSound("resources/appear.wav");
    actionSound = LoadSound("resources/action.wav");

    appearSoundPlayed = false;

    TraceLog(LOG_DEBUG, "Init game win screen");
}

void UpdateGameWinScreen(void)
{
    if (IsKeyPressed(KEY_ENTER)) 
    {
        nextScreen = GAME_PLAY;
        PlaySound(actionSound);
    }

    if (!appearSoundPlayed)
    {
        PlaySound(appearSound);
        appearSoundPlayed = true;
    }
}

void DrawGameWinScreen(void)
{
    float x, y;

    ClearBackground(COLOR_SCREEN_DEFAULT);

    /* Draw title */
    x = title.x + (title.width * 0.5) - MeasureText(textTitle, titleFontSize) * 0.5;
    y = title.y + (title.height * 0.5) - (titleFontSize * 0.5);

    DrawText(textTitle, x, y, titleFontSize, COLOR_TEXT_DEFAULT);

    /* Draw tile */
    x = tile.x + (tile.width * 0.5) - MeasureText(textTile, tileFontSize) * 0.5;
    y = tile.y + (tile.height * 0.5) - (tileFontSize * 0.5);

    DrawRoundedRectangleRec(tile, tileBorderRadius, COLOR_2048);
    DrawText(textTile, x, y, tileFontSize, COLOR_TILE_FONT);

    /* Draw help text */
    x = help.x + (help.width * 0.5) - MeasureText(textHelp, helpFontSize) * 0.5;
    y = help.y + (help.height * 0.5) - (helpFontSize * 0.5);

    DrawText(textHelp, x, y, helpFontSize, COLOR_TEXT_DEFAULT);
}

void UnloadGameWinScreen(void)
{
    UnloadSound(appearSound);
    UnloadSound(actionSound);
    TraceLog(LOG_DEBUG, "Unload game win screen");
}
