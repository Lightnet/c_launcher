cmake_minimum_required(VERSION 3.11)
project(MyRaylibProject C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)



include(FetchContent)

# Disable unnecessary builds for raylib
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)

set(BUILD_SHARED_LIBS ON)
# set(ENABLE_LIB_ONLY ON)
set(ENABLE_STATIC_LIB ON)

# Fetch raylib
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
)

# Fetch raygui
FetchContent_Declare(
    raygui
    GIT_REPOSITORY https://github.com/raysan5/raygui.git
    GIT_TAG 4.0
)

# Fetch bzip2
# FetchContent_Declare(
#     bzip2
#     GIT_REPOSITORY https://github.com/libarchive/bzip2.git
#     GIT_TAG master
# )

# Fetch bsdiff
FetchContent_Declare(
    bsdiff
    GIT_REPOSITORY https://github.com/zhuyie/bsdiff.git
    GIT_TAG master
)

FetchContent_MakeAvailable(
  raylib 
  raygui 
  # bzip2
  bsdiff
)

# MSVC-specific definitions
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# Define the executable
add_executable(${PROJECT_NAME} 
  src/main.c
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE 
    raylib
    # bz2_static
    bsdiff
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${raygui_SOURCE_DIR}/src
    ${bsdiff_SOURCE_DIR}/include
    # ${bsdiff_SOURCE_DIR}
    # ${bzip2_SOURCE_DIR}
    # ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Post-build step to copy DLLs to the output directory
if(WIN32) # Only apply this on Windows where DLLs are needed
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_BINARY_DIR}/_deps/bsdiff-build/Debug/bsdiff.dll"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/bsdiff.dll"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_BINARY_DIR}/_deps/raylib-build/raylib/Debug/raylib.dll"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/raylib.dll"
        COMMENT "Copying DLLs to output directory..."
    )
endif()


# Debug messages
message(STATUS "bsdiff source dir: ${bsdiff_SOURCE_DIR}")
message(STATUS "bzip2 source dir: ${bzip2_SOURCE_DIR}")