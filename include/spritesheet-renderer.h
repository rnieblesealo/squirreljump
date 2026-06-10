#pragma once

#include <map>
#include <raylib.h>
#include <string>
#include <unordered_map>
#include <vector>

struct SpritesheetImage
{
  SpritesheetImage(Texture2D &image, unsigned int rows, unsigned int cols);

  Texture2D &_image;

  unsigned int _frame_count;
  unsigned int _rows;
  unsigned int _cols;
  unsigned int _frame_width;
  unsigned int _frame_height;
};

enum KEYFRAME_INSTRUCTION
{
  PLAY_SOUND
};

struct KeyframeData
{
  std::vector<std::pair<KEYFRAME_INSTRUCTION, std::string>> _instructions;
};

struct Animation
{
  Animation(SpritesheetImage const                     &spritesheet,
            std::map<unsigned int, KeyframeData> const *keyFrames);

  SpritesheetImage const                     &_spritesheet;
  std::map<unsigned int, KeyframeData> const *_keyframes;
};

class SpritesheetRenderer
{
public:
  SpritesheetRenderer(
      std::unordered_map<std::string, Animation const *> const &animations,
      std::string                                               startAnimation,
      std::unordered_map<std::string, Sound const &> const     *soundEffects = nullptr);
  ~SpritesheetRenderer();

  void flip(double deltaTime);
  void render(float x, float y);
  void switchTo(std::string key);

private:
  Animation const                                      *_curr;
  std::unordered_map<std::string, Animation const *>    _animations;
  std::unordered_map<std::string, Sound const &> const *_sfx;

  unsigned int _fps;
  double       _frame_duration;
  unsigned int _frame;
  double       _frame_timer;
};
