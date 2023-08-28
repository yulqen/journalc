#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
  char *search_term;
} Options;

typedef struct
{
  char *line;
  char *filename;
} JournalLine;

Options opts;

int emplaceBack(JournalLine **lines, int *lineCount, const char *line,
                const char *filename)
{
  JournalLine newLine;
  newLine.line = strdup(line);
  if (newLine.line == NULL)
  {
    perror("Memory allocation error on adding line.");
    return 0;
  }

  newLine.filename = strdup(filename);
  if (newLine.filename == NULL)
  {
    perror("Memory allocation error on adding filename.");
    return 0;
  }
  *lines = realloc(*lines, (*lineCount + 1) * sizeof(JournalLine));
  if (*lines == NULL)
  {
    free(newLine.line);
    free(newLine.filename);
    perror("Memory allocation error.");
    return 0;
  }

  (*lines)[*lineCount] = newLine;
  (*lineCount)++;
  return 1;
}

void parse_args(int argc, char *const *argv)
{
  for (int i = 1; i < argc; i++)
  {
    char *argument = argv[i];
    // Handling the -g case, which is the search term
    {
      if (i < argc - 1)
      {  // protection against segmentation fault
        printf("Searching for (-g): %s\n", argv[i + 1]);
        opts.search_term = argv[i + 1];
      } else
      {
        printf("-g flag requires one argument\n");
        return;
      }
      i++;  // skip next argument as it's already used as -g value
    }
  }
}

int main(int argc, char *argv[])
{
  parse_args(argc, argv);
  FILE *file = fopen("2023-08-25.md", "r");
  if (file == NULL)
  {
    perror("Error opening file.");
    return 1;
  }

  // Create a pointer to JournalLine - intended as the starting
  // point for dynamically allocated memory
  // If you don't do that you could do
  //  struct Line *lines = malloc(lineCount * sizeof(struct Line));
  // below somewhere. But using malloc, we need the linecount so
  // we would have to run through the file first, then reset
  // in the file using seek to run through again to get the lines
  JournalLine *journal_lines = NULL;
  int linecount = 0;
  char buffer[2000];

  // Read each line from the file - the max chars read
  // from the line in the file is the size of the buffer
  // we have allocated above - so we won't go beyond it
  while (fgets(buffer, sizeof(buffer), file))
  {
    // Create a new struct
    if (!emplaceBack(&journal_lines, &linecount, buffer, "yonkers.txt"))
    {
      break;  // we failed to add the line
    }
  }
  fclose(file);

  // access journal_lines using journal_lines[index];
  printf("First line: %s", journal_lines[0].line);
  printf("Second line: %s", journal_lines[1].line);
  printf("Third line: %s", journal_lines[2].line);

  // free memory for each journal_line's line using free
  for (int i = 0; i < linecount; ++i)
  {
    free(journal_lines[i].line);
    free(journal_lines[i].filename);
  }
  // free memory allocated for the array too
  free(journal_lines);
  return 0;
}
