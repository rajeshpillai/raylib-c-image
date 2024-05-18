
#include "include/raylib.h"
#include "include/raymath.h"
#include <math.h>
#include <stdlib.h>

typedef struct Particle {
    Vector2 position;
    Vector2 origin;
    Vector2 velocity;
    Color color;
    float size;
    float force;
    float angle;
    float distance;
    float friction;
    float ease;
} Particle;

typedef struct Effect {
    int width;
    int height;
    int mouseRadius;
    int gap;
    Vector2 mouse;
    int particleCount;
    Particle *particles;
} Effect;

void InitParticles(Effect *effect, Image image, int screenWidth, int screenHeight);
void UpdateParticles(Effect *effect);
void DrawParticles(Effect *effect);
//Vector2 Vector2Subtract(Vector2 v1, Vector2 v2);  // Use from raymath
//float Vector2LengthSqr(Vector2 v);

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Particle Animation");
    SetTargetFPS(60);

    // Load image
    Image image = LoadImage("images/mario.png"); // Use the converted PNG image

    Effect effect = {
        .width = screenWidth,
        .height = screenHeight,
        .mouseRadius = 500,
        .gap = 2,
        .mouse = {screenWidth / 2, screenHeight / 2},
        .particleCount = 0,
        .particles = NULL
    };

    InitParticles(&effect, image, screenWidth, screenHeight);

    while (!WindowShouldClose()) {
        effect.mouse = GetMousePosition();
        UpdateParticles(&effect);
        BeginDrawing();
        ClearBackground(BLACK);
        DrawParticles(&effect);
        EndDrawing();
    }

    UnloadImage(image);
    free(effect.particles);
    CloseWindow();
    
    return 0;
}

void InitParticles(Effect *effect, Image image, int screenWidth, int screenHeight) {
    int imageWidth = image.width;
    int imageHeight = image.height;
    
    // Calculate the aspect ratio
    float aspectRatio = (float)imageWidth / (float)imageHeight;
    
    // Calculate new dimensions to maintain aspect ratio
    int newWidth, newHeight;
    if (screenWidth / aspectRatio <= screenHeight) {
        newWidth = screenWidth;
        newHeight = (int)(screenWidth / aspectRatio);
    } else {
        newHeight = screenHeight;
        newWidth = (int)(screenHeight * aspectRatio);
    }
    
    // Resize the image to fit within the screen while maintaining aspect ratio
    ImageResize(&image, newWidth, newHeight);
    
    // Center the image
    int offsetX = (screenWidth - newWidth) / 2;
    int offsetY = (screenHeight - newHeight) / 2;

    Color *pixels = LoadImageColors(image);
    effect->particleCount = (newWidth / effect->gap) * (newHeight / effect->gap);
    effect->particles = (Particle *)malloc(effect->particleCount * sizeof(Particle));
    
    int index = 0;
    for (int y = 0; y < newHeight; y += effect->gap) {
        for (int x = 0; x < newWidth; x += effect->gap) {
            Color color = pixels[y * newWidth + x];
            if (color.a > 0) {
                effect->particles[index].position = (Vector2){x + offsetX, y + offsetY};
                effect->particles[index].origin = (Vector2){x + offsetX, y + offsetY};
                effect->particles[index].velocity = (Vector2){0, 0};
                effect->particles[index].color = color;
                effect->particles[index].size = 2.0f;
                effect->particles[index].force = 0;
                effect->particles[index].angle = 0;
                effect->particles[index].distance = 0;
                effect->particles[index].friction = 0.98f;
                effect->particles[index].ease = 0.2f;
                index++;
            }
        }
    }
    
    UnloadImageColors(pixels);
}

void UpdateParticles(Effect *effect) {
    for (int i = 0; i < effect->particleCount; i++) {
        Particle *p = &effect->particles[i];
        Vector2 dxdy = Vector2Subtract(effect->mouse, p->position);
        p->distance = Vector2LengthSqr(dxdy);
        if (p->distance > 0) {
            p->force = -effect->mouseRadius / p->distance;
            if (p->distance < effect->mouseRadius) {
                p->angle = atan2f(dxdy.y, dxdy.x);
                p->velocity.x += p->force * cosf(p->angle);
                p->velocity.y += p->force * sinf(p->angle);
            }
        }
        p->position.x += (p->velocity.x *= p->friction) + (p->origin.x - p->position.x) * p->ease;
        p->position.y += (p->velocity.y *= p->friction) + (p->origin.y - p->position.y) * p->ease;
    }
}

void DrawParticles(Effect *effect) {
    for (int i = 0; i < effect->particleCount; i++) {
        Particle *p = &effect->particles[i];
        DrawRectangle(p->position.x, p->position.y, p->size, p->size, p->color);
    }
}

// Vector2 Vector2Subtract(Vector2 v1, Vector2 v2) {
//     return (Vector2){v1.x - v2.x, v1.y - v2.y};
// }

// float Vector2LengthSqr(Vector2 v) {
//     return v.x * v.x + v.y * v.y;
// }