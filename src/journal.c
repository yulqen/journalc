#include "journal.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "date.h"

Options opts;

JournalLine new_journalline(char *line, char *filename)
{
    // Start with the outer layer
    JournalLine *jl = malloc(sizeof(JournalLine));
    if (jl == NULL)
    {
        perror("Could not create JournalLine object.");
        exit(1);
    }
    // Then the data layer
    jl->line = malloc(strlen(line) * sizeof(char));
    jl->line = line;
    if (jl->line == NULL)
    {
        free(jl);
        perror("Could not create line member inside JournalLine.");
        exit(1);
    }
    jl->filename = malloc(strlen(filename) * sizeof(char));
    jl->filename = filename;
    if (jl->filename == NULL)
    {
        free(jl);
        perror("Could not create filename member inside JournalLine.");
        exit(1);
    }
    // If both layers worked - return the JournalLine
    return *jl;
}

void del_journalline(JournalLine *jl)
{
    if (jl != NULL)
    {
        free(jl->line);
        free(jl->filename);
        free(jl);
    }
}

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

char **get_relevant_files(int *linesize)
{
    DIR *journal_dir;
    struct dirent *dir_information;
    struct stat file_stat;
    char *fullpath = malloc(256 * sizeof(char));

    if ((journal_dir = opendir(JOURNAL_DIR_PATH)) == NULL)
    {
        perror("Error opening directory.");
        exit(1);
    }

    int idx = 0;
    int capacity = 20;
    char **string_array = malloc(capacity * sizeof(char *));

    while ((dir_information = readdir(journal_dir)) != 0)
    {
        strcpy(fullpath, JOURNAL_DIR_PATH);
        strcat(fullpath, "/");
        strcat(fullpath, dir_information->d_name);
        if (!stat(fullpath, &file_stat))
        {
            if (!S_ISDIR(file_stat.st_mode))
            {
                int length = strlen(fullpath);
                const char *m_ext = fullpath + length - 3;
                const char *t_ext = fullpath + length - 4;
                if (strcmp(m_ext, ".md") == 0 || strcmp(t_ext, ".txt") == 0)
                {
                    if (idx == capacity)
                    {
                        capacity = (int)(capacity * 1.5);
                        printf("Expanding array to %d\n", capacity);
                        char **new_array = realloc(string_array, capacity * sizeof(char *));
                        if (new_array == NULL)
                        {
                            perror("Something went wrong with the realloc.\n");
                            exit(1);
                        } else
                        {
                            string_array = new_array;
                        }
                    }

                    char *new_path = malloc((strlen(fullpath) + 1) * sizeof(char));
                    strcpy(new_path, fullpath);

                    string_array[idx] = new_path;
                    idx++;
                }
            }
        }
    }

    *linesize = idx;

    if ((closedir(journal_dir) != 0))
    {
        perror("Unable to close directory");
    }
    free(fullpath);
    return string_array;
}

// Free memory function
void free_relevant_files(char **string_array, int linesize)
{
    for (int i = 0; i < linesize; i++)
    {
        free(string_array[i]);
    }
    free(string_array);
}

int journal_lines_insert_line_filename(JournalLine **lines, int *lineCount, const char *line,
                                       const char *filename)
{
    JournalLine newLine;
    newLine.line = strdup(line);
    if (newLine.line == NULL)
    {
        perror("Memory allocation error on adding line.");
        return 1;
    }

    newLine.filename = strdup(filename);
    if (newLine.filename == NULL)
    {
        perror("Memory allocation error on adding filename.");
        return 1;
    }
    *lines = realloc(*lines, (*lineCount + 1) * sizeof(JournalLine));
    if (*lines == NULL)
    {
        free(newLine.line);
        free(newLine.filename);
        perror("Memory allocation error.");
        return 1;
    }

    (*lines)[*lineCount] = newLine;
    (*lineCount)++;
    return 0;
}

void get_line_from_file()
{
    char *date = today_date_basic_iso_format();
    printf("Doing this on %s\n", date);
    free(date);  // Not sure if you have to do this but it reduces warnings in valgrind

    /* Move all this code into a function and call it on each item from the array of filenames
     * gathered below
     */
    FILE *file = fopen("2023-08-25.md", "r");
    if (file == NULL)
    {
        perror("Error opening file.");
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
}

void get_all_relevant_files()
{
    /* This is the main code that calls get_relevant_files() */
    int s = 0; /* we use this to track the number of lines so we can free them */
    char **toss = get_relevant_files(&s);
    printf("There seemingly were %d lines.\n", s);
    for (int i = 0; i < s; ++i)
    {
        printf("%s\n", toss[i]);
    }
    free_relevant_files(toss, s);
}
