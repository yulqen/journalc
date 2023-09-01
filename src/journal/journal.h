#ifndef JOURNAL_H
#define JOURNAL_H
#define JOURNAL_DIR_PATH "/home/lemon/Documents/Notes/Archive"

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

JournalLine *JournalLineCreate(char *line, char *filename);
void JournalLineDelete(JournalLine *jl);

void ParseArgs(int argc, char *const *argv);
char **GetRelevantFiles(int *counter);
int JournalLinePopulate(JournalLine **lines, int *lineCount, const char *line,
                                       const char *filename);
void GetAllRelevantFiles();
void GetLineFromFile(char *filepath);
#endif
