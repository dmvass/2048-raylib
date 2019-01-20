#ifndef TRANSITION_H
#define TRANSITION_H

#include "raylib.h"

bool OnTransition;

//-------------------------------------------------------------------------------------------------
// Local Functions Declaration
//-------------------------------------------------------------------------------------------------
void TransitionToScreen(const int screen);
void UpdateTransition(void);
void DrawTransition(const Color color);

#endif
