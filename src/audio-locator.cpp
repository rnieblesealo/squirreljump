#include "audio-locator.h"

#include <cassert>

Audio *AudioLocator::_audio_service = nullptr;

Audio *AudioLocator::getAudio()
{
  assert(_audio_service != nullptr);
  return _audio_service;
}

void AudioLocator::provide(Audio *audioService) { _audio_service = audioService; }
