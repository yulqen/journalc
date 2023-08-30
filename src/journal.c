#include "journal.h"
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char **get_relevant_files(int *linesize)
{
    DIR *journal_dir;
    struct dirent *dir_information;
    struct stat file_stat;
    char fullpath[256];
    int linecount = 0;

    if ((journal_dir = opendir(JOURNAL_DIR_PATH)) == NULL)
    {
        perror("Error opening directory.");
        exit(1);
    }

    // First we have to iterate through the dir to count the entries
    // we want....
    while ((dir_information = readdir(journal_dir)) != 0)
    {
        strcpy(fullpath, JOURNAL_DIR_PATH);
        strcat(fullpath, "/");
        strcat(fullpath, dir_information->d_name);
        if (!stat(fullpath, &file_stat))
        {
            if (!S_ISDIR(file_stat.st_mode))
            {
                linecount++;
            }
        }
    }
    // Exposing the linecount to the caller
    *linesize = linecount;
    // Make sure to close the directory stream
    if ((closedir(journal_dir) != 0))
    {
        perror("Unable to close directory");
        exit(1);
    }

    // Request enough memory for array
    char **string_array = malloc(linecount * sizeof(char *));
    for (int i = 0; i < linecount; ++i)
    {
        // 256 is arbitary here; it means we can only store 256 chars
        string_array[i] = malloc(256 * sizeof(char));
    }

    // Now we open the directory again and create another stream
    if ((journal_dir = opendir(JOURNAL_DIR_PATH)) == NULL)
    {
        perror("Error opening directory.");
        exit(1);
    }
    // Reinitialise the struct for the next round of entries
    dir_information = NULL;

    int idx = 0;
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
                // get pointer three chars back from the end of the string
                const char *m_ext = fullpath + length - 3;
                const char *t_ext = fullpath + length - 4;
                if (strcmp(m_ext, ".md") == 0 || strcmp(t_ext, ".txt") == 0)
                {
                    /* printf("File: %s", fullpath); */
                    {
                        /* printf(" |-> extension is %s\n", m_ext); */
                        strcpy(string_array[idx], fullpath);
                        idx++;
                    }
                }
            }
        }
    }
    // Make sure to close the directory stream
    if ((closedir(journal_dir) != 0))
    {
        perror("Unable to close directory");
    }
    return string_array;
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
