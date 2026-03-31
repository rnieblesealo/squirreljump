#include "raylib.h"
#include <chrono>
#include <filesystem>
#include <random>
#include <spritesheet-renderer.h>
#include <vector>

const int OBSTACLE_SIZE = 50;
const int COIN_SIZE     = 50;

const int PLAYER_JUMP_HEIGHT = 13;
const int PLAYER_SPRITE_SIZE = 75; // how big we want the sprite to be in the window
                                   //
const int FLOOR_Y = 300;           // position of floor

const float GRAVITY         = -0.75;
const float BASE_GAME_SPEED = 7.5f;

const int SCREEN_WIDTH  = 400;
const int SCREEN_HEIGHT = 400;

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

typedef struct coin
{

} coin;

typedef struct obstacle
{
  int xpos;
  int ypos;

  bool flying = false;
} obstacle;

typedef struct player
{
  int                                   xpos        = 40;
  int                                   ypos        = FLOOR_Y - PLAYER_SPRITE_SIZE;
  float                                 dy          = 0;
  bool                                  is_grounded = true;
  std::shared_ptr<SPRITESHEET_RENDERER> sprite      = nullptr;

  // these two are completely eyeballed to taste

  Rectangle hitbox = {(float)xpos,
                      (float)ypos,
                      (float)PLAYER_SPRITE_SIZE - 30,
                      (float)PLAYER_SPRITE_SIZE - 20};

  Rectangle draw_rect = {(float)xpos - 12,
                         (float)ypos - 6,
                         (float)PLAYER_SPRITE_SIZE,
                         (float)PLAYER_SPRITE_SIZE};

} player;

typedef struct input_state
{
  bool held_down            = false;
  bool has_done_first_input = false;
} input_state;

input_state inputs;

bool UserInput()
{
  bool getting_input = GetTouchPointCount() > 0 || IsKeyDown(KEY_SPACE);

  /* checking if input isn't already held down when getting new input implements
  no key repeat behavior; this is so that when we do things like press to
  change game state the transitions aren't skipped due to key repeat */

  if (!inputs.held_down && getting_input)
  {
    inputs.held_down = true;
    inputs.has_done_first_input =
        true; // this is only set once and is used to define game state

    return true;
  }
  else
  {
    return false;
  }
}

int main(void)
{

  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "squirrel");
  InitAudioDevice();

  SetTargetFPS(60);

  Sound pickupCoin = LoadSound(std::filesystem::path("../assets/pickupCoin.wav").c_str());
  Sound explosion  = LoadSound(std::filesystem::path("../assets/explosion.wav").c_str());
  Sound jump       = LoadSound(std::filesystem::path("../assets/jump.wav").c_str());
  Sound land       = LoadSound(std::filesystem::path("../assets/land.wav").c_str());

  std::mt19937 random_num_generator(std::chrono::system_clock::now()
                                        .time_since_epoch()
                                        .count()); // this is seeded with the current time

  float currGameSpeed = BASE_GAME_SPEED;

  GameState gameState = TITLE;

  Texture2D floor_texture =
      LoadTexture(std::filesystem::path("../assets/floor.png").c_str());

  const float FLOOR_SCALE = static_cast<float>(SCREEN_WIDTH) / floor_texture.width;
  const float FLOOR_WIDTH = floor_texture.width * FLOOR_SCALE;

  float floor_a_x = 0;
  float floor_b_x = FLOOR_WIDTH;

  // === PLAYER ==========================================================================

  player player;

  // sprite setup

  // Load Sprites
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
  niko_spritesheet_renderer->enableOutline(true);
  niko_spritesheet_renderer->setSpritesheet("run");

  player.sprite = niko_spritesheet_renderer;

  // === DELTATIME =======================================================================

  float dt            = 0; // in SECONDS
  auto  lastCycleTime = std::chrono::high_resolution_clock::now();

  // === OBSTACLES =======================================================================

  std::vector<obstacle> obstacles;

  // every n seconds,
  //  there is an x% chance that an obstacle will spawn!

  float obstacle_spawn_timer = 0;
  float obstacle_spawn_opportunity_interval =
      0.75; // in SECONDS; below 0.6ish makes it impossible for player to dodge evertyhiung
  float obstacle_spawn_chance = 0.75f; // 0-1
  float chance_obstacle_will_fly =
      0.5; // will this enemy fly? flying enemies make the player need to stay rather than jump

  Texture2D cactus_texture =
      LoadTexture(std::filesystem::path("assets/cactus.png").c_str());

  Texture2D eagle_texture =
      LoadTexture(std::filesystem::path("assets/eagle.png").c_str());

  const float GROUND_OBSTACLE_SCALE = (float)OBSTACLE_SIZE / cactus_texture.width;
  const float SKY_OBSTACLE_SCALE    = (float)OBSTACLE_SIZE / eagle_texture.width;

  // === COINS ===========================================================================

  std::vector<game_object> coins;

  float coin_spawn_timer                = 0;
  float coin_spawn_opportunity_interval = 0.6;
  float coin_spawn_chance               = 0.6;

  int coin_count      = 0;
  int coin_high_score = 0;

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
      currGameSpeed = 0;

      // restore player pos; vel
      player.dy          = 0;
      player.ypos        = FLOOR_Y - player.hitbox.height;
      player.is_grounded = true;

      // clear all coins, enemies
      coins.clear();
      obstacles.clear();

      // reset coin count
      coin_count = 0;

      // disable score & spawns until we are playing

      if (UserInput())
      {
        gameState = PLAYING;

        // perform a jump
        player.is_grounded = false;
        player.dy          = -PLAYER_JUMP_HEIGHT;

        PlaySound(jump);
      }
    }
    case PLAYING:
    {
      currGameSpeed = BASE_GAME_SPEED;

      if (UserInput() && player.is_grounded)
      {
        player.is_grounded = false;
        player.dy          = -PLAYER_JUMP_HEIGHT;

        PlaySound(jump);
      }

      player.dy -= GRAVITY;     // apply acceleration
      player.ypos += player.dy; // apply vel

      // if the player touches the ground again
      // (lowest edge y is below floor)
      // reset player's velocity and put them on the floor
      if (player.ypos + player.hitbox.height >= FLOOR_Y)
      {
        player.dy   = 0;
        player.ypos = FLOOR_Y - player.hitbox.height;

        if (!player.is_grounded)
        {
          PlaySound(land);
        }

        player.is_grounded = true;
      }

      break;
    }
    case GAME_OVER:
    {
      // freeze the game until input is received
      currGameSpeed = 0.0f;

      if (UserInput())
      {
        gameState = TITLE;
      }

      break;
    }
    }

    // since pos changed, update player draw rect and hitbox positions
    player.draw_rect.x = player.xpos - 12;
    player.draw_rect.y = player.ypos - 6;

    player.hitbox.x = player.xpos;
    player.hitbox.y = player.ypos;

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
            int flying_offset = PLAYER_SPRITE_SIZE * 2;

            obstacles.push_back(obstacle{SCREEN_WIDTH + OBSTACLE_SIZE,
                                         FLOOR_Y - OBSTACLE_SIZE - flying_offset,
                                         true});
          }
          else
          {
            // obstacle is at ground
            obstacles.push_back(
                obstacle{SCREEN_WIDTH + OBSTACLE_SIZE, FLOOR_Y - OBSTACLE_SIZE, false});
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

        if (CheckCollisionRecs(player.hitbox, obstacle_hitbox) && gameState == PLAYING)
        {
          gameState = GAME_OVER;

          // play death sound
          PlaySound(explosion);

          // freeze game
          currGameSpeed = 0;
          player.dy     = 0;

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
              SCREEN_WIDTH + COIN_SIZE,
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

        if (CheckCollisionRecs(player.hitbox, coin_hitbox))
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
      inputs.held_down = false;
    }

    // WARNING: BAD INPUT LOGIC

    if (floor_a_x < (float)-SCREEN_WIDTH)
    {
      floor_a_x = (float)SCREEN_WIDTH - (abs(floor_a_x) - (float)SCREEN_WIDTH);
    }

    if (floor_b_x < (float)-SCREEN_WIDTH)
    {

      floor_b_x = (float)SCREEN_WIDTH - (abs(floor_b_x) - (float)SCREEN_WIDTH);
    }

    floor_a_x -= currGameSpeed;
    floor_b_x -= currGameSpeed;

    // === DRAWING =======================================================================

    BeginDrawing();

    // draw floors

    if (gameState == TITLE && !inputs.has_done_first_input)
    {
      DrawRectangle(0, FLOOR_Y, SCREEN_WIDTH, 2, RED);
    }
    else
    {
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
    }

    // draw all obstacles
    for (auto &obstacle : obstacles)
    {
      // change sprite depending on whether is flying or not

      if (obstacle.flying)
      {

        DrawTextureEx(
            eagle_texture,
            {static_cast<float>(obstacle.xpos), static_cast<float>(obstacle.ypos)},
            0,
            GROUND_OBSTACLE_SCALE,
            WHITE);
      }
      else
      {
        DrawTextureEx(
            cactus_texture,
            {static_cast<float>(obstacle.xpos), static_cast<float>(obstacle.ypos)},
            0,
            GROUND_OBSTACLE_SCALE,
            WHITE);
      }
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

    // draw debug stuff

    /*
    std::string debug =
        std::format("STATE: {}\nBLOCKED: {}\nPRESSING: {}\nM: {}, {}\nGRND: {} DY: {}",
                    static_cast<int>(gameState),
                    static_cast<int>(inputs.held_down),
                    static_cast<int>(GetTouchPointCount() > 0 || IsKeyDown(KEY_SPACE)),
                    GetMouseX(),
                    GetMouseY(),
                    player.is_grounded,
                    player.dy);
    */

    if (gameState != TITLE)
    {
      std::string coin_gui =
          coin_high_score > 0
              ? std::format("score {} ( high score {} )", coin_count, coin_high_score)
              : std::format("score {}", coin_count);

      DrawText(coin_gui.c_str(), 0, 0, 22, WHITE);
    }
    // DrawText(debug.c_str(), 0, 48, 24, WHITE);

    // DrawRectangleRec(player.draw_rect, WHITE);
    // DrawRectangleRec(player.hitbox, RED);

    // draw player

    niko_spritesheet_renderer->renderToDest(player.draw_rect);

    float extra_y_offset = 28;

    if (gameState == GAME_OVER)
    {
      DrawText("game over",
               (float)SCREEN_WIDTH / 2 - (float)MeasureText("game over", 32) / 2,
               (float)SCREEN_HEIGHT / 2 - (float)32 / 2,
               32,
               RED);
    }

    if (gameState == TITLE)
    {
      if (!inputs.has_done_first_input)
      {

        DrawText("404",
                 (float)SCREEN_WIDTH / 2 - (float)MeasureText("404", 48) / 2,
                 (float)SCREEN_HEIGHT / 2 - (float)48 / 2 - extra_y_offset,
                 48,
                 RED);

        ClearBackground(BLACK);
      }
      else
      {
        DrawText("press any key/tap",
                 (float)SCREEN_WIDTH / 2 -
                     (float)MeasureText("press any key/tap", 22) / 2,
                 (float)SCREEN_HEIGHT / 2 - (float)22 / 2 - extra_y_offset,
                 22,
                 WHITE);

        ClearBackground({23, 115, 184, 255});
      }
    }
    else
    {
      ClearBackground({23, 115, 184, 255});
    }

    EndDrawing();
  }

  CloseWindow(); // Close window and OpenGL context

  return 0;
}
