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
  InitAudioDevice();
  SetTargetFPS(60);

#ifdef DEBUG
  // sound

  Sound footstep = LoadSound("../assets/footstep.wav");

  std::unordered_map<std::string, Sound const &> charSfx = {{"footstep", footstep}};

  // animation

  Texture2D texCharIdle =
      LoadTexture(std::filesystem::path("../assets/player/PlayerIdle.png").c_str());
  Texture2D texCharRun =
      LoadTexture(std::filesystem::path("../assets/player/PlayerRun.png").c_str());

  SpritesheetImage imgCharIdle(texCharIdle, 1, 4);
  SpritesheetImage imgCharRun(texCharRun, 1, 6);

  std::map<unsigned int, KeyframeData> kfCharRun = {
      {
          2,
          KeyframeData({{PLAY_SOUND, "footstep"}}),
      },
      {
          5,
          KeyframeData({{PLAY_SOUND, "footstep"}}),
      }};

  Animation const *charIdle = new Animation(imgCharIdle, nullptr);
  Animation const *charRun  = new Animation(imgCharRun, &kfCharRun);

  std::unordered_map<std::string, Animation const *> charAnims = {{"idle", charIdle},
                                                                  {"run", charRun}};

  SpritesheetRenderer charAnimator(charAnims, "idle", &charSfx);

  // player

  Player player(charAnimator);
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

#ifdef DEBUG
    player.HandleInput();
#endif

    while (lag >= MS_PER_UPDATE)
    {
#ifdef DEBUG
      player.update();
#endif

      lag -= MS_PER_UPDATE;
    }

    // game clock independent code ( rendering )...

    BeginDrawing();
    ClearBackground(LIME);

#ifdef DEBUG
    player.render(lag / MS_PER_UPDATE, elapsed);
#endif

    EndDrawing();
  }

  CloseWindow();
}

/* References:
 * Game Programming Patterns, p. 150
 */
