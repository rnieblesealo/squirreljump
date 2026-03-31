#include "spritesheet-renderer.h"
#include "raylib.h"
#include <cstdint>
#include <string>

/**
 * @brief Window FPS limit
 */
const uint32_t TARGET_FPS = 60;

/**
 * @brief Identity directions
 */
const Vector2 DIRECTIONS[4]{{0, -1}, // Up
                            {0, 1},  // Down
                            {-1, 0}, // Left
                            {1, 0}}; // Right

void SPRITESHEET_RENDERER::advanceFrames()
{
  this->my_frame_counter++;

  if (this->my_frame_counter >= (TARGET_FPS / this->my_fps))
  {
    this->my_frame_counter = 0;

    this->my_current_frame++;

    if (this->my_current_frame == this->my_active_spritesheet->frame_count)
    {
      this->my_current_frame = 0;
    }

    this->my_frame_rect.y =
        static_cast<float>(this->my_current_frame * this->getFrameHeight());
  }
}

float SPRITESHEET_RENDERER::getFrameWidth() { return this->my_frame_rect.width; }

float SPRITESHEET_RENDERER::getFrameHeight() { return this->my_frame_rect.height; }

void SPRITESHEET_RENDERER::setFPS(uint32_t t) { this->my_fps = t; }

void SPRITESHEET_RENDERER::enableOutline(bool t) { this->should_render_outline = t; }

void SPRITESHEET_RENDERER::renderToDest(Rectangle &dest)
{
  this->advanceFrames();

  if (this->should_render_outline)
  {
    this->renderOutline(dest);
  }

  DrawTexturePro(this->my_active_spritesheet->spritesheet,
                 this->my_frame_rect,
                 dest,
                 Vector2{0, 0}, // No origin offset
                 0,             // No rotation
                 WHITE);        // Normal color
}

void SPRITESHEET_RENDERER::renderOutline(Rectangle &dest)
{
  /**
   * @brief Thickness of the outline
   *
   * We draw 4 black-tinted copies of the sprite offset by the same amount to top, bottom, left and right
   * This creates a poor man's stroke effect! :)
   */
  const int OUTLINE_THICKNESS_WIDTH = 3;
  for (const auto &dir : DIRECTIONS)
  {
    Rectangle dest_outline = {dest.x + OUTLINE_THICKNESS_WIDTH * dir.x,
                              dest.y + OUTLINE_THICKNESS_WIDTH * dir.y,
                              dest.width,
                              dest.height};

    DrawTexturePro(this->my_active_spritesheet->spritesheet,
                   this->my_frame_rect,
                   dest_outline,
                   Vector2{0, 0}, // No origin offset
                   0,             // No rotation
                   BLACK);        // Fully black  TODO: Make into option?
  }
}

bool SPRITESHEET_RENDERER::setSpritesheet(std::string key)
{
  if (!this->my_spritesheets.count(key))
  {
    return false;
  }

  this->my_active_spritesheet = &this->my_spritesheets.at(key);

  /**
   * @note We assume a vertically stacked spritesheet, always!
   */
  this->setFrameDimensions(
      static_cast<float>(this->my_active_spritesheet->spritesheet.width),
      static_cast<float>(
          static_cast<float>(this->my_active_spritesheet->spritesheet.height) /
          this->my_active_spritesheet->frame_count)

  );

  this->resetFrames();

  return true;
}

void SPRITESHEET_RENDERER::setFrameDimensions(float width, float height)
{
  this->my_frame_rect.width  = width;
  this->my_frame_rect.height = height;
}

void SPRITESHEET_RENDERER::resetFrames()
{
  this->my_frame_counter = 0;
  this->my_current_frame = 0;
  this->my_frame_rect.y  = 0;
}
