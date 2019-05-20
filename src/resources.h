#ifndef RESOURCES_H
#define RESOURCES_H

#include <sys/param.h>  // PATH_MAX
#include "raylib.h"

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
