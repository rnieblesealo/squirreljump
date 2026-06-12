#include "audio.h"

#include <cassert>

void Audio::loadSound(std::string key, std::filesystem::path path)
{
  assert(std::filesystem::exists(path));

  if (_sfx.contains(key))
    UnloadSound(_sfx[key]);
  _sfx[key] = LoadSound(path.c_str());
}

void Audio::playSound(std::string key)
{
  assert(_sfx.contains(key));

  PlaySound(_sfx.at(key));
}
