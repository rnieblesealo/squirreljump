#pragma once

#include <filesystem>
#include <raylib.h>
#include <string>
#include <unordered_map>

class Audio
{
public:
  void loadSound(std::string key, std::filesystem::path path);
  void playSound(std::string key);

private:
  std::unordered_map<std::string, Sound>
      _sfx; // NOTE: wrap with own sound type if we ever switch libs!
};
