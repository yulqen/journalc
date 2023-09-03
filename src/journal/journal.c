#include "journal.h"

#include <assert.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

Options opts;

JournalLine *journalline_create(char *line, const char *filename)
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

JournalLine **journal_search_directories_search_term(int *idx, char **target_dirs, char *search_term)
{
    assert(*idx == 0);
    DIR *journal_dir;
    struct dirent *dir_information;
    struct stat file_stat;
    char fullpath[256];
    int capacity = 20;
    JournalLine **jl_array = malloc(capacity * sizeof(JournalLine *));

    for (size_t i = 0; i < 4; i++)
    {
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
                    const char *tgz_ext = fullpath + length - 4;
                    if (strcmp(m_ext, ".md") == 0 || strcmp(t_ext, ".txt") == 0)
                    {
                        if (*idx == capacity)
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

                        // TODO: Here is where we need to parse the file
                        FILE *file = fopen(fullpath, "r");
                        if (file == NULL)
                        {
                            perror("Error opening file.");
                        }

                        char buf[2000]; // Assuming that we don't get many lines longer than 2000 characters

                        while (fgets(buf, sizeof buf, file) != NULL)
                        {
                            const char *delimeter = "\n";
                            char *line = strtok(buf, delimeter);
                            while (line != NULL)
                            {
                                char *ptr = strstr(buf, search_term);
                                if (ptr)
                                {
                                    JournalLine *jl = journalline_create(line, fullpath);
                                    jl_array[*idx] = jl;
                                    (*idx)++;
                                }
                                line = strtok(NULL, delimeter);
                            }
                        }
                        fclose(file);
                    } else if ((strcmp(tgz_ext, ".tgz") == 0))  {
                        // It is an archive file
                        struct archive *a = prepare_archive();
                        tgz_open_and_search(a, fullpath, jl_array, search_term, idx);
                    }
                }
            }
        }

        if ((closedir(journal_dir) != 0))
        {
            perror("Unable to close directory");
        }
    }
    return jl_array;
}

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