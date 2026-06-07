#ifndef SPRITESHEET_RENDERER_H
#define SPRITESHEET_RENDERER_H
#endif

#include <raylib.h>
#include <string>
#include <unordered_map>

struct SpritesheetImage
{
  SpritesheetImage(Texture2D   &image,
                   std::string  key,
                   unsigned int rows,
                   unsigned int cols);

  Texture2D &_image; // already contains width, height

  std::string _key; // aka its name

  unsigned int _frame_count;
  unsigned int _r;
  unsigned int _c;
  unsigned int _fw;
  unsigned int _fh;
};

class SpritesheetRenderer
{
public:
  SpritesheetRenderer(std::unordered_map<std::string, SpritesheetImage const *> images,
                      unsigned int                                              fps,
                      std::string                                               defaultKey

  );
  ~SpritesheetRenderer();

  void update(double deltaTime);
  void render(float x, float y);
  void switchTo(std::string key);

private:
  SpritesheetImage const *_curr = nullptr;

  unsigned int _fps;
  double       _frame_duration;
  unsigned int _frame       = 0;
  double       _frame_timer = 0;

  std::unordered_map<std::string, SpritesheetImage const *> _images;
};
