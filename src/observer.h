#ifndef OBSERVER_H
#define OBSERVER_H

#define MAX_OBSERVERS  8

//-------------------------------------------------------------------------------------------------
// Types and Structures Definition
//-------------------------------------------------------------------------------------------------
typedef enum { ADD_TILE_EVENT, GAME_OVER_EVENT } Event;

typedef void (*Observer)(Event);

//-------------------------------------------------------------------------------------------------
// Functions Declaration
//-------------------------------------------------------------------------------------------------
int AttachObserver(Observer);
int DetachObserver(Observer);
void Notify(Event);

#endif  // OBSERVER_H
