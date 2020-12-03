#ifndef audio_setup_H
#define audio_setup_H

#include <Audio.h>

AudioControlSGTL5000 audioShield;

void audio_setup() {
  AudioMemory(32);
  audioShield.enable();
  audioShield.volume(0.5);
}

#endif
