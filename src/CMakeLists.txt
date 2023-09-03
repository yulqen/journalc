add_executable(${PROJECT_NAME} main.c
        journal/journal.c
        date/date.c
        journal/j_archive.c)

find_package(LibArchive REQUIRED)
target_link_libraries(${PROJECT_NAME} LibArchive::LibArchive)
add_subdirectory(journal)
add_subdirectory(date)