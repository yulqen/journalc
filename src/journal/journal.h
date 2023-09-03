#ifndef JOURNAL_H
#define JOURNAL_H
#include <archive.h>

// Utility function
void ParseArgs(int argc, char *const *argv);

/* Storage for program options */
typedef struct { char *search_term; } Options;

/* Storage for text extracted from a line in a target file */
typedef struct{ char *line; char *filename; } JournalLine;

/* tgz handling */
struct archive *prepare_archive();

/* Given a suitably prepared archive (see prepare_archive()) and an array of JournalLine structs, will
 * search for search_term in filepath. Also requires to be passed a pointer to an active counter, idx.
 * */
void tgz_open_and_search(struct archive *a, char *filepath, JournalLine **jls, const char *search_term, int *idx);

/* An internal function called from tgz_open_and_search to do the actual finding. */
void tgz_search_in_file(struct archive *a, JournalLine **jls, const char *search_term, int *idx);

// Create a JournalLine object.
JournalLine *journalline_create(char *line, const char *filename);

// Destroy a Journalline object.
void journalline_destroy(JournalLine *jl);

// Main function that walks an array of directories and looks within md, txt and tgz files for
// the search term. Requires the number of directories to be searched - dir_count.
JournalLine **journal_search_directories_search_term(int *idx, int dir_count, char **target_dirs, char *search_term);

/* ------------ Legacy functions ------------------------------ */
int JournalLinePopulate(JournalLine **lines, int *lineCount, const char *line,
                                       const char *filename);
char **GetRelevantFiles(int *counter);
void GetAllRelevantFiles();
void GetLineFromFile(char *filepath);

#endif
