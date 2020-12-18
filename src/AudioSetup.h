#ifndef audio_setup_H
#define audio_setup_H

#include <Audio.h>

AudioControlSGTL5000 audioShield;

void audio_setup() {
  AudioMemory(128);
  audioShield.enable();
  audioShield.volume(0.5);
  audioShield.surroundSoundEnable();
  audioShield.lineOutLevel(14);
}

#endif
