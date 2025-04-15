#include "raylib.h"
#include <stdlib.h>
#include <string.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#undef RAYGUI_IMPLEMENTATION            // Avoid including raygui implementation again

#define MAX_FILEPATH_SIZE 2048

#include "bsdiff.h"

static void log_error(void *opaque, const char *errmsg){
	(void)opaque;
	fprintf(stderr, "%s", errmsg);
}

int generate_patch(const char *oldname, const char *newname, const char *patchname){
	int ret = 1;
	struct bsdiff_stream oldfile = { 0 }, newfile = { 0 }, patchfile = { 0 };
	struct bsdiff_ctx ctx = { 0 };
	struct bsdiff_patch_packer packer = { 0 };

	ret = bsdiff_open_file_stream(BSDIFF_MODE_READ, oldname, &oldfile);
	if (ret != BSDIFF_SUCCESS) {
		fprintf(stderr, "can't open oldfile: %s\n", oldname);
		goto cleanup;
	}
	ret = bsdiff_open_file_stream(BSDIFF_MODE_READ, newname, &newfile);
	if (ret != BSDIFF_SUCCESS) {
		fprintf(stderr, "can't open newfile: %s\n", newname);
		goto cleanup;
	}
	ret = bsdiff_open_file_stream(BSDIFF_MODE_WRITE, patchname, &patchfile);
	if (ret != BSDIFF_SUCCESS) {
		fprintf(stderr, "can't open patchfile: %s\n", patchname);
		goto cleanup;
	}
	ret = bsdiff_open_bz2_patch_packer(BSDIFF_MODE_WRITE, &patchfile, &packer);
	if (ret != BSDIFF_SUCCESS) {
		fprintf(stderr, "can't create BZ2 patch packer\n");
		goto cleanup;
	}

	ctx.log_error = log_error;

	ret = bsdiff(&ctx, &oldfile, &newfile, &packer);
	if (ret != BSDIFF_SUCCESS) {
		fprintf(stderr, "bsdiff failed: %d\n", ret);
		goto cleanup;
	}

cleanup:
	bsdiff_close_patch_packer(&packer);
	bsdiff_close_stream(&patchfile);
	bsdiff_close_stream(&newfile);
	bsdiff_close_stream(&oldfile);

	return ret;
}

int apply_patch(const char *oldname, const char *newname, const char *patchname){
	int ret = 1;
	struct bsdiff_stream oldfile = { 0 }, newfile = { 0 }, patchfile = { 0 };
	struct bsdiff_ctx ctx = { 0 };
	struct bsdiff_patch_packer packer = { 0 };

	ret = bsdiff_open_file_stream(BSDIFF_MODE_READ, oldname, &oldfile);
	if (ret != BSDIFF_SUCCESS) {
		fprintf(stderr, "can't open oldfile: %s\n", oldname);
		goto cleanup;
	}
	ret = bsdiff_open_file_stream(BSDIFF_MODE_WRITE, newname, &newfile);
	if (ret != BSDIFF_SUCCESS) {
		fprintf(stderr, "can't open newfile: %s\n", newname);
		goto cleanup;
	}
	ret = bsdiff_open_file_stream(BSDIFF_MODE_READ, patchname, &patchfile);
	if (ret != BSDIFF_SUCCESS) {
		fprintf(stderr, "can't open patchfile: %s\n", patchname);
		goto cleanup;
	}
	ret = bsdiff_open_bz2_patch_packer(BSDIFF_MODE_READ, &patchfile, &packer);
	if (ret != BSDIFF_SUCCESS) {
		fprintf(stderr, "can't create BZ2 patch packer\n");
		goto cleanup;
	}

	ctx.log_error = log_error;
	
	ret = bspatch(&ctx, &oldfile, &newfile, &packer);
	if (ret != BSDIFF_SUCCESS) {
		fprintf(stderr, "bspatch failed: %d\n", ret);
		goto cleanup;
	}

cleanup:
	bsdiff_close_patch_packer(&packer);
	bsdiff_close_stream(&patchfile);
	bsdiff_close_stream(&newfile);
	bsdiff_close_stream(&oldfile);

	return ret;
}


int main(void)
{
    // Initialization
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, " Diff and Patcher ");

    char tool_mode[10] = "diff"; // Assign string "test"
    char status[256] = "None";

    char hash01[300] = "None";
    char hash02[300] = "None";
    char hash03[300] = "None";

    // Define the drop box area
    Rectangle dropBox01 = { 312, 72, 336, 24 };
    Rectangle dropBox02 = { 312, 168, 336, 24 };
    Rectangle dropBox03 = { 312, 264, 336, 24 };

    bool isHovering01 = false;
    bool isHovering02 = false;
    bool isHovering03 = false;
    char filePath01[MAX_FILEPATH_SIZE] = { 0 };
    char filePath02[MAX_FILEPATH_SIZE] = { 0 };
    char filePath03[MAX_FILEPATH_SIZE] = { 0 };
    bool hasFile01 = false;
    bool hasFile02 = false;
    bool hasFile03 = false;

    SetTargetFPS(60);

    // Main game loop
    while (!WindowShouldClose())
    {
        // Update
        Vector2 mousePos = GetMousePosition();
        isHovering01 = CheckCollisionPointRec(mousePos, dropBox01);
        isHovering02 = CheckCollisionPointRec(mousePos, dropBox02);
        isHovering03 = CheckCollisionPointRec(mousePos, dropBox03);

        if (IsFileDropped() && isHovering01 && !hasFile01)
        {
            FilePathList droppedFiles = LoadDroppedFiles();
            if (droppedFiles.count > 0)
            {
                strncpy(filePath01, droppedFiles.paths[0], MAX_FILEPATH_SIZE - 1);
                filePath01[MAX_FILEPATH_SIZE - 1] = '\0';
                hasFile01 = true;
            }
            UnloadDroppedFiles(droppedFiles);
        }

        if (IsFileDropped() && isHovering02 && !hasFile02)
        {
            FilePathList droppedFiles = LoadDroppedFiles();
            if (droppedFiles.count > 0)
            {
                strncpy(filePath02, droppedFiles.paths[0], MAX_FILEPATH_SIZE - 1);
                filePath02[MAX_FILEPATH_SIZE - 1] = '\0';
                hasFile02 = true;
            }
            UnloadDroppedFiles(droppedFiles);
        }

        if (IsFileDropped() && isHovering03 && !hasFile03)
        {
            FilePathList droppedFiles = LoadDroppedFiles();
            if (droppedFiles.count > 0)
            {
                strncpy(filePath03, droppedFiles.paths[0], MAX_FILEPATH_SIZE - 1);
                filePath03[MAX_FILEPATH_SIZE - 1] = '\0';
                hasFile03 = true;
            }
            UnloadDroppedFiles(droppedFiles);
        }

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (GuiButton((Rectangle){ 24, 24, 120, 24 }, GuiIconText(ICON_FILE_OPEN, "Diff"))){
          strcpy(tool_mode, "diff");
        };
        if (GuiButton((Rectangle){ 168, 24, 120, 24 }, GuiIconText(ICON_FILE_OPEN, "Patch"))){
          strcpy(tool_mode, "patch");
        };
        if (GuiButton((Rectangle){ 312, 24, 120, 24 }, GuiIconText(ICON_FILE_OPEN, "Hash"))){
          strcpy(tool_mode, "hash");
        };
        GuiLabel((Rectangle){ 456, 24, 48, 24 }, "Current:");
        //GuiLabel((Rectangle){ 504, 24, 120, 24 }, "Diff");
        GuiLabel((Rectangle){ 504, 24, 120, 24 }, tool_mode);

        //CURRENT FILE
        GuiLabel((Rectangle){ 24, 72, 120, 24 }, "Current File");
        if (GuiButton((Rectangle){ 168, 72, 120, 24 }, GuiIconText(ICON_FILE_OPEN, "Open File")));
        if (GuiButton((Rectangle){ 672, 72, 120, 24 }, GuiIconText(ICON_FILE_OPEN, "Reset"))){
          hasFile01=false;
        };
        GuiLabel((Rectangle){ 24, 96, 72, 24 }, "File Path:");
        GuiLabel((Rectangle){ 96, 96, 576, 24 }, filePath01);
        GuiLabel((Rectangle){ 24, 120, 552, 24 }, "Hash");
        GuiLabel((Rectangle){ 96, 120, 552, 24 }, hash01);
        //NEW FILE OR DIFF
        GuiLabel((Rectangle){ 24, 168, 120, 24 }, "New File");
        if (GuiButton((Rectangle){ 168, 168, 120, 24 }, GuiIconText(ICON_FILE_OPEN, "Open File")));
        if (GuiButton((Rectangle){ 672, 168, 120, 24 }, GuiIconText(ICON_FILE_OPEN, "Reset"))){
          hasFile02=false;
        };
        GuiLabelButton((Rectangle){ 24, 192, 72, 24 }, "File Path:");
        GuiLabel((Rectangle){ 96, 192, 576, 24 }, filePath02);
        GuiLabelButton((Rectangle){ 24, 216, 72, 24 }, "Hash");
        GuiLabel((Rectangle){ 96, 216, 552, 24 }, hash02);
        // PATCH
        GuiLabelButton((Rectangle){ 24, 264, 120, 24 }, "Patch");
        GuiButton((Rectangle){ 168, 264, 120, 24 }, "Open File");
        if (GuiButton((Rectangle){ 672, 264, 120, 24 }, GuiIconText(ICON_FILE_OPEN, "Reset"))){
          hasFile03=false;
        };
        GuiLabel((Rectangle){ 24, 288, 552, 24 }, "File Path");
        GuiLabel((Rectangle){ 96, 288, 576, 24 }, filePath03);
        GuiLabel((Rectangle){ 24, 312, 552, 24 }, "Hash");
        GuiLabel((Rectangle){ 96, 312, 552, 24 }, hash03);

        if (GuiButton((Rectangle){ 24, 360, 120, 24 }, GuiIconText(ICON_FOLDER_SAVE, "Apply"))){
          
          if (strcmp(tool_mode, "diff") == 0) {
            if (!hasFile01 || !hasFile02) {
                strncpy(status, "No File Path!", sizeof(status) - 1);
                status[sizeof(status) - 1] = '\0';
            } else if (!FileExists(filePath01) || !FileExists(filePath02)) {
                strncpy(status, "File does not exist!", sizeof(status) - 1);
                status[sizeof(status) - 1] = '\0';
            } else {
                const char *basename = GetFileNameWithoutExt(filePath01);
                if (!basename) {
                    strncpy(status, "Invalid filename!", sizeof(status) - 1);
                    status[sizeof(status) - 1] = '\0';
                } else {
                    char patchfile[MAX_FILEPATH_SIZE];
                    snprintf(patchfile, sizeof(patchfile), "%s.dff", basename);
                    int result = generate_patch(filePath01, filePath02, patchfile);
                    if (result == BSDIFF_SUCCESS) {
                        snprintf(status, sizeof(status), "Diff generated successfully! diff file: %s", patchfile);
                        // Optionally update filePath03 for UI
                        strncpy(filePath03, patchfile, MAX_FILEPATH_SIZE - 1);
                        filePath03[MAX_FILEPATH_SIZE - 1] = '\0';
                        hasFile03 = true;
                    } else {
                        strncpy(status, "Diff generation failed!", sizeof(status) - 1);
                        status[sizeof(status) - 1] = '\0';
                    }
                }
            }
          }


          if (strcmp(tool_mode, "patch") == 0) {
              if (!hasFile01 || !hasFile03) {
                  strncpy(status, "No File Path!", sizeof(status) - 1);
                  status[sizeof(status) - 1] = '\0';
              } else if (!FileExists(filePath01) || !FileExists(filePath03)) {
                  strncpy(status, "File does not exist!", sizeof(status) - 1);
                  status[sizeof(status) - 1] = '\0';
              } else {
                  // Get filename from filePath01
                  const char *filename = GetFileName(filePath01);
                  if (!filename) {
                      strncpy(status, "Invalid filename!", sizeof(status) - 1);
                      status[sizeof(status) - 1] = '\0';
                  } else {
                      // Create filePath02 with "new_" prefix in the same directory
                      char newfile[MAX_FILEPATH_SIZE];
                      snprintf(newfile, sizeof(newfile), "%s\\new_%s", GetDirectoryPath(filePath01), filename);
                      
                      // Apply patch
                      int result = apply_patch(filePath01, newfile, filePath03);
                      if (result == BSDIFF_SUCCESS) {
                          // Update filePath02 for UI display
                          strncpy(filePath02, newfile, MAX_FILEPATH_SIZE - 1);
                          filePath02[MAX_FILEPATH_SIZE - 1] = '\0';
                          hasFile02 = true;
                          snprintf(status, sizeof(status), "Patch applied successfully to %s!", filePath02);
                      } else {
                          strncpy(status, "Patch application failed!", sizeof(status) - 1);
                          status[sizeof(status) - 1] = '\0';
                      }
                  }
              }
          }

          if (strcmp(tool_mode, "hash") == 0) {
              strncpy(status, "Hash mode not implemented!", sizeof(status) - 1);
              status[sizeof(status) - 1] = '\0';
          }
        };
        GuiLabel((Rectangle){ 24, 384, 48, 24 }, "Status:");
        // GuiLabel((Rectangle){ 72, 384, 720, 24 }, "None");
        GuiLabel((Rectangle){ 72, 384, 720, 24 }, status);

        //===============================================================================
        //===============================================================================
        // Draw drop box 01
        Color boxColor01 = isHovering01 ? Fade(GREEN, 0.3f) : Fade(GRAY, 0.2f);
        DrawRectangleRec(dropBox01, boxColor01);
        DrawRectangleLinesEx(dropBox01, 2, isHovering01 ? GREEN : DARKGRAY);

        // Draw instructions or file info
        if (!hasFile01)
        {
            //DrawText("Drop File Here!", 312, 72, 12, DARKGRAY);
            GuiLabel((Rectangle){ 312+8, 72, 336, 24 }, "Drop File Here!");
            //GuiLabel((Rectangle){ 96, 96, 576, 24 }, filePath01);
        }
        else
        {
            //DrawText("Dropped file:", 312, 264, 12, DARKGRAY);
            GuiLabel((Rectangle){ 312+8, 72, 336, 24 }, "Drop File Here!");
            //DrawText(filePath01, dropBox01.x + 8, dropBox01.y + 20, 10, GRAY);
            //DrawText(filePath01, 96, 96, 10, DARKGRAY);
            // GuiLabel((Rectangle){ 96, 96, 576, 24 }, filePath01);
        }

        //===============================================================================
        //===============================================================================
        // Draw drop box 02
        Color boxColor02 = isHovering02 ? Fade(GREEN, 0.3f) : Fade(GRAY, 0.2f);
        DrawRectangleRec(dropBox02, boxColor02);
        DrawRectangleLinesEx(dropBox02, 2, isHovering02 ? GREEN : DARKGRAY);

        // Draw instructions or file info
        if (!hasFile02)
        {
            //DrawText("Drop File Here!", 312, 168, 12, DARKGRAY);
            //DrawText("Diff None", 78, 184, 10, DARKGRAY);
            GuiLabel((Rectangle){ 312+8, 168, 336, 24 }, "Drop File Here!");
            //GuiLabel((Rectangle){ 96, 192, 576, 24 }, filePath02);
        }
        else
        {
            // DrawText("Dropped file:", 312+8, 168, 12, DARKGRAY);
            GuiLabel((Rectangle){ 312+8, 168, 336, 24 }, "Drop File Here!");
            // DrawText(filePath02, 96, 192, 10, DARKGRAY);
            // GuiLabel((Rectangle){ 96, 192, 576, 24 }, filePath02);
        }

        //===============================================================================
        //===============================================================================
        // Draw drop box 03
        Color boxColor03 = isHovering03 ? Fade(GREEN, 0.3f) : Fade(GRAY, 0.2f);
        DrawRectangleRec(dropBox03, boxColor03);
        DrawRectangleLinesEx(dropBox03, 2, isHovering03 ? GREEN : DARKGRAY);

        // Draw instructions or file info
        if (!hasFile03)
        {
            // DrawText("Drop File Here!", 312, 264, 12, DARKGRAY);
            // DrawText("None", 78, 184, 10, DARKGRAY);
            GuiLabel((Rectangle){ 312+8, 264, 336, 24 }, "Drop File Here!");
            // GuiLabel((Rectangle){ 96, 288, 576, 24 }, filePath03);
        }
        else
        {
            // DrawText("Dropped file:", 312, 264, 12, DARKGRAY);
            //DrawText(filePath02, dropBox02.x + 8, dropBox02.y + 20, 10, GRAY);
            // DrawText(filePath03, 78, 184, 10, DARKGRAY);
            GuiLabel((Rectangle){ 312+8, 264, 336, 24 }, "Drop File Here!");
            // GuiLabel((Rectangle){ 96, 288, 576, 24 }, filePath03);
        }

        EndDrawing();
    }

    // De-Initialization
    CloseWindow();
    return 0;
}