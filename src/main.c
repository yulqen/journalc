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
    char **dirs = (char *[]) {
        "/home/lemon/Documents/Notes/journal/home",
        "/home/lemon/Documents/Notes/MOD/work_journal",
        "/home/lemon/Documents/Notes/Archive"
    };
    int counter = 0;
    JournalLine **toss = putLinesFromRelevantFilesIntoJournalLines(&counter, dirs);
    for (int i = 0; i < counter; ++i)
    {
        /* FIXME: need to print the file header, then iterate through all the lines....*/ 
        printf("Received file: %s\n", toss[i]->filename);
        printf("Line: %s\n", toss[i]->line);
    }
    free(toss);
    return 0;
}
