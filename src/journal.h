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

JournalLine new_journalline(char *line, char *filename);
void del_journalline(JournalLine *jl);

void parse_args(int argc, char *const *argv);
char **get_relevant_files(int *linesize);
int journal_lines_insert_line_filename(JournalLine **lines, int *lineCount, const char *line,
                                       const char *filename);
void get_all_relevant_files();
void get_line_from_file();

#endif
