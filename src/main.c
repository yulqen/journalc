#include <stdio.h>

#include "journal.h"

int main(int argc, char *argv[])
{
    // TODO what needs doing:
    //  - Get all the markdown and text files using get_relevant_files()
    //  - Iterate through each file and seach for the search term
    //  - Create a JournalLine object based on the matching line and the file path
    //  - Add this object to an array.
    //  - Return this to the caller.
    //  - A list function takes this array and prints out the matching lines and file paths
    //      - In doing so, it highlights the search term
    //  - Everything is formatted nicely
    parse_args(argc, argv);
    int *s = 0;
    char **toss = get_relevant_files(s);

    return 0;
}
