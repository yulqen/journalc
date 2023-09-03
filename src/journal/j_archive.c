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

void operate_on_archive(struct archive *a, char *filepath, JournalLine **jls,
                        const char *search_term, int *idx)
{
    a = prepare_archive();
    int r = archive_read_open_filename(a, filepath, 10242);
    if (r != ARCHIVE_OK)
    {
        printf("Failed to open archive: %s \n", filepath);
        return;
    }
    get_lines_from_tgz(a, jls, search_term, idx);
    archive_read_close(a);
    archive_read_free(a);
}

void get_lines_from_tgz(struct archive *a, JournalLine **jls, const char *search_term, int *idx)
{
    struct archive_entry *entry;
    while (archive_read_next_header(a, &entry) == ARCHIVE_OK)
    {
        const char *filename = archive_entry_pathname(entry);
        if (filename)
        {
            // Perform any necessary checks here
            if (archive_entry_filetype(entry) == AE_IFREG)
            {
                // biggest current file size in chars is 19408
                const size_t bufferSize = 20480;  // Adjust as needed
                char buffer[bufferSize];

                size_t bytesRead;
                while ((bytesRead = archive_read_data(a, buffer, bufferSize)) > 0)
                {
                    const char *delimeter = "\n";
                    char *line = strtok(buffer, delimeter);
                    while (line != NULL)
                    {
                        char *ptr = strstr(line, search_term);
                        if (ptr)
                        {
                            JournalLine *jl = journalline_create(buffer, filename);
                            jls[*idx] = jl;
                            (*idx)++;
                        }
                        line = strtok(NULL, delimeter);
                    }
                }
            }
        }
    }
}