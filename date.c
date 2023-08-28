#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

char *todayDate()
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char *out = (char *)malloc(11);
    if (out != NULL)
    {
        sprintf(out, "%d-%02d-%02d\n", tm.tm_year + 1900, tm.tm_mon + 1,
                tm.tm_mday);
    }
    return out;
}

int main(int argc, char *argv[])
{
    char *today = todayDate();
    if (today != NULL)
    {
        printf("Date: %s", today);
        free(today);
    } else
    {
        printf("Error obtaining date.\n");
    }
    return 0;
}
