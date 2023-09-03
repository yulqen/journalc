//
// Created by lemon on 02/09/23.
//
#include <archive.h>
#include <archive_entry.h>
#include <string.h>

#include "journal.h"

struct archive *prepare_archive()
{
    struct archive *a = archive_read_new();
    archive_read_support_filter_gzip(a);
    archive_read_support_format_tar(a);
    return a;
}

void tgz_open_and_search(struct archive *a, char *filepath, JournalLine **jls,
                         const char *search_term, int *idx)
{
    a = prepare_archive();
    int r = archive_read_open_filename(a, filepath, 10242);
    if (r != ARCHIVE_OK)
    {
        printf("Failed to open archive: %s \n", filepath);
        return;
    }
    tgz_search_in_file(a, jls, search_term, idx);
    archive_read_close(a);
    archive_read_free(a);
}
void tgz_search_in_file(struct archive *a, JournalLine **jls, const char *search_term, int *idx)
{
    struct archive_entry *entry;

    int count = 0;

    while (archive_read_next_header(a, &entry) == ARCHIVE_OK)
    {
        const char *filename = archive_entry_pathname(entry);
        //        printf("Currently reading: %s, iteration %d\n", filename, ++count);
        if (filename)
        {
            // Perform any necessary checks here
            if (archive_entry_filetype(entry) == AE_IFREG)
            {
                // biggest current file size in chars is 19408
                const size_t bufferSize = 1024 * 10;  // 10k buffer
                char buffer[bufferSize];

                size_t bytesRead;
                while ((bytesRead = archive_read_data(a, buffer, bufferSize)) > 0)
                {
                    //                    printf("Bytes read: %zu ", bytesRead);
                    const char *delim = "\n";
                    char *saveptr;
                    char *line = strtok_r(buffer, delim, &saveptr);
                    while (line != NULL)
                    {
                        //                        printf("idx outside of adding to array: %d\n",
                        //                        *idx); printf("%s\n", line);
                        char *ptr = strstr(line, search_term);
                        if (ptr)
                        {
                            JournalLine *jl = journalline_create(line, filename);
//                            printf("Adding %s to the array from %s\n", line,
//                                   archive_entry_pathname(entry));
                            jls[*idx] = jl;
                            (*idx)++;
                        }
                        line = strtok_r(NULL, delim, &saveptr);
                        printf("line: %s\n", line);
                    }
                }
            }
        }
    }
}