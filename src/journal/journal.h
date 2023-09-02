#ifndef JOURNAL_H
#define JOURNAL_H

/* Storage for program options */
typedef struct { char *search_term; } Options;

/* Storage for text extracted from a line in a target file */
typedef struct{ char *line; char *filename; } JournalLine;

JournalLine *journalline_create(char *line, char *filename);
void journalline_destroy(JournalLine *jl);
JournalLine **putLinesFromRelevantFilesIntoJournalLines(int *counter, char **target_dirs);

/* Legacy functions */
int JournalLinePopulate(JournalLine **lines, int *lineCount, const char *line,
                                       const char *filename);
char **GetRelevantFiles(int *counter);
void GetAllRelevantFiles();
void GetLineFromFile(char *filepath);

void ParseArgs(int argc, char *const *argv);
#endif
