#ifndef voice_H
#define voice_H

#include <Audio.h>
#include <vector>

using namespace std;

class Voice {
private:
  vector<AudioConnection*> patchCords;
public:
  // Keep track of the current midi note.
  int currentNote = 0;

  // These objects are public, so we can access them directly from the synth object.
  AudioSynthWaveformModulated osc1;
  AudioSynthWaveformModulated osc2;
  AudioMixer4 mixer;
  AudioEffectEnvelope env;

  Voice() {
    // Start oscillators.
    this->osc1.begin(1.0, 880, WAVEFORM_SINE);
    this->osc2.begin(1.0, 100, WAVEFORM_SAWTOOTH);

    // Connect oscillators to voice mixer.
    this->patchCords.push_back(new AudioConnection(this->osc1, 0, this->mixer, 0));
    this->patchCords.push_back(new AudioConnection(this->osc2, 0, this->mixer, 1));

    // Setup mixer.
    this->mixer.gain(0, 1.0);
    this->mixer.gain(1, 1.0);

    // Connect mixer to envelope.
    this->patchCords.push_back(new AudioConnection(this->mixer, 0, this->env, 0));
    this->env.attack(20);
    this->env.decay(100);
    this->env.sustain(0.7);
    this->env.release(300);
  };
  ~Voice() {

  };
};

#endif