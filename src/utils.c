#include <stdlib.h>
#include <string.h>

#include "raylib.h"


char* StrConcat(const char *s1, const char *s2)
{
    char *buffer;

    buffer = malloc(strlen(s1) + strlen(s2) + 1);
    if (!buffer)
    {
        TraceLog(LOG_ERROR, "Can't allocate memory");
        return NULL;
    }
    strcpy(buffer, s1);
    strcat(buffer, s2);
    return buffer;
}
