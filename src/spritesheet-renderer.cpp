#include "spritesheet-renderer.h"
#include "audio-locator.h"
#include <cassert>
#include <raylib.h>

SpritesheetImage::SpritesheetImage(Texture2D &image, unsigned int rows, unsigned int cols)
    : _image(image)
    , _frame_count(rows * cols)
    , _rows(rows)
    , _cols(cols)
    , _frame_width(image.width / cols)
    , _frame_height(image.height / rows)
{
}

Animation::Animation(SpritesheetImage const                     &spritesheet,
                     std::map<unsigned int, KeyframeData> const *keyFrames)
    : _spritesheet(spritesheet)
    , _keyframes(keyFrames)
{
}

SpritesheetRenderer::SpritesheetRenderer(
    std::unordered_map<std::string, Animation const *> const &animations,
    std::string                                               startAnimation)
    : _animations(animations)
    , _fps(12)
    , _frame_duration(1000.0 / _fps) // in ms
    , _frame(0)
    , _frame_timer(0)
    , _curr(nullptr)
{
  assert(_animations.contains(startAnimation));
  switchTo(startAnimation);
}

SpritesheetRenderer::~SpritesheetRenderer() {};

void SpritesheetRenderer::flip(double deltaTime)
{
  _frame_timer += deltaTime;

  if (_frame_timer >= _frame_duration)
  {
    if (_frame + 1 >= _curr->_spritesheet._frame_count)
      _frame = 0;
    else
      ++_frame;
    _frame_timer = 0;

    if (!_curr->_keyframes)
      return;

    if (_curr->_keyframes->contains(_frame))
    {
      KeyframeData k = _curr->_keyframes->at(_frame);
      for (auto const &instr : k._instructions)
      {
        switch (instr.first)
        {
        case PLAY_SOUND:
        {
#ifdef DEBUG
          AudioLocator::getAudio()->playSound(instr.second);
#endif
          break;
        }
        }
      }
    }
  }
}

void SpritesheetRenderer::render(float x, float y)
{
  int fiw = _frame % _curr->_spritesheet._cols;
  int fih = _frame / _curr->_spritesheet._cols;

  Rectangle src = {static_cast<float>(_curr->_spritesheet._frame_width * fiw),
                   static_cast<float>(_curr->_spritesheet._frame_height * fih),
                   static_cast<float>(_curr->_spritesheet._frame_width),
                   static_cast<float>(_curr->_spritesheet._frame_height)};

  unsigned int scale = 2;
  Rectangle    dest  = {x,
                        y,
                        static_cast<float>(_curr->_spritesheet._frame_width * scale),
                        static_cast<float>(_curr->_spritesheet._frame_height * scale)};

  DrawTexturePro(_curr->_spritesheet._image, src, dest, Vector2{0, 0}, scale, RAYWHITE);
}

void SpritesheetRenderer::switchTo(std::string key)
{
  if (!_animations.contains(key))
    return;
  _curr = _animations.at(key);

  _frame       = 0;
  _frame_timer = 0;
}
