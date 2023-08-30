#include <stdio.h>

#include "journal.h"

int main(int argc, char *argv[])
{
    parse_args(argc, argv);
    get_all_relevant_files();

    return 0;
}
