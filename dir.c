#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#define JOURNAL_DIR_PATH "/home/lemon/Documents/Notes/Archive"

/* Used a lot of this:
 * https://stackoverflow.com/questions/1442116/how-to-get-the-date-and-time-values-in-a-c-program
 */

char **get_relevant_files()
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

    printf("Our markdown files: \n\n");

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

    char **string_array = malloc(linecount * sizeof(char *));
    for (int i = 0; i < linecount; ++i)
    {
        string_array[i] = malloc(256 * sizeof(char));
    }

    // TODO: Need to reopen this directory and go again
    /* if ((journal_dir = opendir(JOURNAL_DIR_PATH)) == NULL) */
    /* { */
    /*     perror("Error opening directory."); */
    /* } */
    struct dirent *dir_information2;

    while ((dir_information2 = readdir(journal_dir)) != 0)
    {
        strcpy(fullpath, JOURNAL_DIR_PATH);
        strcat(fullpath, "/");
        strcat(fullpath, dir_information->d_name);
        if (!stat(fullpath, &file_stat))
        {
            if (!S_ISDIR(file_stat.st_mode))
            {
                int idx = 0;
                int length = strlen(fullpath);
                // get pointer three chars back from the end of the string
                const char *m_ext = fullpath + length - 3;
                const char *t_ext = fullpath + length - 4;
                if (strcmp(m_ext, ".md") == 0 || strcmp(t_ext, ".txt") == 0)
                {
                    printf("File: %s", fullpath);
                    {
                        printf(" |-> extension is %s\n", m_ext);
                        strcpy(string_array[idx], fullpath);
                        idx++;
                    }
                }
            }
        }
    }
    return string_array;
}

int main(int argc, char *argv[])
{
    char **toss = get_relevant_files();
    printf("%s\n", toss[0]);
    free(toss);
    return 0;
}
