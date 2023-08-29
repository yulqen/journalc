#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#define JOURNAL_DIR_PATH "/home/lemon/Documents/Notes/Archive"

/* Used a lot of this:
 * https://stackoverflow.com/questions/1442116/how-to-get-the-date-and-time-values-in-a-c-program
 */

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

int main(int argc, char *argv[])
{
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
