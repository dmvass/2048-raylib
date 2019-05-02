#ifndef RESOURCES_H
#define RESOURCES_H

#include <sys/param.h>  // PATH_MAX
#include "raylib.h"

#if defined(PLATFORM_OSX)
    #define SAVE_DIR   "/Library/Application Support/2048"
    #define SAVE_FILE  "/save.dat"
#else
    #define SAVE_DIR   "/2048"
    #define SAVE_FILE  "/save.dat"
#endif

//-------------------------------------------------------------------------------------------------
// Global Variable Declaration
//-------------------------------------------------------------------------------------------------

// SFX
extern Sound actionSound;
extern Sound appearSound;
extern Sound moveSound;
extern Sound mergeSound;

// Fonts
extern Font textFont;

// Save data
extern char saveDirPath[PATH_MAX];
extern char saveFilePath[PATH_MAX];

//-------------------------------------------------------------------------------------------------
// Functions Declaration
//-------------------------------------------------------------------------------------------------
void InitResources(const char *absolutepath);
void UnloadResources(void);

#endif  // RESOURCES_H
