#!/bin/sh

clang -o build/journalc -ggdb -larchive -I src/ src/date/date.c src/journal/j_archive.c src/main.c src/journal/journal.c
