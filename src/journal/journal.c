#include "journal.h"

#include <assert.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

Options opts;

JournalLine *journalline_create(char *line, char *filename)
{
    JournalLine *jl = malloc(sizeof(JournalLine));
    if (jl == NULL)
    {
        perror("Could not create JournalLine object.");
        exit(1);
    }

    jl->line = malloc(strlen(line) + 1);  // Allocate memory for the string and the null-terminator
    if (jl->line == NULL)
    {
        free(jl);
        perror("Could not create line member inside JournalLine.");
        exit(1);
    }
    strcpy(jl->line, line);  // Copy the content of the input line

    jl->filename =
        malloc(strlen(filename) + 1);  // Allocate memory for the string and the null-terminator
    if (jl->filename == NULL)
    {
        free(jl->line);
        free(jl);
        perror("Could not create filename member inside JournalLine.");
        exit(1);
    }
    strcpy(jl->filename, filename);  // Copy the content of the input filename

    return jl;
}

void journallist_destroy(JournalLine *jl)
{
    if (jl != NULL)
    {
        free(jl->line);
        free(jl->filename);
        free(jl);
    }
}

/* Parses the command line arguments */
void ParseArgs(int argc, char *const *argv)
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

JournalLine **putLinesFromRelevantFilesIntoJournalLines(int *counter, char **target_dirs)
{
    assert(*counter == 0);
    int idx = *counter;
    DIR *journal_dir;
    struct dirent *dir_information;
    struct stat file_stat;
    char fullpath[256];
    int capacity = 20;
    JournalLine **jl_array = malloc(capacity * sizeof(JournalLine *));

    for (size_t i = 0; i < 2; i++)
    {
        assert(*counter == 0);
        if ((journal_dir = opendir(target_dirs[i])) == NULL)
        {
            perror("Error opening directory.");
            exit(1);
        }

        while ((dir_information = readdir(journal_dir)) != 0)
        {
            strcpy(fullpath, target_dirs[i]);
            strcat(fullpath, "/");
            strcat(fullpath, dir_information->d_name);
            if (!stat(fullpath, &file_stat))
            {
                if (!S_ISDIR(file_stat.st_mode))
                {
                    size_t length = strlen(fullpath);
                    const char *m_ext = fullpath + length - 3;
                    const char *t_ext = fullpath + length - 4;
                    if (strcmp(m_ext, ".md") == 0 || strcmp(t_ext, ".txt") == 0)
                    {
                        if (idx == capacity)
                        {
                            capacity = (int)(capacity * 1.5);
                            printf("Expanding array to %d\n", capacity);
                            JournalLine **new_array = realloc(jl_array, capacity * sizeof(char *));
                            if (new_array == NULL)
                            {
                                perror("Something went wrong with the realloc.\n");
                                exit(1);
                            } else
                            {
                                jl_array = new_array;
                            }
                        }

                        JournalLine *jl = journalline_create("toss", fullpath);
                        jl_array[idx] = jl;
                        idx++;
                    }
                }
            }
        }

        if ((closedir(journal_dir) != 0))
        {
            perror("Unable to close directory");
        }
    }
    *counter = idx;
    return jl_array;
}

//char **GetRelevantFiles(int *counter)
//{
//    assert(*counter == 0);
//    int idx = *counter;
//    DIR *journal_dir;
//    struct dirent *dir_information;
//    struct stat file_stat;
//    char fullpath[256];
//
//    if ((journal_dir = opendir(JOURNAL_DIR_PATH)) == NULL)
//    {
//        perror("Error opening directory.");
//        exit(1);
//    }
//
//    int capacity = 20;
//    char **string_array = malloc(capacity * sizeof(char *));
//
//    while ((dir_information = readdir(journal_dir)) != 0)
//    {
//        strcpy(fullpath, JOURNAL_DIR_PATH);
//        strcat(fullpath, "/");
//        strcat(fullpath, dir_information->d_name);
//        if (!stat(fullpath, &file_stat))
//        {
//            if (!S_ISDIR(file_stat.st_mode))
//            {
//                size_t length = strlen(fullpath);
//                const char *m_ext = fullpath + length - 3;
//                const char *t_ext = fullpath + length - 4;
//                if (strcmp(m_ext, ".md") == 0 || strcmp(t_ext, ".txt") == 0)
//                {
//                    if (idx == capacity)
//                    {
//                        capacity = (int)(capacity * 1.5);
//                        printf("Expanding array to %d\n", capacity);
//                        char **new_array = realloc(string_array, capacity * sizeof(char *));
//                        if (new_array == NULL)
//                        {
//                            perror("Something went wrong with the realloc.\n");
//                            exit(1);
//                        } else
//                        {
//                            string_array = new_array;
//                        }
//                    }
//
//                    char *new_path = strdup(fullpath);
//                    if (new_path == NULL)
//                    {
//                        perror("Memory allocation error for the new path.\n");
//                        exit(1);
//                    }
//                    string_array[idx] = new_path;
//                    idx++;
//                }
//            }
//        }
//    }
//
//    if ((closedir(journal_dir) != 0))
//    {
//        perror("Unable to close directory");
//    }
//    *counter = idx;
//    return string_array;
//}

// Free memory function
void FreeRelevantFiles(char **string_array, int linesize)
{
    for (int i = 0; i < linesize; i++)
    {
        free(string_array[i]);
    }
    free(string_array);
}

int JournalLinePopulate(JournalLine **lines, int *lineCount, const char *line, const char *filename)
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

void GetLineFromFile(char *filepath)
{
    FILE *file = fopen(filepath, "r");
    if (file == NULL)
    {
        perror("Error opening file.");
    }

    JournalLine jlines[100];  // fuck dynamic memory!

    int count = 0;
    char buf[2000];

    // Read each line from the file - the max chars read
    // from the line in the file is the size of the buf
    // we have allocated above - so we won't go beyond it
    while (fgets(buf, sizeof(buf), file) != NULL)
    {
        // Create a new struct
        jlines[count] = *journalline_create(buf, filepath);
        count++;
    }
    fclose(file);

    // access journal_lines using journal_lines[index];
    /* printf("These lines are from the struct:\n\n"); */
    for (int i = 0; i < 200; ++i)
    {
        // FIXME: This is where there segfault happens because we're trashing across 200 bits
        printf("%s\n", jlines[i].filename);
        printf("%s\n\n", jlines[i].line);
    }

    // free memory for each journal_line's line using free
    for (int i = 0; i < count; ++i)
    {
        free(jlines[i].line);
        free(jlines[i].filename);
    }
}

//void GetAllRelevantFiles()
//{
//    /* This is the main code that calls GetRelevantFiles() */
//    int s = 0; /* we use this to track the number of lines so we can free them */
//    char **toss = GetRelevantFiles(&s);
//    printf("There seemingly were %d lines.\n", s);
//    for (int i = 0; i < s; ++i)
//    {
//        printf("%s\n", toss[i]);
//    }
//    FreeRelevantFiles(toss, s);
//}
