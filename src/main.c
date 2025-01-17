#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "journal/journal.h"

int main(int argc, char *argv[])
{
    char *search_term = NULL;
    parse_args(argc, argv, &search_term);

    char *dirs[] = {"/home/lemon/Documents/Notes/journal/home", "/home/lemon/Documents/Notes/MOD/work_journal",
                    "/home/lemon/Documents/Notes/Archive", "/home/lemon/Documents/Notes/journal/archives",
                    "/home/lemon/Documents/org"};
    // Calculate the size of the array of dicts
    int dir_count = sizeof dirs / sizeof dirs[0];
    int idx = 0;
    JournalLine **toss = NULL;
    toss = journal_search_directories_search_term(&idx, dir_count, dirs, search_term);
    char *current_fn = NULL;
    for (int i = 0; i < idx; ++i)
    {
        if (current_fn == NULL || strcmp(current_fn, toss[i]->filename) != 0)
        {
            printf("\n%s\n", toss[i]->filename);
            current_fn = toss[i]->filename;
        }
        char *hline = highlight_search_term(toss[i]->line, search_term);
        printf("%s\n", hline);
        free(hline);
    }
    for (int i = 0; i < idx; ++i)
    {
        free(toss[i]->line);
        free(toss[i]->filename);
        free(toss[i]);
    }
    printf("\nFound \"%s\" %d times.\n", search_term, idx);
    free(toss);
    free(search_term);
    return 0;
}
