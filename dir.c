#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#define JOURNAL_DIR_PATH "/home/lemon/Documents/Notes/Archive"

/* Used a lot of this:
 * https://stackoverflow.com/questions/1442116/how-to-get-the-date-and-time-values-in-a-c-program
 */

int main(int argc, char *argv[])
{
    DIR *journal_dir;
    struct dirent *dir_information;
    struct stat file_stat;
    char fullpath[256];

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
                int length = strlen(fullpath);
                // get pointer three chars back from the end of the string
                const char *m_ext = fullpath + length - 3;
                if (strcmp(m_ext, ".md") == 0)
                {
                    printf("File: %s", fullpath);
                    {
                        printf(" |-> extension is %s\n", m_ext);
                    }
                }
                const char *t_ext = fullpath + length - 4;
                if (strcmp(t_ext, ".txt") == 0)
                {
                    printf("File: %s", fullpath);
                    {
                        printf(" |-> extension is %s\n", t_ext);
                    }
                }
            }
        }
    }

    return 0;
}
