add_executable(${PROJECT_NAME} main.c
        journal/journal.c
        date/date.c)
add_subdirectory(journal)
add_subdirectory(date)