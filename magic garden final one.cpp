#include "raylib.h"
#include <math.h>
#include <stdio.h>

#define MAX_PARTICLES 150
#define MAX_PETALS 60

typedef enum { SEED, SPROUT, BUD, BLOOM, FULL } GrowthStage;

static const char* poeticLines[] = {
    "In the silence of the earth, a dream begins...",
    "A touch of green, reaching for the morning light.",
    "Patience whispers, as colors prepare to dance.",
    "The soul unfolds, painting the world with grace.",
    "A symphony of petals, basking in eternal glow."
};

typedef struct Particle { Vector2 pos; Vector2 speed; float alpha; Color color; bool active; } Particle;
typedef struct Petal { Vector2 pos; Vector2 speed; float alpha; bool active; } Petal;
typedef struct Flower { Vector2 pos; Color petalColor; GrowthStage stage; float growthValue; float timer; const char* name; } Flower;

static Particle particles[MAX_PARTICLES];
static Petal petals[MAX_PETALS];

static void CreateSparkle(Vector2 pos, Color color) {
    for (int i = 0, count = 0; i < MAX_PARTICLES && count < 25; i++) {
        if (!particles[i].active) {
            particles[i].active = true;
            particles[i].pos = pos;
            particles[i].speed = (Vector2){ (float)GetRandomValue(-100, 100) / 20.0f, (float)GetRandomValue(-100, 100) / 20.0f };
            particles[i].alpha = 1.0f;
            particles[i].color = color;
            count++;
        }
    }
}

int main(void) {
    SetConfigFlags(FLAG_WINDOW_UNDECORATED); 
    InitWindow(800, 600, "Magic Garden Presentation"); 
    
    int screenW = GetMonitorWidth(GetCurrentMonitor());
    int screenH = GetMonitorHeight(GetCurrentMonitor());
    
    SetWindowSize(screenW, screenH);
    SetWindowPosition(0, 0); 
    SetTargetFPS(60);

    InitAudioDevice();
    Music music = LoadMusicStream("music.mp3"); 
    PlayMusicStream(music);

    Flower garden[3] = {
        { {(float)screenW * 0.25f, (float)screenH * 0.82f}, VIOLET, SEED, 0.1f, 0.0f, "Luna"  },
        { {(float)screenW * 0.50f, (float)screenH * 0.82f}, SKYBLUE, SEED, 0.1f, 0.0f, "Azure" },
        { {(float)screenW * 0.75f, (float)screenH * 0.82f}, RED,     SEED, 0.1f, 0.0f, "Ember" }
    };

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        UpdateMusicStream(music);
        if (IsKeyPressed(KEY_ESCAPE)) break;

        float skyLerp = (sinf(GetTime() * 0.15f) + 1.0f) * 0.5f; 
        Color skyTop = ColorLerp((Color){15, 15, 40, 255}, (Color){110, 170, 255, 255}, skyLerp);
        Color skyBot = ColorLerp((Color){45, 40, 85, 255}, (Color){255, 220, 170, 255}, skyLerp);

        if (GetRandomValue(0, 100) < 7) {
            for (int i = 0; i < MAX_PETALS; i++) {
                if (!petals[i].active) {
                    petals[i] = { {(float)GetRandomValue(0, screenW), -20}, 
                                  {(float)GetRandomValue(-5, 5)/12.0f, (float)GetRandomValue(12, 25)/10.0f}, 0.8f, true };
                    break;
                }
            }
        }

        float totalProgress = 0;
        for (int i = 0; i < 3; i++) {
            if (garden[i].stage < FULL) {
                garden[i].timer += dt;
                garden[i].growthValue += dt * 0.04f; 
                if (garden[i].timer >= 6.0f) {
                    garden[i].timer = 0.0f;
                    garden[i].stage = (GrowthStage)(garden[i].stage + 1);
                    CreateSparkle((Vector2){ garden[i].pos.x, garden[i].pos.y - (220.0f * garden[i].growthValue) }, garden[i].petalColor);
                }
            }
            totalProgress += (float)garden[i].stage / FULL;
        }
        float overallAvg = (totalProgress / 3.0f) * 100.0f;

        const char* currentStatus = (overallAvg < 100) ? "The garden is growing together..." : "The garden is in full eternal glory! ✨";

        BeginDrawing();
            DrawRectangleGradientV(0, 0, screenW, screenH, skyTop, skyBot);

            if (overallAvg > 60.0f) {
                float rbAlpha = (overallAvg - 60.0f) / 40.0f; 
                Color rbColors[] = {RED, ORANGE, YELLOW, GREEN, BLUE, (Color){75, 0, 130, 255}, VIOLET};
                Vector2 center = {(float)screenW / 2, (float)screenH * 0.82f};
                for (int i = 0; i < 7; i++) {
                    DrawRing(center, (screenW * 0.35f) + (i * 12), (screenW * 0.35f) + (i * 12) + 10, 180, 360, 40, Fade(rbColors[i], rbAlpha * 0.25f));
                }
            }

            DrawRectangle(0, (int)(screenH * 0.82f), screenW, (int)(screenH * 0.18f), (Color){20, 45, 20, 255}); 

            DrawText("WELCOME TO OUR MAGIC GARDEN", (screenW/2) - (MeasureText("WELCOME TO OUR MAGIC GARDEN", 40)/2), 40, 40, GOLD);

            for (int i = 0; i < 3; i++) {
                float sway = sinf(GetTime() * 1.2f + garden[i].pos.x) * (garden[i].growthValue * 15.0f);
                Vector2 base = { garden[i].pos.x + sway, garden[i].pos.y };
                float headY = base.y - (220.0f * garden[i].growthValue);
                if (garden[i].stage >= SPROUT) {
                    DrawLineEx(base, {base.x, headY}, 7.0f, DARKGREEN);
                    DrawEllipse((int)base.x - 20, (int)(base.y - (80.0f * garden[i].growthValue)), 22, 8, LIME);
                    DrawEllipse((int)base.x + 20, (int)(base.y - (80.0f * garden[i].growthValue)), 22, 8, LIME);
                }
                if (garden[i].stage >= BLOOM) {
                    float sz = 45.0f * garden[i].growthValue;
                    for (int j = 0; j < 6; j++) DrawCircleV({base.x + cosf(j*60*DEG2RAD)*sz*0.7f, headY + sinf(j*60*DEG2RAD)*sz*0.7f}, sz*0.6f, garden[i].petalColor);
                    DrawCircleV({base.x, headY}, sz*0.4f, YELLOW);
                }
                DrawText(garden[i].name, (int)garden[i].pos.x - 20, (int)garden[i].pos.y + 25, 22, RAYWHITE);
            }

            for (int i = 0; i < MAX_PETALS; i++) if (petals[i].active) {
                petals[i].pos.y += petals[i].speed.y;
                DrawCircleV(petals[i].pos, 4, Fade(PINK, petals[i].alpha));
                if (petals[i].pos.y > screenH) petals[i].active = false;
            }
            for (int i = 0; i < MAX_PARTICLES; i++) if (particles[i].active) {
                particles[i].pos.x += particles[i].speed.x; particles[i].pos.y += particles[i].speed.y;
                particles[i].alpha -= 0.012f;
                DrawCircleV(particles[i].pos, 2.5f, Fade(particles[i].color, particles[i].alpha));
                if (particles[i].alpha <= 0) particles[i].active = false;
            }

            float boxW = 650.0f;
            float boxX = (screenW - boxW) / 2.0f;
            float boxY = (float)screenH - 85.0f;
            Rectangle statusBox = { boxX, boxY, boxW, 60.0f };
            DrawRectangleRounded(statusBox, 0.4f, 10, Fade(BLACK, 0.7f));
            DrawRectangleRoundedLines(statusBox, 0.4f, 10, Fade(RAYWHITE, 0.4f)); 
            DrawText(currentStatus, (int)(screenW/2 - MeasureText(currentStatus, 24)/2), (int)(boxY + 18), 24, (overallAvg >= 100) ? GOLD : RAYWHITE);

            int poeticIdx = (int)(overallAvg / 21.0f);
            if (poeticIdx > 4) poeticIdx = 4;
            DrawText(poeticLines[poeticIdx], (int)(screenW/2 - MeasureText(poeticLines[poeticIdx], 30)/2), (int)(screenH * 0.18f), 30, WHITE);

        EndDrawing();
    }
    CloseAudioDevice(); CloseWindow();
    return 0;
}