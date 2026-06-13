#pragma once

#include "audio/audio-intf.h"
#include <filesystem>
#include <raylib.h>
#include <string>
#include <unordered_map>

class Audio : IAudio
{
public:
  void loadSound(std::string key, std::filesystem::path path) override;
  void playSound(std::string key) override;

private:
  std::unordered_map<std::string, Sound>
      _sfx; // NOTE: wrap with own sound type if we ever switch libs!
};
