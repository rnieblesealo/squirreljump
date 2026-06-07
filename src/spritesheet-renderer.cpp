#include "spritesheet-renderer.h"
#include <cassert>
#include <raylib.h>

#ifdef DEBUG
#include <iostream>
#endif

SpritesheetImage::SpritesheetImage(Texture2D   &image,
                                   std::string  key,
                                   unsigned int frameCount)
    : _image(image)
    , _key(key)
    , _frame_count(frameCount)
    , _fw(image.width / frameCount)
    , _fh(image.height)
{
}

SpritesheetRenderer::SpritesheetRenderer(
    std::unordered_map<std::string, SpritesheetImage const *> images,
    unsigned int                                              fps,
    std::string                                               defaultKey)
    : _images(images)
    , _fps(fps)
    , _frame_duration(1000.0 / _fps)
{
  assert(_images.contains(defaultKey));
  switchTo(defaultKey);
}

SpritesheetRenderer::~SpritesheetRenderer()
{
  for (const auto &[key, value] : _images)
    delete value;
};

void SpritesheetRenderer::update(double deltaTime)
{
  _frame_timer += deltaTime;

  if (_frame_timer >= _frame_duration)
  {
    if (_frame + 1 >= _curr->_frame_count)
      _frame = 0;
    else
      ++_frame;
    _frame_timer = 0;
  }

#ifdef DEBUG
  std::cout << _frame_timer << " " << _frame_duration << " " << deltaTime << "\n";
#endif
}

void SpritesheetRenderer::render(float x, float y)
{
  Rectangle src = {static_cast<float>(_curr->_fw * _frame),
                   0,
                   static_cast<float>(_curr->_fw),
                   static_cast<float>(_curr->_fh)};

  Rectangle dest = {x, y, static_cast<float>(_curr->_fw), static_cast<float>(_curr->_fh)};

  DrawTexturePro(_curr->_image, src, dest, Vector2{0, 0}, 0, RAYWHITE);
}

void SpritesheetRenderer::switchTo(std::string key)
{
  if (!_images.contains(key))
    return;
  _curr = _images.at(key);

  _frame       = 0;
  _frame_timer = 0;
}
