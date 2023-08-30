#ifndef JOURNAL_H
#define JOURNAL_H
#define JOURNAL_DIR_PATH "/home/lemon/Documents/Notes/Archive"

/* Storage for text extracted from a line in a target file */
typedef struct
{
    char *line;
    char *filename;
} JournalLine;

char **get_relevant_files(int *linesize);
int journal_lines_insert_line_filename(JournalLine **lines, int *lineCount, const char *line, const char *filename);

#endif
