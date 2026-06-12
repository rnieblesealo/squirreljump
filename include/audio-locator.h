#pragma once

#include "audio.h"

class AudioLocator
{
public:
  static Audio *getAudio();
  static void   provide(Audio *audioService);

private:
  static Audio *_audio_service; // TODO: use null object
};
