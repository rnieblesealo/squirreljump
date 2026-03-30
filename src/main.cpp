#include "raylib.h"
#include <chrono>
#include <filesystem>
#include <format>
#include <random>
#include <vector>

const int FLOOR_Y       = 300; // how many pixels down floor should span for
const int PLAYER_SIZE   = 50;  // w + h of player hitbox
const int OBSTACLE_SIZE = PLAYER_SIZE;
const int COIN_SIZE     = PLAYER_SIZE;

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

  // <player logic vars>

  int   playerX    = 40;
  int   playerY    = FLOOR_Y - PLAYER_SIZE;
  float playerDy   = 0;
  float jumpHeight = 13;
  float gravity    = -0.75;
  bool  isGrounded = true;

  // <player logic vars/>

  // <dt>

  float dt            = 0; // in SECONDS
  auto  lastCycleTime = std::chrono::high_resolution_clock::now();

  // </dt>

  // <obstacle logic vars>

  std::vector<game_object> obstacles;

  // every n seconds,
  //  there is an x% chance that an obstacle will spawn!

  float obstacle_spawn_timer = 0;
  float obstacle_spawn_opportunity_interval =
      0.75; // in SECONDS; below 0.6ish makes it impossible for player to dodge evertyhiung
  float obstacle_spawn_chance = 0.75f; // 0-1
  float chance_obstacle_will_fly =
      0.5; // will this enemy fly? flying enemies make the player need to stay rather than jump

  // </obstacle logic vars>

  // <coin vars>

  std::vector<game_object> coins;

  float coin_spawn_timer                = 0;
  float coin_spawn_opportunity_interval = 0.6;
  float coin_spawn_chance               = 0.6;

  int coin_count      = 0;
  int coin_high_score = 0;

  // </coin vars>

  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    // <player logic>

    switch (gameState)
    {
    case TITLE:
    {
      // restore game speed
      currGameSpeed = baseGameSpeed;

      // restore player pos; vel
      playerDy   = 0;
      playerY    = FLOOR_Y - PLAYER_SIZE;
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
      if (playerY + PLAYER_SIZE >= FLOOR_Y)
      {
        playerDy = 0;
        playerY  = FLOOR_Y - PLAYER_SIZE;

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

    // update player hitbox according to pos
    Rectangle player_hitbox{static_cast<float>(playerX),
                            static_cast<float>(playerY),
                            PLAYER_SIZE,
                            PLAYER_SIZE};
    // <player logic/>

    // <dt>

    auto currentTime = std::chrono::high_resolution_clock::now();
    dt = std::chrono::duration<float, std::chrono::seconds::period>(currentTime -
                                                                    lastCycleTime)
             .count();

    lastCycleTime = currentTime; // then becomes now

    // </dt>

    // <obstacle logic>

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
            obstacles.push_back(game_object{screenWidth + OBSTACLE_SIZE,
                                            FLOOR_Y - OBSTACLE_SIZE - PLAYER_SIZE * 2});
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
                  PLAYER_SIZE}); // this SPECIFIC setup will put coins right between flying enemies and player
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

    BeginDrawing();

    // draw floor
    DrawRectangle(0, FLOOR_Y, screenWidth, screenHeight - FLOOR_Y, GREEN);

    // draw all obstacles
    for (auto &obstacle : obstacles)
    {
      DrawRectangle(obstacle.xpos, obstacle.ypos, OBSTACLE_SIZE, OBSTACLE_SIZE, PURPLE);
    }

    // draw all coins
    for (auto &coin : coins)
    {
      DrawRectangle(coin.xpos, coin.ypos, OBSTACLE_SIZE, OBSTACLE_SIZE, YELLOW);
    }

    // draw player
    DrawRectangle(playerX, playerY, PLAYER_SIZE, PLAYER_SIZE, ORANGE);

    // draw gui
    std::string coin_gui =
        coin_high_score > 0
            ? std::format("SCORE {} ( HIGH SCORE {} )", coin_count, coin_high_score)
            : std::format("SCORE {}", coin_count);

    std::string debug =
        std::format("STATE: {} BLOCKED: {} PRESSING: {}",
                    static_cast<int>(gameState),
                    static_cast<int>(blocked),
                    static_cast<int>(GetTouchPointCount() > 0 || IsKeyDown(KEY_SPACE)));

    DrawText(coin_gui.c_str(), 0, 0, 24, WHITE);
    DrawText(debug.c_str(), 0, 48, 24, WHITE);

    ClearBackground(BLUE);

    EndDrawing();
  }

  CloseWindow(); // Close window and OpenGL context

  return 0;
}
