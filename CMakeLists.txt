# Include file path configuration for backoff algorithm library.
include(BackoffAlgorithmFilePaths.cmake)

# Library target for backoff algorithm.
add_library( backoff_algorithm
               ${BACKOFF_ALGORITHM_SOURCES} )

target_include_directories( backoff_algorithm
                              PUBLIC
                                ${BACKOFF_ALGORITHM_INCLUDE_PUBLIC_DIRS} )

# Include unit tests.
if(BUILD_TESTS)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/utest)
endif()
