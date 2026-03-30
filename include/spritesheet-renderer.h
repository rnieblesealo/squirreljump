#ifndef SPRITESHEET_RENDERER_H
#define SPRITESHEET_RENDERER_H

#include "raylib.h"
#include <cstdint>
#include <map>

/**
 * @brief Default sprite animation speed
 */
const uint32_t DEFAULT_FPS = 8;

/**
 * @brief Value object for an individual spritesheet
 * Different spritesheets represent different animation states (running, jumping, crouching, etc.)
 */
typedef struct SPRITESHEET
{
  Texture2D &spritesheet;
  uint32_t   frame_count;
} SPRITESHEET;

/**
 * @brief Manages and renders frames from a vertically stacked spritesheet.
 *
 * This component simplifies rendering animated sprites, handling frame
 * advancement and source rectangle calculation. It requires a spritesheet
 * where frames are stacked vertically. The `render` method functions
 * similarly to Raylib's `DrawTexturePro`, using a destination rectangle
 * to define the on-screen position and scale. This design keeps external
 * scaling straightforward and convenient.
 */
class SPRITESHEET_RENDERER final
{
private:
  /**
   * @brief Contains animation states
   */
  std::map<std::string, SPRITESHEET const &> const &my_spritesheets;

  /**
   * @brief A reference to the active spritesheet
   */
  SPRITESHEET const *my_active_spritesheet;

  /**
   * @brief Rect of the current frame relative to the spritesheet
   */
  Rectangle my_frame_rect;

  /**
   * @brief The animation speed in FPS
   */
  uint32_t my_fps;

  /**
   * @brief Counts elapsed game frames since last frame advance
   */
  uint32_t my_frame_counter;

  /**
   * @brief The index of the currently displayed frame
   */
  uint32_t my_current_frame;

  /**
   * @brief Specifies whether to render an outline around this sprite
   */
  bool should_render_outline;

  /**
   * @brief Resets frame to the first one and clears counter and timer
   */
  void resetFrames();

  /**
   * @brief Advances the spritesheet's animation
   */
  void advanceFrames();

  /**
   * @brief Renders a poor-man's outline around the spritesheet
   *
   * A poor man's outline = drawing 4 black-tinted copies of this spritesheet
   * and rendering each one offset slightly to each of the 4 directions
   * */
  void renderOutline(Rectangle &dest);

public:
  explicit SPRITESHEET_RENDERER(
      std::map<std::string, SPRITESHEET const &> const &spritesheets)
      : my_spritesheets(spritesheets)
      , my_active_spritesheet(nullptr) // No animation is active by default!
      , my_fps(DEFAULT_FPS)
      , my_frame_counter(0)
      , my_current_frame(0)
      , should_render_outline(false)
      , my_frame_rect(Rectangle{0, 0, 0, 0})
  {
  }

  /**
   * @brief Getters
   * @returns Unscaled width and height of a frame
   */
  float getFrameWidth();
  float getFrameHeight();

  /**
   * @brief Setters
   * @param t = the new value to use
   */
  void enableOutline(bool t);
  void setFPS(uint32_t t);

  /**
   * @brief Changes the active spritesheet
   * @param key The key of the desired spritesheet
   * @returns True if set animation successfully, False otherwise
   */
  bool setSpritesheet(std::string key);

  /**
   * @brief Changes the dimensions of the frame rect
   * @param width The new width
   * @param height The new height
   *
   * This should be used when changing spritesheets!
   */
  void setFrameDimensions(float width, float height);

  /**
   * @brief Advances the animation and renders the animated spritesheet
   * @param dest Screen-relative rect where the sprite will be drawn
   */
  void renderToDest(Rectangle &dest);
};

#endif
