#!/bin/sh

clang -o build/journalc -I src/ src/date.c src/main.c src/journal.c
