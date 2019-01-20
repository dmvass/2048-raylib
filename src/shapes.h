#ifndef SHAPES_H
#define SHAPES_H

#include "raylib.h"
#include "rlgl.h"  // raylib OpenGL abstraction layer to OpenGL 1.1, 2.1, 3.3+ or ES2

/* Check if config flags have been externally provided on compilation line */
#if !defined(EXTERNAL_CONFIG_FLAGS)
    #include "config.h"  // Defines module configuration flags
#endif

void DrawRoundedRectangleRec(Rectangle rec, float radius, Color color);

#endif
