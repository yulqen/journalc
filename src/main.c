#include <stdio.h>
#include "journal.h"


int main(int argc, char *argv[])
{
    parse_args(argc, argv);
    get_line_from_file();

    return 0;
}
