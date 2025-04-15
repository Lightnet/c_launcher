/*******************************************************************************************
*
*   LayoutName v1.0.0 - Tool Description
*
*   LICENSE: Propietary License
*
*   Copyright (c) 2022 raylib technologies. All Rights Reserved.
*
*   Unauthorized copying of this file, via any medium is strictly prohibited
*   This project is proprietary and confidential unless the owner allows
*   usage in any other form by expresely written permission.
*
**********************************************************************************************/

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

//----------------------------------------------------------------------------------
// Controls Functions Declaration
//----------------------------------------------------------------------------------


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

    // layout_name: controls initialization
    //----------------------------------------------------------------------------------
    Rectangle ScrollPanel000ScrollView = { 0, 0, 0, 0 };
    Vector2 ScrollPanel000ScrollOffset = { 0, 0 };
    Vector2 ScrollPanel000BoundsOffset = { 0, 0 };
    bool Button003Pressed = false;
    bool Button004Pressed = false;
    bool Button005Pressed = false;
    bool Button006Pressed = false;
    float ProgressBar007Value = 0.0f;
    //----------------------------------------------------------------------------------

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Implement required update logic
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR))); 

            // raygui: controls drawing
            //----------------------------------------------------------------------------------
            GuiScrollPanel((Rectangle){ 24, 24, 624 - ScrollPanel000BoundsOffset.x, 312 - ScrollPanel000BoundsOffset.y}, NULL, (Rectangle){ 24, 24, 624, 312 }, &ScrollPanel000ScrollOffset, &ScrollPanel000ScrollView);
            GuiLabel((Rectangle){ 72, 384, 576, 24 }, "None");
            Button003Pressed = GuiButton((Rectangle){ 672, 408, 120, 24 }, "Options"); 
            GuiDisable();
            Button004Pressed = GuiButton((Rectangle){ 672, 360, 120, 24 }, "Start"); 
            GuiEnable();

            Button005Pressed = GuiButton((Rectangle){ 672, 312, 120, 24 }, "Check"); 
            Button006Pressed = GuiButton((Rectangle){ 672, 264, 120, 24 }, "Update"); 
            GuiProgressBar((Rectangle){ 24, 360, 624, 24 }, NULL, NULL, &ProgressBar007Value, 0, 1);
            GuiLabel((Rectangle){ 24, 408, 72, 24 }, "Version:");
            GuiLabel((Rectangle){ 120, 408, 120, 24 }, "0.0.0");
            GuiLabel((Rectangle){ 24, 384, 48, 24 }, "Status:");
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

