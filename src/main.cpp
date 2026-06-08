#include "player.h"
#include "spritesheet-renderer.h"
#include <chrono>
#include <filesystem>
#include <raylib.h>

const double MS_PER_UPDATE = 10; // the game's effective tickrate

const int screenWidth  = 400;
const int screenHeight = 300;

int main()
{
  InitWindow(screenWidth, screenHeight, "platformer");
  SetTargetFPS(60);

  Texture2D texCharIdle =
      LoadTexture(std::filesystem::path("../assets/player/PlayerIdle.png").c_str());
  Texture2D texCharRun =
      LoadTexture(std::filesystem::path("../assets/player/PlayerRun.png").c_str());

  std::unordered_map<std::string, SpritesheetImage const *> charSprites = {
      {"idle", new SpritesheetImage(texCharIdle, "idle", 1, 4)},
      {"run", new SpritesheetImage(texCharRun, "run", 1, 6)}};

  SpritesheetRenderer characterSprite(charSprites, 12, "idle");

  Player player(characterSprite);

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

    player.HandleInput();

    while (lag >= MS_PER_UPDATE)
    {
      player.update();

      lag -= MS_PER_UPDATE;
    }

    // game clock independent code ( rendering )...

    BeginDrawing();
    ClearBackground(LIME);

    player.render(lag / MS_PER_UPDATE, elapsed);

    EndDrawing();
  }

  CloseWindow();
}

/* References:
 * Game Programming Patterns, p. 150
 */
