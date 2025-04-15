#define WIN32_LEAN_AND_MEAN 
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define NOGDI
#define NOUSER
#define MMNOSOUND

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "miniz.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MESSAGE_SIZE 256

// Function to extract all files from a ZIP to the current directory
bool ExtractZipFile(const char *zipFile, char *errorMsg, int msgSize) {
    mz_zip_archive zip_archive = { 0 };
    bool success = true;

    // Initialize ZIP archive
    if (!mz_zip_reader_init_file(&zip_archive, zipFile, 0)) {
        snprintf(errorMsg, msgSize, "Failed to open ZIP file: %s", zipFile);
        return false;
    }

    // Get number of files in ZIP
    int fileCount = (int)mz_zip_reader_get_num_files(&zip_archive);
    if (fileCount == 0) {
        snprintf(errorMsg, msgSize, "ZIP file %s is empty", zipFile);
        mz_zip_reader_end(&zip_archive);
        return false;
    }

    // Iterate through all files in the ZIP
    for (int i = 0; i < fileCount; i++) {
        mz_zip_archive_file_stat file_stat = { 0 };
        if (!mz_zip_reader_file_stat(&zip_archive, i, &file_stat)) {
            snprintf(errorMsg, msgSize, "Failed to read file info at index %d", i);
            success = false;
            break;
        }

        // Skip directories
        if (mz_zip_reader_is_file_a_directory(&zip_archive, i)) {
            continue;
        }

        // Extract file to current directory
        char *filename = file_stat.m_filename;
        size_t extracted_size = 0;
        void *extracted_data = mz_zip_reader_extract_to_heap(&zip_archive, i, &extracted_size, 0);
        if (!extracted_data) {
            snprintf(errorMsg, msgSize, "Failed to extract file: %s", filename);
            success = false;
            break;
        }

        // Write extracted data to file
        FILE *outFile = fopen(filename, "wb");
        if (!outFile) {
            snprintf(errorMsg, msgSize, "Failed to create file: %s", filename);
            mz_free(extracted_data);
            success = false;
            break;
        }

        fwrite(extracted_data, 1, extracted_size, outFile);
        fclose(outFile);
        mz_free(extracted_data);

        if (extracted_size == 0) {
            snprintf(errorMsg, msgSize, "File %s extracted with zero data", filename);
            success = false;
            break;
        }
    }

    // Clean up
    mz_zip_reader_end(&zip_archive);

    if (success) {
        snprintf(errorMsg, msgSize, "Successfully extracted %d files from %s", fileCount, zipFile);
    }

    return success;
}

int main(void) {
    // Initialize window
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "raylib raygui - Unzip test.zip Example");
    SetTargetFPS(60);

    // Define button rectangle
    Rectangle buttonRect = { screenWidth/2.0f - 100, screenHeight/2.0f - 30, 200, 60 };
    bool showMessage = false;
    char message[MAX_MESSAGE_SIZE] = { 0 };

    while (!WindowShouldClose()) {
        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw GUI button
        if (GuiButton(buttonRect, "Unzip test.zip")) {
            bool success = ExtractZipFile("test.zip", message, MAX_MESSAGE_SIZE);
            showMessage = true;
        }

        // Display result message
        if (showMessage) {
            DrawText(message, screenWidth/2 - MeasureText(message, 20)/2, screenHeight/2 + 80, 20, DARKGRAY);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}