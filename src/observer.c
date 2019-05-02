#include <stdlib.h>
#include "observer.h"

//-------------------------------------------------------------------------------------------------
// Local Variables Definition 
//-------------------------------------------------------------------------------------------------
static Observer _observers[MAX_OBSERVERS];
static int _count = 0;

//-------------------------------------------------------------------------------------------------
// Functions Definition
//-------------------------------------------------------------------------------------------------
int AttachObserver(Observer observer)
{
    if (_count < MAX_OBSERVERS) 
    {
        _observers[_count++] = observer;
        return 0;
    }
    return -1;
}

int DetachObserver(Observer observer)
{
    if (_count > 0)
    {
        for (int i = 0; i < MAX_OBSERVERS; i++)
        {
            if (_observers[i] == observer)
            {
                _count--;

                _observers[i] = _observers[_count];
                _observers[_count] = NULL;

                return 0;
            }
        }
    }
    return -1;
}

void Notify(Event event)
{
    for (int i = 0; i < _count; i++)
    {
        Observer observer = _observers[i];
        observer(event);
    }
}
