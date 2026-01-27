cmake_minimum_required(VERSION 3.15)

project(backoffAlgorithm LANGUAGES C)

# ------------------------------------------------------------------------------
# Includes
# ------------------------------------------------------------------------------

include(${CMAKE_CURRENT_LIST_DIR}/backoffAlgorithmFilePaths.cmake)

# ------------------------------------------------------------------------------
# Library targets
# ------------------------------------------------------------------------------

add_library(backoff_algorithm ${BACKOFF_ALGORITHM_SOURCES})

target_include_directories(backoff_algorithm PUBLIC ${BACKOFF_ALGORITHM_INCLUDE_PUBLIC_DIRS})
