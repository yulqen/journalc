#include "journal.h"

#include <assert.h>
#include <dirent.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_BUFF_SIZE 2048
#define CAPACITY 200

Options opts;

/*
 *  const String kANSIReset = "\033[0m";
 *  const String kANSIYellowUnderline = "\033[33;1;4m";
 *
 */

char *highlight_search_term(char *s, char *target)
{
    char *out = calloc(MAX_BUFF_SIZE, MAX_BUFF_SIZE * 3 * sizeof(char));
    char before_target[MAX_BUFF_SIZE] = {0};
    char found_target[MAX_BUFF_SIZE] = {0};
    char after_target[MAX_BUFF_SIZE] = {0};
    char *t = strstr(s, target);
    if (t && strlen(target) < MAX_BUFF_SIZE)
    {
        if (strlen(s) > MAX_BUFF_SIZE)
        {
            fprintf(stderr, "\nThe string %s exceeds the maximum size allowed - not highlighting it.\n", s);
            char *out_err = malloc(strlen(s) + 1);  // Use strlen instead of sizeof
            if (out_err == NULL)
            {  // Check if memory allocation succeeded
                fprintf(stderr, "\nMemory allocation failed\n");
                return NULL;
            } else
            {
                strcpy(out_err, s);  // Use strcpy instead of snprintf
                free(out);
                return out_err;
            }
        }

        strncpy(before_target, s, t - s);
        strncpy(found_target, t, strlen(target));
        strncpy(after_target, t + strlen(target), MAX_BUFF_SIZE - strlen(target));
        snprintf(out, sizeof(out) * MAX_BUFF_SIZE, "%s\x1B[33;1;4m%s\x1B[0m%s", before_target, found_target,
                 after_target);

    } else
    {
        printf("Not found\n");
    }
    return out;
}

char *help =
    "Usage: %s\n"
    "Can do lots of stuff with journal files.\n"
    "Including:\n"
    "    -g|--grep [search_term]\n";

void parse_args(int argc, char *const *argv, char **search_term)
{
    if (argc != 3)
    {
        fprintf(stderr, help, argv[0]);
        exit(1);
    } else
    {
        if ((strcmp(argv[1], "-g") == 0) || (strcmp(argv[1], "--grep") == 0))
        {
            *search_term = malloc(strlen(argv[2]) * sizeof(char) + 1);
            strcpy(*search_term, argv[2]);
            printf("search_term is %s inside parse_args\n", *search_term);
        } else
        {
            fprintf(stderr, "We only accept either -g or --grep at this time.\n");
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

void journalline_array_reallocate(int *capacity, JournalLine ***jls)
{
    // ChatGPT comment: adding one here Also, although not directly causing your problem, in your re-allocation
    // function I notice
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
    DIR *dir;
    struct dirent *dir_information;
    int capacity = CAPACITY;
    JournalLine **jls = calloc(capacity, capacity * sizeof(JournalLine *));
    if (jls == NULL)
    {
        char *e = NULL;
        sprintf(e, "Unable to allocate memory for array of jls in %s at %d", __FILE__, __LINE__);
        perror(e);
    }

    for (int i = 0; i < dir_count; i++)
    {
        if ((dir = opendir(target_dirs[i])) == NULL)
        {
            perror("Error opening directory.");
            continue;
        }

        while ((dir_information = readdir(dir)) != 0)
        {
            char fullpath[256];
            /* if the entry is a regular file */
            if (dir_information->d_type == DT_REG)
            {
                strcpy(fullpath, target_dirs[i]);
                strcat(fullpath, "/");
                strcat(fullpath, dir_information->d_name);
                size_t length = strlen(fullpath);
                const char *m_ext = fullpath + length - 3;
                const char *t_ext = fullpath + length - 4;
                const char *tgz_ext = fullpath + length - 4;

                // If it is a normal text file (md or text)
                if (strcmp(m_ext, ".md") == 0 || strcmp(t_ext, ".txt") == 0 || strcmp(t_ext, ".org") == 0)
                {
                    jls = text_file_search(idx, search_term, fullpath, &capacity, jls);

                    // Or a tgz file
                } else if ((strcmp(tgz_ext, ".tgz") == 0))
                {
                    jls = tgz_search(idx, search_term, fullpath, &capacity, jls);
                }
            }
        }
        closedir(dir);
    }
    return jls;
}

JournalLine **text_file_search(int *idx, const char *search_term, const char *fullpath, int *capacity,
                               JournalLine **jls)
{
    // TODO: Here is where we need to parse the file
    FILE *file = fopen(fullpath, "r");
    if (file == NULL)
    {
        perror("Error opening file.");
    }
    size_t size = 2048;
    char *line = malloc(size * sizeof(char *));

    while (getline(&line, &size, file) != -1)
    {
        char *ptr = strstr(line, search_term);
        if (ptr)
        {
            printf("idx is: %d cap: %d\n", *idx, *capacity);
            if (*idx == *capacity - 1)
            {
                printf("Reallocating\n");
                journalline_array_reallocate(capacity, &jls);
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
