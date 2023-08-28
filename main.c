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

  // Create a pointer to JournalLine - intended as the starting
  // point for dynamically allocated memory
  JournalLine *journal_lines = NULL;
  int linecount = 0;
  char buffer[2000];

  // Read each line from the file - the max chars read
  // from the line in the file is the size of the buffer
  // we have allocated above - so we won't go beyond it
  while (fgets(buffer, sizeof(buffer), file)) {
    // Create a new struct
    JournalLine newLine;

    // The line member of the struct is assigned the memory
    // address of a dynamically allocated string which
    // duplicates the content of the buffer - this is the
    // genius move here
    newLine.line = strdup(buffer);
    if (newLine.line == NULL) {
      perror("Memory allocation error");
      break;
    }

    // memory is allocated for the journal_lines array using
    // realloc. It increases the memory size to accommdate the
    // new line.
    // (linecount + 1) * sizeof(JournalLine) calculates the
    // required memory size
    journal_lines =
        realloc(journal_lines, (linecount + 1) * sizeof(JournalLine));
    if (journal_lines == NULL) {
      perror("Memory allocation error");
      free(newLine.line);
      break;
    }
    // the newline struct is added to the journal_lines array
    // and linecount is incremented
    journal_lines[linecount] = newLine;
    linecount++;
  }
  fclose(file);

  // access journal_lines using journal_lines[index];
  printf("First line: %s", journal_lines[0].line);
  printf("Second line: %s", journal_lines[1].line);
  printf("Third line: %s", journal_lines[2].line);

  // free memory for each journal_line's line using free
  for (int i = 0; i < linecount; ++i) {
    free(journal_lines[i].line);
  }
  // free memory allocated for the array too
  free(journal_lines);
  return 0;
}
