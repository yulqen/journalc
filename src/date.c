#include <time.h>
#include <stdio.h>
#include <stdlib.h>

char *today_date_basic_iso_format()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char *out = (char *)malloc(11);
    if (out != NULL)
    {
        sprintf(out, "%d-%02d-%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
    }
    return out;
}

