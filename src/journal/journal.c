#include "journal.h"

#include <assert.h>
#include <dirent.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

Options opts;

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

void write_log(const char *format, ...)
{
    va_list args;
    FILE *file = fopen("log.txt", "a");
    if (file == NULL)
    {
        perror("Error opening file.");
        return;
    }

    // Get current time
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    fprintf(file, "%02d:%02d:%02d: ", tm.tm_hour, tm.tm_min, tm.tm_sec);

    va_start(args, format);
    vfprintf(file, format, args);
    va_end(args);

    fprintf(file, "\n");
    fclose(file);
}

void journalline_array_reallocate(const int *idx, int *capacity, JournalLine ***jls)
{
    if (*idx == (*capacity))
    {  // Chat GPT commennt: adding one here Also, although not directly causing your problem, in your re-allocation function I notice
        // you're enlarging the capacity by a factor of 1.5 each time it's called. This approach (also known as
        // geometric expansion) is very common in dynamic array operations to get an amortized linear complexity.
        // However, casting float to int might cause trouble sometimes as it truncates the decimal part which can result
        // in unchanged capacity when the capacity is very small. To get this right, the new capacity should be
        // calculated as:
        (*capacity) = (int)((*capacity) * 1.5) + 1;
        JournalLine **new_array = realloc((*jls), (*capacity) * sizeof(JournalLine *));
        if (new_array == NULL)
        {
            perror("Something went wrong with the realloc.\n");
            exit(1);
        } else
        {
            (*jls) = new_array;
        }
    }
}

void journalline_destroy(JournalLine *jl)
{
    if (jl != NULL)
    {
        write_log("Freeing line: %s at %p; ", jl->line, jl->line);
        free(jl->line);
        write_log("in file: %s\n", jl->filename);
        free(jl->filename);
        //        free(jl);
    }
}

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

    jl->filename = malloc(strlen(filename) + 1);  // Allocate memory for the string and the null-terminator
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

JournalLine **journal_search_directories_search_term(int *idx, int dir_count, char **target_dirs, char *search_term)
{
    assert(*idx == 0);
    DIR *journal_dir;
    struct dirent *dir_information;
    struct stat file_stat;
    int capacity = 20;
    JournalLine **jls = malloc(capacity * sizeof(JournalLine *));

    for (size_t i = 0; i < dir_count; i++)
    {
        if ((journal_dir = opendir(target_dirs[i])) == NULL)
        {
            perror("Error opening directory.");
            exit(1);
        }

        while ((dir_information = readdir(journal_dir)) != 0)
        {
            char fullpath[256];
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

                    // If it is a normal text file (md or text)
                    if (strcmp(m_ext, ".md") == 0 || strcmp(t_ext, ".txt") == 0)
                    {
                        jls = text_file_search(idx, search_term, fullpath, capacity, jls);

                        // Or a tgz file
                    } else if ((strcmp(tgz_ext, ".tgz") == 0))
                    {
                        jls = tgz_search(idx, search_term, fullpath, capacity, jls);
                    }
                }
            }
        }
    }
    return jls;
}

JournalLine **text_file_search(int *idx, const char *search_term, const char *fullpath, int capacity, JournalLine **jls)
{
    // TODO: Here is where we need to parse the file
    FILE *file = fopen(fullpath, "r");
    if (file == NULL)
    {
        perror("Error opening file.");
    }

    char buf[2000];  // Assuming that we don't get many lines longer than 2000
                     // characters

    size_t size = 2048;
    char *line = malloc(size);

    while (getline(&line, &size, file) != -1)
    {
        char *ptr = strstr(line, search_term);
        if (ptr)
        {
            if (*idx == capacity - 1)
            {
                journalline_array_reallocate(idx, &capacity, &jls);
            }
            JournalLine *jl = journalline_create(line, fullpath);
            jls[*idx] = jl;
            (*idx)++;
        }
    }

    free(line);
    fclose(file);
    return jls;
}