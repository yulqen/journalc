#include <stdio.h>

#include "journal.h"

int main(int argc, char *argv[])
{
    parse_args(argc, argv);
    JournalLine jl = new_journalline("This is a new line", "toss.txt");
    printf("jl line is %s\n", jl.line);
    printf("jl filename is %s\n", jl.filename);
    del_journalline(&jl);

    return 0;
}
