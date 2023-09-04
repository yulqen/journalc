#ifndef JOURNAL_H
#define JOURNAL_H
#include <archive.h>

// logging function
void write_log(const char *format, ...);

// Utility function
void ParseArgs(int argc, char *const *argv);

/* Storage for program options */
typedef struct
{
    char *search_term;
} Options;

/* Storage for text extracted from a line in a target file */
typedef struct
{
    char *line;
    char *filename;
} JournalLine;

// Allocates more memory for the array of JournalLine objects, jls.
void journalline_array_reallocate(const int *idx, int *capacity, JournalLine ***jls);

/* tgz handling */
struct archive *prepare_archive();

// Search through md and txt files
JournalLine **text_file_search(int *idx, const char *search_term, const char *fullpath, int capacity,
                               JournalLine **jls);

/* Given a suitably prepared archive (see prepare_archive()) and an array of JournalLine structs, will
 * search for search_term in filepath. Also requires to be passed a pointer to an active counter, idx.
 * */
JournalLine **tgz_search(int *idx, const char *search_term, const char *filepath, int capacity, JournalLine **jls);

/* An internal function called from tgz_search to do the actual finding. */
JournalLine **tgz_search_in_file(struct archive *a, JournalLine **jls, const char *search_term, int capacity, int *idx);

// Create a JournalLine object.
JournalLine *journalline_create(char *line, const char *filename);

// Destroy a Journalline object.
void journalline_destroy(JournalLine *jl);

// Main function that walks an array of directories and looks within md, txt and tgz files for
// the search term. Requires the number of directories to be searched - dir_count.
JournalLine **journal_search_directories_search_term(int *idx, int dir_count, char **target_dirs, char *search_term);

#endif
