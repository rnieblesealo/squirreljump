#pragma once

#include "audio.h"
#include "audio/null-audio.h"

class AudioLocator
{
public:
  static void   initialize();
  static void   provide(Audio *audioService);
  static Audio *getAudio();

private:
  static Audio    *_audio_service;
  static NullAudio _null_service;
};
