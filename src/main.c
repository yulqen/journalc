#include <stdio.h>
#include <stdlib.h>

#include "journal/journal.h"

int main(int argc, char *argv[])
{
    // TODO what needs doing:
    //  - Get all the markdown and text files using GetRelevantFiles()
    //  - Iterate through each file and seach for the search term
    //  - Create a JournalLine object based on the matching line and the file path
    //  - Add this object to an array.
    //  - Return this to the caller.
    //  - A list function takes this array and prints out the matching lines and file paths
    //      - In doing so, it highlights the search term
    //  - Everything is formatted nicely
    ParseArgs(argc, argv);
    char **toss = GetRelevantFiles();
    for (int i = 0; i < 3; ++i)
    {
        printf("Received file: %s\n", toss[i]);
    }
    free(toss);
    return 0;
}
