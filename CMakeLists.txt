cmake_minimum_required(VERSION 3.11)
project(c_launcher C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(FetchContent)

# Disable unnecessary builds for raylib
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)


# set(ENABLE_LIB_ONLY ON)

# set(ENABLE_LIB_ONLY OFF)
# set(BUILD_SHARED_LIBS ON)
# set(ENABLE_STATIC_LIB ON)

set(BUILD_SHARED_LIBS ON)
set(ENABLE_STATIC_LIB OFF)

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

FetchContent_Declare(
    libcurl
    GIT_REPOSITORY https://github.com/curl/curl.git
    GIT_TAG curl-8_13_0
)
# Disable protocols and features
set(CURL_USE_OPENSSL OFF)
set(CURL_USE_LIBSSH2 OFF)
# set(PERL_FOUND OFF CACHE BOOL "" FORCE)
set(CURL_DISABLE_FTP ON)
set(CURL_DISABLE_FILE ON)
set(CURL_DISABLE_TFTP ON)
set(CURL_DISABLE_TELNET ON)
set(CURL_DISABLE_DICT ON)
set(CURL_DISABLE_GOPHER ON)
set(CURL_DISABLE_MQTT ON)
set(CURL_DISABLE_POP3 ON)
set(CURL_DISABLE_IMAP ON)
set(CURL_DISABLE_SMB ON)
set(CURL_DISABLE_SMTP ON)
set(CURL_DISABLE_RTSP ON)
set(CURL_DISABLE_LDAP ON)
# Disable optional dependencies
# set(CURL_DISABLE_LIBPSL ON) # Explicitly disable Libpsl
set(CURL_USE_LIBPSL OFF)
set(CURL_DISABLE_COOKIES ON)
set(CURL_DISABLE_CRYPTO_AUTH ON) # Disable NTLM, etc.
set(CURL_ZLIB OFF) # Disable Zlib
set(CURL_BROTLI OFF) # Disable Brotli
set(CURL_ZSTD OFF) # Disable Zstd
set(USE_NGHTTP2 OFF) # Disable HTTP/2
set(USE_LIBIDN2 OFF) # Disable HTTP/2
# Build options
set(BUILD_CURL_EXE OFF)
# set(BUILD_SHARED_LIBS OFF)
set(CURL_USE_SCHANNEL ON) # Windows SSL
set(ENABLE_CURL_MANUAL OFF) # Disable manuals (avoids Perl)
# Ensure minimal HTTP/HTTPS support
set(HTTP_ONLY ON) # Only HTTP/HTTPS protocols
set(BUILD_CURL_EXE OFF)
set(BUILD_EXAMPLES OFF)
set(BUILD_LIBCURL_DOCS OFF)
set(BUILD_MISC_DOCS OFF)
set(BUILD_TESTING OFF)

FetchContent_MakeAvailable(
  raylib 
  raygui 
  # bzip2
  bsdiff
  libcurl
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
    libcurl
    winmm
    gdi32
    user32
    shell32
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${raygui_SOURCE_DIR}/src
    ${bsdiff_SOURCE_DIR}/include
    ${libcurl_SOURCE_DIR}/include
    ${libcurl_BINARY_DIR}/include/curl # For generated curl_config.h
    # ${bsdiff_SOURCE_DIR}
    # ${bzip2_SOURCE_DIR}
    # ${CMAKE_CURRENT_SOURCE_DIR}/src
)


set(apps 
  # src/drop_file_box.c
  # src/core_drop_files.c
  # src/custom_file_dialog.c
  src/raygui_launcher.c
  src/raygui_diff_patch.c
)

foreach(app_source ${apps})
  # Get the filename without extension to use as executable name
  get_filename_component(app_name ${app_source} NAME_WE)
  
  # Create executable for each source file
  add_executable(${app_name} ${app_source})
  
  # Link libraries
  target_link_libraries(${app_name} PRIVATE 
    raylib
    bsdiff
    libcurl
    winmm
    gdi32
    user32
    shell32
  )
  
  # Set include directories
  target_include_directories(${app_name} PRIVATE 
    ${raygui_SOURCE_DIR}/src
    ${raygui_SOURCE_DIR}/examples/custom_file_dialog
    ${bsdiff_SOURCE_DIR}/include
    ${libcurl_SOURCE_DIR}/include
    ${libcurl_BINARY_DIR}/include/curl
  )
endforeach()


# Ensure Windows subsystem (no console)
# if (MSVC)
#     set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
# endif()

# Post-build step to copy DLLs to the output directory
if(WIN32) # Only apply this on Windows where DLLs are needed
  set(bsdiff_DLL_DEST "${CMAKE_BINARY_DIR}/_deps/bsdiff-build/Debug/bsdiff.dll")
  
  if(EXISTS "${bsdiff_DLL_DEST}")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_BINARY_DIR}/_deps/bsdiff-build/Debug/bsdiff.dll"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/bsdiff.dll"
        COMMENT "Copying bsdiff.dll to output directory..."
    )
  endif()
  set(raylib_DLL_DEST "${CMAKE_BINARY_DIR}/_deps/bsdiff-build/Debug/bsdiff.dll")
  if(EXISTS "${raylib_DLL_DEST}")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_BINARY_DIR}/_deps/raylib-build/raylib/Debug/raylib.dll"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/raylib.dll"
        COMMENT "Copying raylib.dll to output directory..."
    )
  endif()

  set(libcurl_DLL_DEST "${CMAKE_BINARY_DIR}/_deps/libcurl-build/lib/Debug/libcurl-d.dll")
  if(EXISTS "${libcurl_DLL_DEST}")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_BINARY_DIR}/_deps/libcurl-build/lib/Debug/libcurl-d.dll"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>/libcurl-d.dll"
        COMMENT "Copying libcurl-d.dll to output directory..."
    )
  endif()

endif()

# Debug messages
# message(STATUS "bsdiff source dir: ${bsdiff_SOURCE_DIR}")
# message(STATUS "bzip2 source dir: ${bzip2_SOURCE_DIR}")