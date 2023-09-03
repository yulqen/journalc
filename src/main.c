#include <stdio.h>
#include <stdlib.h>

#include "journal/journal.h"

int main(int argc, char *argv[])
{
    ParseArgs(argc, argv);
    char *dirs[] = {
//        "/home/lemon/Documents/Notes/journal/home",
//        "/home/lemon/Documents/Notes/MOD/work_journal",
//        "/home/lemon/Documents/Notes/Archive",
        "/home/lemon/Documents/Notes/journal/archives"};
    // Calculate the size of the array of dicts
    int dir_count = sizeof dirs / sizeof dirs[0];

    int idx = 0;
    JournalLine **toss = journal_search_directories_search_term(&idx, dir_count, dirs, "Python");
    for (int i = 0; i < idx; ++i)
    {
        /* FIXME: need to print the file header, then iterate through all the lines....*/
        printf("Received file: %s\n", toss[i]->filename);
        printf("Line: %s\n", toss[i]->line);
    }
    free(toss);
    return 0;
}
