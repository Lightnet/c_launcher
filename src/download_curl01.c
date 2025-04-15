#define WIN32_LEAN_AND_MEAN 
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define NOGDI
#define NOUSER
#define MMNOSOUND

#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    float progress; // 0.0f to 1.0f
    CURLM *multi_handle;
    CURL *easy_handle;
    int still_running;
    FILE *fp; // File to save download
} AppData;

size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata) {
    AppData *app = (AppData *)userdata;
    return fwrite(ptr, size, nmemb, app->fp); // Write to file
}

int progress_callback(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
    AppData *app = (AppData *)clientp;
    if (dltotal > 0) {
        app->progress = (float)dlnow / dltotal;
    }
    return 0;
}

void init_download(AppData *app, const char *url, const char *filename) {
    app->fp = fopen(filename, "wb");
    if (!app->fp) return;

    app->easy_handle = curl_easy_init();
    if (!app->easy_handle) {
        fclose(app->fp);
        app->fp = NULL;
        return;
    }

    curl_easy_setopt(app->easy_handle, CURLOPT_URL, url);
    curl_easy_setopt(app->easy_handle, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(app->easy_handle, CURLOPT_WRITEDATA, app);
    curl_easy_setopt(app->easy_handle, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(app->easy_handle, CURLOPT_XFERINFOFUNCTION, progress_callback);
    curl_easy_setopt(app->easy_handle, CURLOPT_XFERINFODATA, app);
    curl_easy_setopt(app->easy_handle, CURLOPT_FOLLOWLOCATION, 1L); // Handle redirects

    curl_multi_add_handle(app->multi_handle, app->easy_handle);
    app->still_running = 1;
}

void cleanup_download(AppData *app) {
    if (app->easy_handle) {
        curl_multi_remove_handle(app->multi_handle, app->easy_handle);
        curl_easy_cleanup(app->easy_handle);
        app->easy_handle = NULL;
    }
    if (app->fp) {
        fclose(app->fp);
        app->fp = NULL;
    }
    app->progress = 0.0f;
    app->still_running = 0;
}

int main(void) {
    // Initialize Raylib
    InitWindow(400, 200, "Launcher");
    SetTargetFPS(60);

    // Initialize Raygui
    GuiSetStyle(DEFAULT, TEXT_SIZE, 20);

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    AppData app = {0};
    app.multi_handle = curl_multi_init();
    if (!app.multi_handle) {
        CloseWindow();
        return 1;
    }
    app.progress = 0.0f;

    bool download_triggered = false;

    // Main loop
    while (!WindowShouldClose()) {
        // Update
        if (app.still_running) {
            CURLMcode result = curl_multi_perform(app.multi_handle, &app.still_running);
            if (result != CURLM_OK) {
                cleanup_download(&app);
            }
            if (!app.still_running) {
                cleanup_download(&app);
            }
        }

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // GUI
        if (GuiButton((Rectangle){50, 50, 100, 40}, "Download") && !app.still_running) {
            init_download(&app, "https://github.com/raysan5/raylib/releases/download/5.5/raylib-5.5_win32_msvc16.zip", "raylib-5.5_win32_msvc16.zip");
            download_triggered = true;
        }
        GuiProgressBar((Rectangle){50, 100, 300, 20}, NULL, NULL, &app.progress, 0.0f, 1.0f);

        if (download_triggered && !app.still_running) {
            DrawText("Download Complete!", 50, 140, 20, DARKGRAY);
        }

        EndDrawing();
    }

    // Cleanup
    if (app.still_running) cleanup_download(&app);
    if (app.multi_handle) curl_multi_cleanup(app.multi_handle);
    curl_global_cleanup();
    CloseWindow();

    return 0;
}