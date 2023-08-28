#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  char *search_term;
} Options;

typedef struct {
  char *line;
  char *filename;
} JournalLine;

Options opts;

void parse_args(int argc, char *const *argv) {
  for (int i = 1; i < argc; i++) {
    char *argument = argv[i];
    // Handling the -g case, which is the search term
    {
      if (i < argc - 1) {  // protection against segmentation fault
        printf("Searching for (-g): %s\n", argv[i + 1]);
        opts.search_term = argv[i + 1];
      } else {
        printf("-g flag requires one argument\n");
        return;
      }
      i++;  // skip next argument as it's already used as -g value
    }
  }
}

int main(int argc, char *argv[]) {
  parse_args(argc, argv);
  FILE *file = fopen("2023-08-25.md", "r");
  JournalLine *journal_lines = NULL;
  int linecount = 0;
  char buffer[2000];
  while (fgets(buffer, sizeof(buffer), file)) {
    JournalLine newLine;
    newLine.line = strdup(buffer);
    if (newLine.line == NULL) {
      perror("Memory allocation error");
      break;
    }

    journal_lines =
        realloc(journal_lines, (linecount + 1) * sizeof(JournalLine));
    if (journal_lines == NULL) {
      perror("Memory allocation error");
      free(newLine.line);
      break;
    }
    journal_lines[linecount] = newLine;
    linecount++;
  }
  fclose(file);

  // access journal_lines using journal_lines[index];
  printf("First line: %s", journal_lines[0].line);

  for (int i = 0; i < linecount; ++i) {
    free(journal_lines[i].line);
  }
  free(journal_lines);
  return 0;
}
