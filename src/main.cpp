#ifdef DEBUG
#include "spritesheet-renderer.h"
#include <filesystem>
#endif

#include <chrono>
#include <raylib.h>

const double MS_PER_UPDATE = 50.0; // the game's effective tickrate

const int screenWidth  = 400;
const int screenHeight = 300;

int main()
{
  InitWindow(screenWidth, screenHeight, "platformer");
  SetTargetFPS(30);

#ifdef DEBUG
  Texture2D texCharIdle =
      LoadTexture(std::filesystem::path("../assets/charIdle.png").c_str());
  Texture2D texCharWalk =
      LoadTexture(std::filesystem::path("../assets/charWalk.png").c_str());

  std::unordered_map<std::string, SpritesheetImage const *> charSprites = {
      {"idle", new SpritesheetImage(texCharIdle, "idle", 1, 10)},
      {"walk", new SpritesheetImage(texCharWalk, "walk", 6, 4)}};

  SpritesheetRenderer characterSprite(charSprites, 16, "idle");
  characterSprite.switchTo("walk");
#endif

  auto   previous = std::chrono::high_resolution_clock::now();
  double lag      = 0;

  while (!WindowShouldClose())
  {
    auto   current = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double, std::chrono::milliseconds::period>(
                         current - previous)
                         .count();
    previous       = current;
    lag += elapsed;

    // input code...

    while (lag >= MS_PER_UPDATE)
    {
      // game clock dependent code...

      lag -= MS_PER_UPDATE;
    }

    // game clock independent code ( rendering )...

    characterSprite.update(elapsed);

    BeginDrawing();
    ClearBackground(LIME);

    characterSprite.render(0, 0);
    EndDrawing();
  }

  CloseWindow();
}

/* References:
 * Game Programming Patterns, p. 150 (State) -- Main loop style
 */
