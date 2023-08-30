#!/bin/sh

clang -o build/journalc  -g -I src/ src/date.c src/main.c src/journal.c
