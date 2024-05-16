#include <inttypes.h>
#include <math.h>
#include <raylib.h>
#include <raymath.h>

#ifdef PLATFORM_WEB
#include <emscripten/emscripten.h>
#endif

#define TARGET_FPS 240
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define fmin3(a, b, c) fmin(a, fmin(b, c))
#define fmax3(a, b, c) fmax(a, fmax(b, c))

typedef struct {
  Vector2 p;
  float r;
} Circle;

float edgeFunction(Vector2 a, Vector2 b, Vector2 c) {
  return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

void updateDrawFrame(void) {
  static Vector2 av;
  static Vector2 bv;
  static Vector2 cv;
  static int width;
  static int height;
  static int dragging = 0;
  static bool first = true;
  static bool TNFull = false;

  if (first || (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_ZERO))) {
    av = (Vector2){.50, .15};
    bv = (Vector2){.85, .85};
    cv = (Vector2){.15, .85};
    first = false;
  }

#ifndef PLATFORM_WEB
  if (TNFull) {
    bool wasFull = IsWindowFullscreen();
    ToggleFullscreen();
    if (wasFull)
      SetWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    TNFull = false;
  }
  if (IsKeyPressed(KEY_F11)) {
    TNFull = true;
    SetWindowSize(1920, 1080);
  }
#endif

  width = GetScreenWidth();
  height = GetScreenHeight();
  static const float circleRad = 10;
  Vector2 a = {width * av.x, height * av.y};
  Vector2 b = {width * bv.x, height * bv.y};
  Vector2 c = {width * cv.x, height * cv.y};
  Circle ac = {a, circleRad};
  Circle bc = {b, circleRad};
  Circle cc = {c, circleRad};

  bool mouseDown = IsMouseButtonDown(0);
  Vector2 mousePos = GetMousePosition();
  if (!mouseDown)
    dragging = 0;
  if (mouseDown && dragging <= 0) {
    if (CheckCollisionPointCircle(mousePos, ac.p, ac.r))
      dragging = 1;
    else if (CheckCollisionPointCircle(mousePos, bc.p, bc.r))
      dragging = 2;
    else if (CheckCollisionPointCircle(mousePos, cc.p, cc.r))
      dragging = 3;
  }
  if (dragging == 1) {
    ac.p = mousePos;
    a = ac.p;
    av = (Vector2){ac.p.x / width, ac.p.y / height};
  } else if (dragging == 2) {
    bc.p = mousePos;
    b = bc.p;
    bv = (Vector2){bc.p.x / width, bc.p.y / height};
  } else if (dragging == 3) {
    cc.p = mousePos;
    c = cc.p;
    cv = (Vector2){cc.p.x / width, cc.p.y / height};
  }

  float ABC = edgeFunction(a, b, c);
  Vector4 bbox = {fmin3(a.x, b.x, c.x), fmin3(a.y, b.y, c.y),
                  fmax3(a.x, b.x, c.x), fmax3(a.y, b.y, c.y)};

  BeginDrawing();
  Vector2 p;
  ClearBackground(RAYWHITE);
  if (ABC >= 0) {
    for (p.y = bbox.y; p.y < bbox.w; ++p.y) {
      for (p.x = bbox.x; p.x < bbox.z; ++p.x) {
        Color color = {200, 200, 200, 255};
        float ABP = edgeFunction(a, b, p);
        float BCP = edgeFunction(b, c, p);
        float CAP = edgeFunction(c, a, p);
        if (ABP >= (float)0 && BCP >= (float)0 && CAP >= (float)0) {
          float wa = BCP / ABC;
          float wb = CAP / ABC;
          float wc = ABP / ABC;
          color.r = 255 * wa;
          color.g = 255 * wb;
          color.b = 255 * wc;
        }
        DrawPixelV(p, color);
      }
    }
  }
  DrawCircleV(ac.p, ac.r, BLACK);
  DrawCircleV(bc.p, bc.r, BLACK);
  DrawCircleV(cc.p, cc.r, BLACK);
  DrawFPS(10, 10);
  EndDrawing();
}

int main(void) {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "wasd tester");

#ifdef PLATFORM_WEB
  emscripten_set_main_loop(updateDrawFrame, TARGET_FPS, 1);
#else
  SetTargetFPS(TARGET_FPS);
  while (!WindowShouldClose()) {
    updateDrawFrame();
  }
#endif

  CloseWindow();

  return 0;
}
