/****************************************************************
   Music Garden – raylib (FINAL CLEAN VERSION)
   Language: C
   No Beep, No windows.h, No Sound
****************************************************************/

#include "raylib.h"
#include <math.h>

#define SCREEN_WIDTH  900
#define SCREEN_HEIGHT 500

typedef enum {
    SEED = 0,
    SPROUT,
    BUD,
    BLOOM,
    FULL
} GrowthStage;

typedef struct {
    Vector2 pos;
    Color petal;
    GrowthStage stage;
    float timer;
} Flower;

// ---------------- DRAW FLOWER ----------------
void DrawFlower(Flower *f)
{
    // Stem
    if (f->stage >= SPROUT)
        DrawRectangle(f->pos.x - 3, f->pos.y, 6, 80, GREEN);

    // Leaves
    if (f->stage >= BLOOM) {
        DrawCircle(f->pos.x - 12, f->pos.y + 40, 10, DARKGREEN);
        DrawCircle(f->pos.x + 12, f->pos.y + 40, 10, DARKGREEN);
    }

    // Bud
    if (f->stage >= BUD)
        DrawCircle(f->pos.x, f->pos.y - 10, 10, f->petal);

    // Bloom
    if (f->stage >= BLOOM) {
        for (int i = 0; i < 6; i++) {
            float a = i * PI / 3;
            DrawCircle(
                f->pos.x + cosf(a) * 18,
                f->pos.y - 10 + sinf(a) * 18,
                12,
                f->petal
            );
        }
        DrawCircle(f->pos.x, f->pos.y - 10, 8, YELLOW);
    }

    // Glow
    if (f->stage == FULL)
        DrawCircleLines(f->pos.x, f->pos.y - 10, 35, GOLD);
}

// ---------------- DRAW RAINBOW ----------------
void DrawRainbow(void)
{
    Color colors[7] = { VIOLET, BLUE, SKYBLUE, GREEN, YELLOW, ORANGE, RED };

    for (int band = 0; band < 7; band++) {
        for (float a = PI; a <= 2 * PI; a += 0.01f) {
            float r = 220 - band * 8;
            float x = SCREEN_WIDTH / 2 + cosf(a) * r;
            float y = SCREEN_HEIGHT / 2 + sinf(a) * r - 80;
            DrawCircle(x, y, 3, colors[band]);
        }
    }
}

// ---------------- MAIN ----------------
int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Music Garden - raylib (C)");
    SetTargetFPS(60);

    Flower flowers[3] = {
        { {250, 300}, VIOLET,  SEED, 0 },
        { {450, 300}, SKYBLUE, SEED, 0 },
        { {650, 300}, RED,     SEED, 0 }
    };

    bool showRainbow = false;

    while (!WindowShouldClose()) {

        float dt = GetFrameTime();

        // Growth logic
        for (int i = 0; i < 3; i++) {
            flowers[i].timer += dt;

            if (flowers[i].timer > 1.2f && flowers[i].stage < FULL) {
                flowers[i].stage =(GrowthStage) (flowers[i].stage + 1);
                flowers[i].timer = 0;
            }

            if (flowers[i].stage == FULL)
                showRainbow = true;
        }

        BeginDrawing();
        ClearBackground(SKYBLUE);

        // Ground
        DrawRectangle(0, 350, SCREEN_WIDTH, 200, DARKGREEN);

        DrawText("Music Garden (raylib - C)", 270, 20, 30, DARKBLUE);
        DrawText("Flowers grow peacefully 🌸", 320, 60, 20, GRAY);

        for (int i = 0; i < 3; i++)
            DrawFlower(&flowers[i]);

        if (showRainbow) {
            DrawRainbow();
            DrawText("Rainbow appears 🌈", 360, 460, 20, MAROON);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}