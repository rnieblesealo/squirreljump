#pragma once

#include <filesystem>
#include <string>

class IAudio
{
public:
  virtual void loadSound(std::string key, std::filesystem::path path) = 0;
  virtual void playSound(std::string key)                             = 0;

protected:
  inline virtual ~IAudio() = 0; // TODO: why inline?
};

IAudio::~IAudio() {} // TODO: why do we need an out of line definition?