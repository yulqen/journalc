#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "journal.h"
#include "date.h"

/* Storage for program options */
typedef struct
{
    char *search_term;
} Options;

Options opts;

/*
 * This puts a line and a filename into a JournalLine and stores it in the lines
 array.
 * Initialise the array with something like:
 *
     if (!emplaceBack(&journal_lines, &linecount, buffer, "yonkers.txt"))
     {
         break;  // we failed to add the line
     }
 *
 * */

/* Parses the command line arguments */
void parse_args(int argc, char *const *argv)
{
    for (int i = 1; i < argc; i++)
    {
        {
            if (i < argc - 1)
            {  // protection against segmentation fault
                printf("Searching for (-g): %s\n", argv[i + 1]);
                opts.search_term = argv[i + 1];
            } else
            {
                printf("-g flag requires one argument\n");
                return;
            }
            i++;  // skip next argument as it's already used as -g value
        }
    }
}

int main(int argc, char *argv[])
{
    char *date = today_date_basic_iso_format();
    printf("Doing this on %s\n", date);
    free(date);  // Not sure if you have to do this but it reduces warnings in valgrind
    parse_args(argc, argv);

    /* Move all this code into a function and call it on each item from the array of filenames
     * gathered below
     */
    FILE *file = fopen("2023-08-25.md", "r");
    if (file == NULL)
    {
        perror("Error opening file.");
        return 1;
    }

    // Create a pointer to JournalLine - intended as the starting
    // point for dynamically allocated memory
    // If you don't do that you could do
    //  struct Line *lines = malloc(lineCount * sizeof(struct Line));
    // below somewhere. But using malloc, we need the linecount so
    // we would have to run through the file first, then reset
    // in the file using seek to run through again to get the lines
    JournalLine *journal_lines = NULL;
    int linecount = 0;
    char buffer[2000];

    // Read each line from the file - the max chars read
    // from the line in the file is the size of the buffer
    // we have allocated above - so we won't go beyond it
    while (fgets(buffer, sizeof(buffer), file))
    {
        // Create a new struct
        if (!journal_lines_insert_line_filename(&journal_lines, &linecount, buffer, "yonkers.txt"))
        {
            break;  // we failed to add the line
        }
    }
    fclose(file);

    // access journal_lines using journal_lines[index];
    printf("These lines are from the struct:\n\n");
    printf("%s", journal_lines[0].line);
    // printf("%s", journal_lines[1].line);
    /* printf("%s", journal_lines[2].line); */

    // free memory for each journal_line's line using free
    for (int i = 0; i < linecount; ++i)
    {
        free(journal_lines[i].line);
        free(journal_lines[i].filename);
    }
    // free memory allocated for the array too
    free(journal_lines);

    /* This is the main code that calls get_relevant_files() */
    int s; /* we use this to track the number of lines so we can free them */
    char **toss = get_relevant_files(&s);
    printf("There seemingly were %d lines.\n", s);
    printf("%s\n", toss[0]);
    for (int i = 0; i < s; ++i)
    {
        free(toss[i]);
    }
    free(toss);

    return 0;
}
