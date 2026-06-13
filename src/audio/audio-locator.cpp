#include "audio/audio-locator.h"

#include <cassert>

Audio    *AudioLocator::_audio_service = nullptr;
NullAudio AudioLocator::_null_service;

void AudioLocator::initialize() { _audio_service = &_null_service; }

void AudioLocator::provide(Audio *audioService)
{
  if (audioService == nullptr)
    _audio_service = &_null_service;
  _audio_service = audioService;
}

Audio *AudioLocator::getAudio()
{
  assert(_audio_service != nullptr);
  return _audio_service;
}