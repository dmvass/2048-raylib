#include <sys/stat.h>  // mkdir, S_IRWXU, S_IRGRP, S_IROTH
#include <errno.h>     // errno, EACCES, ENOENT
#include "raylib.h"    // TraceLog, LOG_DEBUG
#include "utils.h"

// Try to create the save directory if it's not exist
int MakeSaveDir(char *dirpath)
{
    errno = 0;

    if (mkdir(dirpath, S_IRWXU | S_IRGRP | S_IROTH) != 0)
    {
        switch (errno)
        {
            case EACCES:
                TraceLog(LOG_WARNING, "Permission denied");
                break;
            case ENOENT:
                TraceLog(LOG_WARNING, "No such directory");
                break;
            default:
                TraceLog(LOG_WARNING, "Save directory can't be created");
                break;
        }
        return -1;
    }

    TraceLog(LOG_DEBUG, "Save directory was created success");
    return 0;
}
