# Include file path configuration for retry utils library.
include(retryUtilsFilePaths.cmake)

# Library target for retry utils.
add_library( backoff_algorithm
               ${BACKOFF_ALGORITHM_SOURCES} )

target_include_directories( backoff_algorithm
                              PUBLIC
                                ${BACKOFF_ALGORITHM_INCLUDE_PUBLIC_DIRS} )

# Include unit tests.
if(BUILD_TESTS)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/utest)
endif()
