set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/cmd)

# Add the binary tree directory to the search path for linking and include files
link_directories(${PROJECT_BINARY_DIR}/src/wordfreq)
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${PROJECT_BINARY_DIR}/src/wordfreq)

file(COPY DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
add_executable(wordfreq wordfreq.c wordfreq.h mmap.h)
