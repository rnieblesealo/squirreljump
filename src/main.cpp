#include "raylib.h"
#include <chrono>
#include <filesystem>
#include <format>
#include <random>
#include <spritesheet-renderer.h>
#include <vector>

const int FLOOR_Y            = 300; // how many pixels down floor should span for
const int PLAYER_SPRITE_SIZE = 80;  // w + h of player hitbox
const int OBSTACLE_SIZE      = 50;
const int COIN_SIZE          = 50;

enum GameState : uint8_t
{
  TITLE     = 0,
  PLAYING   = 1,
  GAME_OVER = 2
};

typedef struct game_object
{
  int xpos;
  int ypos;

  // size is constant to all for now, might change later
  // they will also all move at same speed on x axis for now
  // both of the above might turn into variables later on!
} game_object;

// <input>
bool blocked = false;
bool PlayerInput()
{
  if (!blocked && (GetTouchPointCount() > 0 || IsKeyDown(KEY_SPACE)))
  {
    blocked = true;
    return true;
  }

  return false;
}
// </input>

int main(void)
{
  const int screenWidth  = 600;
  const int screenHeight = 400;

  InitWindow(screenWidth, screenHeight, "squirrel");
  InitAudioDevice();

  SetTargetFPS(60);

  Sound pickupCoin = LoadSound(std::filesystem::path("../assets/pickupCoin.wav").c_str());
  Sound explosion  = LoadSound(std::filesystem::path("../assets/explosion.wav").c_str());
  Sound jump       = LoadSound(std::filesystem::path("../assets/jump.wav").c_str());
  Sound land       = LoadSound(std::filesystem::path("../assets/land.wav").c_str());

  std::mt19937 random_num_generator(std::chrono::system_clock::now()
                                        .time_since_epoch()
                                        .count()); // this is seeded with the current time

  float baseGameSpeed = 7.5f;
  float currGameSpeed = baseGameSpeed;

  GameState gameState = TITLE;

  Texture2D floor_texture =
      LoadTexture(std::filesystem::path("../assets/floor.png").c_str());

  // do 2 cycling floors thing

  const float FLOOR_SCALE = static_cast<float>(screenWidth) / floor_texture.width;
  const float FLOOR_WIDTH = floor_texture.width * FLOOR_SCALE;

  float floor_a_x = 0;
  float floor_b_x = FLOOR_WIDTH;

  // === PLAYER ===============================================================================

  int   playerX    = 40;
  int   playerY    = FLOOR_Y - PLAYER_SPRITE_SIZE;
  float playerDy   = 0;
  float jumpHeight = 13;
  float gravity    = -0.75;
  bool  isGrounded = true;

  Texture2D spr_niko_run =
      LoadTexture(std::filesystem::path("assets/niko-run.png").c_str());
  Texture2D spr_niko_jump =
      LoadTexture(std::filesystem::path("assets/niko-jump.png").c_str());

  // Organize them into individual spritesheets
  std::map<std::string, SPRITESHEET const &> const &niko_spritesheets{
      {"run", SPRITESHEET{spr_niko_run, 8}}, {"jump", SPRITESHEET{spr_niko_jump, 1}}};

  // Create sprite renderer out of individual spritesheets
  std::shared_ptr<SPRITESHEET_RENDERER> niko_spritesheet_renderer(
      new SPRITESHEET_RENDERER(niko_spritesheets));

  // Apply some settings to renderer
  niko_spritesheet_renderer->setFPS(8);
  niko_spritesheet_renderer->enableOutline(false);
  niko_spritesheet_renderer->setSpritesheet("run");

  // === DELTATIME =======================================================================

  float dt            = 0; // in SECONDS
  auto  lastCycleTime = std::chrono::high_resolution_clock::now();

  // === OBSTACLES =======================================================================

  std::vector<game_object> obstacles;

  // every n seconds,
  //  there is an x% chance that an obstacle will spawn!

  float obstacle_spawn_timer = 0;
  float obstacle_spawn_opportunity_interval =
      0.75; // in SECONDS; below 0.6ish makes it impossible for player to dodge evertyhiung
  float obstacle_spawn_chance = 0.75f; // 0-1
  float chance_obstacle_will_fly =
      0.5; // will this enemy fly? flying enemies make the player need to stay rather than jump

  // === COINS ===========================================================================

  std::vector<game_object> coins;

  float coin_spawn_timer                = 0;
  float coin_spawn_opportunity_interval = 0.6;
  float coin_spawn_chance               = 0.6;

  int coin_count      = 0;
  int coin_high_score = 0;

  // update player hitbox
  Rectangle player_hitbox{static_cast<float>(playerX),
                          static_cast<float>(playerY),
                          PLAYER_SPRITE_SIZE - 30,
                          PLAYER_SPRITE_SIZE - 20};

  Texture2D coin_sprite =
      LoadTexture(std::filesystem::path("assets/cigs-red.png").c_str());

  // === MAIN LOOP =======================================================================

  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    switch (gameState)
    {
    case TITLE:
    {
      // restore game speed
      currGameSpeed = baseGameSpeed;

      // restore player pos; vel
      playerDy   = 0;
      playerY    = FLOOR_Y - player_hitbox.height;
      isGrounded = true;

      // clear all coins, enemies
      coins.clear();
      obstacles.clear();

      // reset coin count
      coin_count = 0;

      // disable score & spawns until we are playing

      if (PlayerInput())
      {
        gameState = PLAYING;

        // perform a jump
        isGrounded = false;
        playerDy   = -jumpHeight;

        PlaySound(jump);
      }
    }
    case PLAYING:
    {
      if (PlayerInput() && isGrounded)
      {
        isGrounded = false;
        playerDy   = -jumpHeight;

        PlaySound(jump);
      }

      playerDy -= gravity; // apply acceleration
      playerY += playerDy; // apply vel

      // if the player touches the ground again
      // (lowest edge y is below floor)
      // reset player's velocity and put them on the floor
      if (playerY + player_hitbox.height >= FLOOR_Y)
      {
        playerDy = 0;
        playerY  = FLOOR_Y - player_hitbox.height;

        if (!isGrounded)
        {
          PlaySound(land);
        }

        isGrounded = true;
      }

      break;
    }
    case GAME_OVER:
    {
      // freeze the game until input is received
      currGameSpeed = 0.0f;

      if (PlayerInput())
      {
        gameState = TITLE;
      }

      break;
    }
    }

    // update player draw rect
    Rectangle player_draw_rect{static_cast<float>(playerX - 12),
                               static_cast<float>(playerY - 6),
                               PLAYER_SPRITE_SIZE,
                               PLAYER_SPRITE_SIZE};

    // update player hitbox
    player_hitbox.x = playerX;
    player_hitbox.y = playerY;

    // update deltatime
    auto currentTime = std::chrono::high_resolution_clock::now();
    dt = std::chrono::duration<float, std::chrono::seconds::period>(currentTime -
                                                                    lastCycleTime)
             .count();

    lastCycleTime = currentTime; // then becomes now

    // OBSTACLE LOGIC

    if (gameState == PLAYING)
    {
      // move all obstacles towards player
      for (auto &obstacle : obstacles)
      {
        obstacle.xpos -= currGameSpeed;
      }

      // spawn more obstacles
      obstacle_spawn_timer += dt;
      if (obstacle_spawn_timer >= obstacle_spawn_opportunity_interval)
      {
        bool will_spawn_obstacle = std::bernoulli_distribution{obstacle_spawn_chance}(
            random_num_generator); // TODO: what is this syntax? constructor with braces???

        if (will_spawn_obstacle)
        {
          bool obstacle_will_fly =
              std::bernoulli_distribution{chance_obstacle_will_fly}(random_num_generator);

          if (obstacle_will_fly)
          {
            obstacles.push_back(
                game_object{screenWidth + OBSTACLE_SIZE,
                            FLOOR_Y - OBSTACLE_SIZE - PLAYER_SPRITE_SIZE * 2});
          }
          else
          {
            // obstacle is at ground
            obstacles.push_back(
                game_object{screenWidth + OBSTACLE_SIZE, FLOOR_Y - OBSTACLE_SIZE});
          }
        }

        obstacle_spawn_timer = 0;
      }

      // check if player touched obstacle
      // game over if so
      for (const auto &obstacle : obstacles)
      {
        Rectangle obstacle_hitbox{static_cast<float>(obstacle.xpos),
                                  static_cast<float>(obstacle.ypos),
                                  COIN_SIZE,
                                  COIN_SIZE};

        if (CheckCollisionRecs(player_hitbox, obstacle_hitbox) && gameState == PLAYING)
        {
          gameState = GAME_OVER;

          // play death sound
          PlaySound(explosion);

          // freeze game
          currGameSpeed = 0;
          playerDy      = 0;

          // set coin high score
          coin_high_score = coin_count;
        }
      }
    }

    // </obstacle logic>

    // <coin logic>

    if (gameState == PLAYING)
    {
      for (auto &coin : coins)
        coin.xpos -= currGameSpeed;

      coin_spawn_timer += dt;
      if (coin_spawn_timer >= coin_spawn_opportunity_interval)
      {
        bool will_spawn_coin = std::bernoulli_distribution{coin_spawn_chance}(
            random_num_generator); // TODO: what is this syntax? constructor with braces???

        if (will_spawn_coin)
        {
          coins.push_back(game_object{
              screenWidth + COIN_SIZE,
              FLOOR_Y - COIN_SIZE -
                  PLAYER_SPRITE_SIZE}); // this SPECIFIC setup will put coins right between flying enemies and player
        }

        coin_spawn_timer = 0;
      }

      // check if player collected (touched) coin

      // NOTE:
      // since we are removing from array as we go, we need to update the iterator each time we perform a remval

      for (auto iter = coins.begin();
           iter != coins.end();) // continue iterating until the iterator becomes null
      {
        Rectangle coin_hitbox{static_cast<float>(iter->xpos),
                              static_cast<float>(iter->ypos),
                              COIN_SIZE,
                              COIN_SIZE};

        if (CheckCollisionRecs(player_hitbox, coin_hitbox))
        {
          iter = coins.erase(iter); // iterator updated HERE
          PlaySound(pickupCoin);
          coin_count++;
        }
        else
        {
          ++iter; // move on to next item if no removals were performed
        }
      }
    }

    // </coin logic>

    // WARNING: BAD INPUT LOGIC

    if (!(GetTouchPointCount() > 0 || IsKeyDown(KEY_SPACE)))
    {
      blocked = false;
    }

    // WARNING: BAD INPUT LOGIC

    if (floor_a_x < -screenWidth)
    {
      floor_a_x = screenWidth - (abs(floor_a_x) - screenWidth);
    }

    if (floor_b_x < -screenWidth)
    {

      floor_b_x = screenWidth - (abs(floor_b_x) - screenWidth);
    }

    floor_a_x -= currGameSpeed;
    floor_b_x -= currGameSpeed;

    // === DRAWING =======================================================================

    BeginDrawing();

    // draw floors
    // DrawRectangle(0, FLOOR_Y, screenWidth, screenHeight - FLOOR_Y, GREEN);

    DrawTextureEx(floor_texture,
                  {static_cast<float>(floor_a_x), static_cast<float>(FLOOR_Y)},
                  0,
                  FLOOR_SCALE,
                  WHITE);
    DrawTextureEx(floor_texture,
                  {static_cast<float>(floor_b_x), static_cast<float>(FLOOR_Y)},
                  0,
                  FLOOR_SCALE,
                  WHITE);

    // draw all obstacles
    for (auto &obstacle : obstacles)
    {
      DrawRectangle(obstacle.xpos, obstacle.ypos, OBSTACLE_SIZE, OBSTACLE_SIZE, PURPLE);
      /*
      DrawTextureEx(
          obstacle_sprite,
          {static_cast<float>(obstacle.xpos), static_cast<float>(obstacle.ypos)},
          0,
          1,
          WHITE);
        */
    }

    // draw all coins
    for (auto &coin : coins)
    {
      DrawTextureEx(coin_sprite,
                    {static_cast<float>(coin.xpos), static_cast<float>(coin.ypos)},
                    0,
                    1,
                    WHITE);
    }

    // draw player
    // DrawRectangle(playerX, playerY, PLAYER_SPRITE_SIZE, PLAYER_SPRITE_SIZE, ORANGE);
    /*
    DrawRectangle(player_hitbox.x,
                  player_hitbox.y,
                  player_hitbox.width,
                  player_hitbox.height,
                  PURPLE);
    */
    niko_spritesheet_renderer->renderToDest(player_draw_rect);

    // draw gui
    std::string coin_gui =
        coin_high_score > 0
            ? std::format("SCORE {} ( HIGH SCORE {} )", coin_count, coin_high_score)
            : std::format("SCORE {}", coin_count);

    std::string debug =
        std::format("STATE: {} BLOCKED: {} PRESSING: {} M: {}, {}",
                    static_cast<int>(gameState),
                    static_cast<int>(blocked),
                    static_cast<int>(GetTouchPointCount() > 0 || IsKeyDown(KEY_SPACE)),
                    GetMouseX(),
                    GetMouseY());

    // DrawText(coin_gui.c_str(), 0, 0, 24, WHITE);
    // DrawText(debug.c_str(), 0, 48, 24, WHITE);

    ClearBackground(ORANGE);

    EndDrawing();
  }

  CloseWindow(); // Close window and OpenGL context

  return 0;
}
