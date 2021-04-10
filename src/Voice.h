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
    this->osc1.begin(WAVEFORM_SAWTOOTH);
    this->osc1.amplitude(1.0);
    this->osc2.begin(WAVEFORM_SINE);
    this->osc2.amplitude(1.0);

    // Connect oscillators to voice mixer.
    this->patchCords.push_back(new AudioConnection(this->osc1, 0, this->mixer, 0));
    this->patchCords.push_back(new AudioConnection(this->osc2, 0, this->mixer, 1));

    // Setup mixer.
    this->mixer.gain(0, 0.5);
    this->mixer.gain(1, 0.5);

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