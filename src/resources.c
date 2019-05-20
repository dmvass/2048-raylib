#include <stdlib.h>  // getenv
#include <string.h>  // strcpy, strcat
#include "resources.h"

//-------------------------------------------------------------------------------------------------
// Local Variables Definition
//-------------------------------------------------------------------------------------------------
Sound actionSound;
Sound appearSound;
Sound moveSound;
Sound mergeSound;

Font textFont;

char saveDirPath[PATH_MAX];
char saveFilePath[PATH_MAX];

//-------------------------------------------------------------------------------------------------
// Local Functions Declaration
//-------------------------------------------------------------------------------------------------
void LoadSFX(const char *absolutepath);
void UnloadSFX(void);
void LoadFonts(const char *absolutepath);
void UnloadFonts(void);

//-------------------------------------------------------------------------------------------------
// Functions Definition
//-------------------------------------------------------------------------------------------------
void InitResources(const char *absolutepath)
{
#if defined(PLATFORM_WINDOWS)
    // Define game absolute save dir path
    strcpy(saveDirPath, getenv("USERPROFILE"));
    strcat(saveDirPath, "\\2048");

    // Define game absolute save file path
    strcpy(saveFilePath, saveDirPath);
    strcat(saveFilePath, "\\storage.data");
#elif defined(PLATFORM_OSX)
    // Define game absolute save dir path
    strcpy(saveDirPath, getenv("HOME"));
    strcat(saveDirPath, "/Library/Application Support/2048");

    // Define game absolute save file path
    strcpy(saveFilePath, saveDirPath);
    strcat(saveFilePath, "/storage.data");
#else
    #error Platform is undefined
#endif

    TraceLog(LOG_DEBUG, "Save path: %s", saveFilePath);

    LoadSFX(absolutepath);
    LoadFonts(absolutepath);
}

void UnloadResources(void)
{
    UnloadSFX();
    UnloadFonts();
}

//-------------------------------------------------------------------------------------------------
// Local Functions Definition
//-------------------------------------------------------------------------------------------------
void LoadSFX(const char *absolutepath)
{
#if defined(BUNDLE_OSX)
    char filepath[PATH_MAX];

    // Load sounds from OSX bundle
    strcpy(filepath, absolutepath);
    moveSound = LoadSound(strcat(filepath, "/../resources/audio/move.wav"));

    strcpy(filepath, absolutepath);
    mergeSound = LoadSound(strcat(filepath, "/../resources/audio/merge.wav"));

    strcpy(filepath, absolutepath);
    actionSound = LoadSound(strcat(filepath, "/../resources/audio/action.wav"));

    strcpy(filepath, absolutepath);
    appearSound = LoadSound(strcat(filepath, "/../resources/audio/appear.wav"));
#else
    // Load sounds
    actionSound = LoadSound("resources/audio/action.wav");
    appearSound = LoadSound("resources/audio/appear.wav");
    moveSound = LoadSound("resources/audio/move.wav");
    mergeSound = LoadSound("resources/audio/merge.wav");
#endif
}

void UnloadSFX(void)
{
    UnloadSound(actionSound);
    UnloadSound(appearSound);
    UnloadSound(moveSound);
    UnloadSound(mergeSound);
}

void LoadFonts(const char *absolutepath)
{
#if defined(BUNDLE_OSX)
    char filepath[PATH_MAX];

    strcpy(filepath, absolutepath);
    textFont = LoadFontEx(strcat(filepath, "/../resources/fonts/ClearSans-Bold.ttf"), 124, 0, 0);
    SetTextureFilter(textFont.texture, FILTER_BILINEAR);
#else
    textFont = LoadFontEx("resources/fonts/ClearSans-Bold.ttf", 124, 0, 0);
    SetTextureFilter(textFont.texture, FILTER_BILINEAR);
#endif
}

void UnloadFonts(void)
{
    UnloadFont(textFont);
}
