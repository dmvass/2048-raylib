#include "raylib.h"
#include "screens.h"

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

static const char *textTitle = "You win!";
static const char *textTile = "2048";
static const char *textHelp = "Press Enter to continue";

//-------------------------------------------------------------------------------------------------
// Game Win Screen Local Functions Declaration
//-------------------------------------------------------------------------------------------------
static void DrawTitle(void);
static void DrawTile(void);
static void DrawHelp(void);

//-------------------------------------------------------------------------------------------------
// Game Win Screen Functions Definition
//-------------------------------------------------------------------------------------------------
void InitGameWinScreen(void)
{
    /* define title position and font size */
    title.x = PADDING;
    title.y = PADDING * 2;
    title.width = (WIDTH - PADDING * 2);
    title.height = title.width * 0.25;
    titleFontSize = title.height * 0.7;

    /* define score position, font size and border radius */
    tile.width = WIDTH * 0.3;
    tile.height = tile.width;
    tile.x = (WIDTH - tile.width) * 0.5;
    tile.y = title.y + title.height + PADDING;
    tileFontSize = tile.height * 0.35;
    tileBorderRadius = tile.width * 0.05;

    /* define help text position and font size */
    help.x = PADDING;
    help.y = tile.y + tile.height + PADDING;
    help.width = (WIDTH - PADDING * 2);
    help.height = help.width * 0.4;
    helpFontSize = help.height * 0.125;

    TraceLog(LOG_INFO, "[GAME] Init game win screen");
}

void UpdateGameWinScreen(void)
{
    if (IsKeyPressed(KEY_ENTER)) 
        TransitionToScreen(SCREEN_GAMEPLAY);
}

void DrawGameWinScreen(void)
{
    ClearBackground(SCREEN_BACKGROUND_COLOR);
    DrawTitle();
    DrawTile();
    DrawHelp();
}

void UnloadGameWinScreen(void)
{
    TraceLog(LOG_INFO, "[GAME] Unload game win screen");
}

static void DrawTitle(void)
{
    float x, y;

    x = title.x + (title.width * 0.5) - MeasureText(textTitle, titleFontSize) * 0.5;
    y = title.y + (title.height * 0.5) - (titleFontSize * 0.5);

    DrawText(textTitle, x, y, titleFontSize, DEFAULT_TEXT_COLOR);
}

static void DrawTile(void)
{
    float x, y;

    x = tile.x + (tile.width * 0.5) - MeasureText(textTile, tileFontSize) * 0.5;
    y = tile.y + (tile.height * 0.5) - (tileFontSize * 0.5);

    DrawRoundedRectangleRec(tile, tileBorderRadius, COLOR_2048);
    DrawText(textTile, x, y, tileFontSize, TILE_FONT_WHITE_COLOR);
}

static void DrawHelp(void)
{
    float x, y;

    x = help.x + (help.width * 0.5) - MeasureText(textHelp, helpFontSize) * 0.5;
    y = help.y + (help.height * 0.5) - (helpFontSize * 0.5);

    DrawText(textHelp, x, y, helpFontSize, DEFAULT_TEXT_COLOR);
}
