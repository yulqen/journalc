#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#define JOURNAL_DIR_PATH "/home/lemon/Documents/Notes/journal/home"

/* Used a lot of this:
 * https://stackoverflow.com/questions/1442116/how-to-get-the-date-and-time-values-in-a-c-program
 */

int main(int argc, char *argv[])
{
    DIR *journal_dir;
    struct dirent *info_archive;
    struct stat file_stat;
    char fullpath[256];

    if ((journal_dir = opendir(JOURNAL_DIR_PATH)) == NULL)
    {
        perror("Error opening directory.");
    }

    printf("Our markdown files: \n\n");

    while ((info_archive = readdir(journal_dir)) != 0)
    {
        strcpy(fullpath, "/home/lemon/Documents/Notes/journal/home");
        strcat(fullpath, "/");
        strcat(fullpath, info_archive->d_name);
        if (!stat(fullpath, &file_stat))
        {
            if (!S_ISDIR(file_stat.st_mode))
            {
                int length = strlen(fullpath);
                // get pointer three chars back from the end of the string
                const char *ext = fullpath + length - 3;
                if (strcmp(ext, ".md") == 0)
                {
                    printf("File: %s", fullpath);
                    {
                        printf(" |-> extension is %s\n", ext);
                    }
                }
            }
        }
    }

    return 0;
}
