#pragma once

#include "audio/audio.h"

class NullAudio : public Audio
{
public:
  void loadSound(std::string key, std::filesystem::path path) override;
  void playSound(std::string key) override;
};