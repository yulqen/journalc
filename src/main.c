#include <stdio.h>
#include <stdlib.h>

#include "journal/journal.h"

int main(int argc, char *argv[])
{
    ParseArgs(argc, argv);
    char **dirs = (char *[])
    {
                "/home/lemon/Documents/Notes/journal/home",
                "/home/lemon/Documents/Notes/MOD/work_journal",
                "/home/lemon/Documents/Notes/Archive",
                "/home/lemon/Documents/Notes/journal/archives"
    };
    int idx = 0;
    JournalLine **toss = putLinesFromRelevantFilesIntoJournalLines(&idx, dirs, "Boris");
    for (int i = 0; i < idx; ++i)
    {
        /* FIXME: need to print the file header, then iterate through all the lines....*/
        printf("Received file: %s\n", toss[i]->filename);
        printf("Line: %s\n", toss[i]->line);
    }
    free(toss);
    return 0;
}
