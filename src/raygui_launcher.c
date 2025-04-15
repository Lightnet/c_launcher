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

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    // Initialization
    //---------------------------------------------------------------------------------------
    int screenWidth = 800;
    int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "layout_name");

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
    char status[256] = "None...";

    char textContent[2048] = "Test Context\n Test Me  Context\n Context\n Context\n Context\n Context\n Context\n Context\n Context\n Context\n Context\n Context\n Context\n Context\n Context\n";


    // layout_name: controls initialization
    //----------------------------------------------------------------------------------
    Rectangle ScrollPanel000ScrollView = { 0, 0, 0, 0 };
    Rectangle ScrollPanel000ScrollOffset = { 0, 0, 340, 340 };
    Rectangle ScrollPanel000BoundsOffset = { 0 };

    Rectangle panelRec = { 24, 24, 624, 312 };
    Rectangle panelContentRec = {0, 0, 624, 312 };
    Vector2 panelScroll = { 99, -20 };
    Rectangle panelView = { 0 };


    bool Button003Pressed = false;
    bool Button004Pressed = false;
    bool Button005Pressed = false;
    bool Button006Pressed = false;
    float ProgressBar007Value = 0.0f;
    //----------------------------------------------------------------------------------

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    float fontSize = 20.0f; // Adjust as needed
    Vector2 textSize = MeasureTextEx(GetFontDefault(), textContent, fontSize, 1.0f);
    panelContentRec = (Rectangle){0, 0, textSize.x, textSize.y}; // Set content size based on text




    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
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

            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR))); 

            // raygui: controls drawing
            //----------------------------------------------------------------------------------
            GuiScrollPanel(
              // (Rectangle){ 24, 24, 624 - ScrollPanel000BoundsOffset.x, 312 - ScrollPanel000BoundsOffset.y}, 
              panelRec,
              // textContent,
              NULL,
              //(Rectangle){ 24, 24, 624, 312 },
              panelContentRec,
              // &ScrollPanel000ScrollOffset, 
              &panelScroll,
              // &ScrollPanel000ScrollView
              &panelView
            );

            BeginScissorMode(panelView.x, panelView.y, panelView.width, panelView.height);
              //draw grid to see srcoll.
              GuiGrid((Rectangle){panelRec.x + panelScroll.x, panelRec.y + panelScroll.y, panelContentRec.width, panelContentRec.height}, NULL, 16, 3, NULL);
              // draw context text here? textContent
              // Draw text content, offset by panelScroll
              DrawTextEx(
                GetFontDefault(),
                textContent,
                (Vector2){panelRec.x + panelScroll.x, panelRec.y + panelScroll.y},
                fontSize,
                1.0f,
                BLACK
              );
            EndScissorMode();


            // Button005Pressed = GuiButton((Rectangle){ 672, 312, 120, 24 }, "Check"); 
            // Button006Pressed = GuiButton((Rectangle){ 672, 264, 120, 24 }, "Update"); 
            //GuiDisable();
            Button004Pressed = GuiButton((Rectangle){ 672, 360, 120, 24 }, "Start"); 
            //GuiEnable();
            
            if(GuiButton((Rectangle){ 672, 384, 120, 24 }, "Download") && !app.still_running){
              init_download(&app, "http://127.0.0.1:3000/", "raylib-5.5_win32_msvc16.zip");
              download_triggered = true;
              strcpy(status, "Downloading ...");
            }; 

            Button003Pressed = GuiButton((Rectangle){ 672, 408, 120, 24 }, "Options"); 

            
            // GuiProgressBar((Rectangle){ 24, 360, 624, 24 }, NULL, NULL, &ProgressBar007Value, 0, 1);
            GuiProgressBar((Rectangle){ 24, 360, 624, 24 }, NULL, NULL, &app.progress, 0, 1);
            GuiLabel((Rectangle){ 24, 408, 72, 24 }, "Version:");
            GuiLabel((Rectangle){ 120, 408, 120, 24 }, "0.0.0");
            GuiLabel((Rectangle){ 24, 384, 48, 24 }, "Status:");
            GuiLabel((Rectangle){ 72, 384, 576, 24 }, status);

            if (download_triggered && !app.still_running) {
              strcpy(status, "Download Complete!");
            }
            //----------------------------------------------------------------------------------

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//------------------------------------------------------------------------------------
// Controls Functions Definitions (local)
//------------------------------------------------------------------------------------

