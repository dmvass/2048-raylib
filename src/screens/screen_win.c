#include "raylib.h"
#include "screens.h"
#include "../game.h"
#include "../resources.h"
#include "../shapes.h"

#define COLOR_TEXT       (Color){ 119, 110, 102, 255 }
#define COLOR_TILE       (Color){ 237, 194,  46, 255 }
#define COLOR_TILE_TEXT  (Color){ 249, 246, 242, 255 }

//-------------------------------------------------------------------------------------------------
// Local Variables Definition
//-------------------------------------------------------------------------------------------------
static Rectangle titleRec;
static Rectangle tileRec;
static Rectangle textRec;

static const char *titleText = "You win!";
static const char *tileText  = "2048";
static const char *text      = "Press Enter to continue";

static bool appearSoundPlayed;

//-------------------------------------------------------------------------------------------------
// Functions Definition
//-------------------------------------------------------------------------------------------------
void InitGameWinScreen(void)
{
    TraceLog(LOG_DEBUG, "Init game win screen");

    int width = GetScreenWidth();
    int height = GetScreenHeight();

    appearSoundPlayed = false;

    titleRec = (Rectangle){ width*0.08f, height*0.15f, width*0.84f, height*0.2f };
    tileRec  = (Rectangle){ width*0.35f, height*0.4f,  width*0.3f,  width*0.3f };
    textRec  = (Rectangle){ width*0.08f, height*0.65f, width*0.84f, height*0.05f };
}

void UpdateGameWinScreen(void)
{
    if (IsKeyPressed(KEY_ENTER))
    {
        GetGame()->state = GAME_PLAY;
        nextScreen       = SCREEN_PLAY;

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
    float font;
    Vector2 vector;

    ClearBackground(COLOR_SCREEN);

    // Draw title
    font = titleRec.height * 0.78f;
    vector = (Vector2) {
        titleRec.x + titleRec.width*0.5f - MeasureTextEx(textFont, titleText, font, 0).x*0.5f,
        titleRec.y + titleRec.height*0.5f - font*0.5f
    };
    DrawTextEx(textFont, titleText, vector, font, 0, COLOR_TEXT);

    // Draw tile
    font = tileRec.height * 0.35f;
    vector = (Vector2) {
        tileRec.x + tileRec.width*0.5f - MeasureText(tileText, font)*0.5f,
        tileRec.y + tileRec.height*0.5f - font*0.5f
    };
    DrawRoundedRectangleRec(tileRec, tileRec.width*0.05f, COLOR_TILE);
    DrawText(tileText, vector.x, vector.y, font, COLOR_TILE_TEXT);

    // Draw help text
    font = textRec.height * 0.78f;
    vector = (Vector2) {
        textRec.x + textRec.width*0.5f - MeasureTextEx(textFont, text, font, 0).x*0.5f,
        textRec.y + textRec.height*0.5f - font*0.5f
    };
    DrawTextEx(textFont, text, vector, font, 0, COLOR_TEXT);
}

void UnloadGameWinScreen(void)
{
    TraceLog(LOG_DEBUG, "Unload game win screen");
}
