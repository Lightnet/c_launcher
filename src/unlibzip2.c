#define WIN32_LEAN_AND_MEAN 
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define NOGDI
#define NOUSER
#define MMNOSOUND

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <bzlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUFFER_SIZE 1024

// Function to decompress a .bz2 file to the current directory
bool DecompressBz2File(const char *inputFile, const char *outputFile, char *errorMsg, int msgSize) {
  FILE *inFile = fopen(inputFile, "rb");
  if (!inFile) {
      snprintf(errorMsg, msgSize, "Failed to open input file: %s", inputFile);
      return false;
  }

  // Check if file is empty
  fseek(inFile, 0, SEEK_END);
  long fileSize = ftell(inFile);
  fseek(inFile, 0, SEEK_SET);
  if (fileSize == 0) {
      snprintf(errorMsg, msgSize, "Input file %s is empty", inputFile);
      fclose(inFile);
      return false;
  }

  FILE *outFile = fopen(outputFile, "wb");
  if (!outFile) {
      snprintf(errorMsg, msgSize, "Failed to open output file: %s", outputFile);
      fclose(inFile);
      return false;
  }

  BZFILE *bzFile = BZ2_bzReadOpen(NULL, inFile, 0, 0, NULL, 0);
  if (!bzFile) {
      snprintf(errorMsg, msgSize, "Failed to initialize bzip2 decompression");
      fclose(inFile);
      fclose(outFile);
      return false;
  }

  char buffer[MAX_BUFFER_SIZE];
  int bytesRead;
  int totalBytesWritten = 0;
  while ((bytesRead = BZ2_bzRead(NULL, bzFile, buffer, MAX_BUFFER_SIZE)) > 0) {
      fwrite(buffer, 1, bytesRead, outFile);
      totalBytesWritten += bytesRead;
  }

  int bzError;
  BZ2_bzReadClose(&bzError, bzFile);
  fclose(inFile);
  fclose(outFile);

  if (bytesRead == -1 || bzError != BZ_OK) {
      snprintf(errorMsg, msgSize, "Error during decompression (code: %d)", bzError);
      return false;
  }

  if (totalBytesWritten == 0) {
      snprintf(errorMsg, msgSize, "No data decompressed from %s", inputFile);
      return false;
  }

  snprintf(errorMsg, msgSize, "File unzipped successfully!");
  return true;
}

int main(void) {
    // Initialize window
    const int screenWidth = 800;
    const int screenHeight = 450;
    InitWindow(screenWidth, screenHeight, "raylib raygui - Unzip bzip2 Example");
    SetTargetFPS(60);

    // Define button rectangle
    Rectangle buttonRect = { screenWidth/2.0f - 100, screenHeight/2.0f - 30, 200, 60 };
    bool showMessage = false;
    char message[256] = { 0 };

    while (!WindowShouldClose()) {
        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw GUI button
        if (GuiButton(buttonRect, "Unzip test.bz2")) {
          bool success = DecompressBz2File("test.zip", "test.txt", message, sizeof(message));
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