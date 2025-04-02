#define WIN32_LEAN_AND_MEAN 
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define NOGDI
#define NOUSER
#define MMNOSOUND

#include <stdio.h>
#include <stdlib.h>

#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

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

void gen_diff(){
  int result = generate_patch("old.txt", "new.txt", "txt.diff");
  printf("bsdiff returned: %d\n", result);
  if (result == 0) {
      printf("Diff generated successfully! diff file: patch.diff\n");
  } else {
      printf("Diff generation failed: %d\n", result);
  }
}

void gen_patch(){
  int result = apply_patch("old.txt", "new.txt", "txt.diff");
  printf("bsdiff returned: %d\n", result);
  if (result == 0) {
      printf("Diff generated successfully! Patch file: patch.diff\n");
  } else {
      printf("Diff generation failed: %d\n", result);
  }
}


const int screenWidth = 800;
const int screenHeight = 450;

int main() {
  printf("init raylib\n");

  InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
  SetTargetFPS(60);

  while (!WindowShouldClose())    // Detect window close button or ESC key
  {
    BeginDrawing();

    ClearBackground(RAYWHITE);

    DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

    if (GuiButton((Rectangle){ 24, 24, 120, 30 }, "#191#Diff")){
      gen_diff();
    }

    if (GuiButton((Rectangle){ 24, 60, 120, 30 }, "#191#Patch")){
      gen_patch();
    }

    EndDrawing();
  }

  CloseWindow(); 

  return 0;
}