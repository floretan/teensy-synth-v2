#ifndef synth_H
#define synth_H

#include "Voice.h"
#include "Frequencies.h"
#include <vector>

#define voiceCount 4
#define freqModRange 4

using namespace std;

class Synth {
private:
  AudioSynthWaveformDc noteInput;
  AudioSynthWaveformDc pitchBend; 
  AudioSynthWaveform lfo;
  AudioMixer4 modulation;

  vector<Voice*> voices;

  AudioMixer4 voicesMixer;
  AudioFilterStateVariable filter;
  AudioEffectMultiply ampMod;
  AudioEffectFreeverbStereo reverb;
  AudioMixer4 left;
  AudioMixer4 right;
  AudioOutputI2S output;

  vector<AudioConnection*> patchCords;

public:
  Synth() {
    // Define modulation sources and mix them together.
    this->patchCords.push_back(new AudioConnection(this->noteInput, 0, this->modulation, 0));
    this->patchCords.push_back(new AudioConnection(this->pitchBend, 0, this->modulation, 1));
    this->patchCords.push_back(new AudioConnection(this->lfo, 0, this->modulation, 2));

    // Define the individual synth voices.
    for (int i = 0; i < voiceCount; i++) {
      auto voice = new Voice(this->modulation);
      voices.push_back(voice);
      this->patchCords.push_back(new AudioConnection(voice->env, 0, this->voicesMixer, i));
      this->voicesMixer.gain(i, 0.25);
    }

    // Connect the output to the filter.
    this->patchCords.push_back(new AudioConnection(this->voicesMixer, 0, this->filter, 0));

    this->patchCords.push_back(new AudioConnection(this->filter, 0, this->reverb, 0));
    this->patchCords.push_back(new AudioConnection(this->filter, 0, this->left, 0));
    this->left.gain(0, 1);
    this->patchCords.push_back(new AudioConnection(this->reverb, 0, this->left, 1));
    this->left.gain(1, 1);

    this->patchCords.push_back(new AudioConnection(this->filter, 0, this->right, 0));
    this->right.gain(0, 1);
    this->patchCords.push_back(new AudioConnection(this->reverb, 1, this->right, 1));
    this->right.gain(1, 1);

    this->patchCords.push_back(new AudioConnection(this->left, 0, this->output, 0));
    this->patchCords.push_back(new AudioConnection(this->right, 0, this->output, 1));
  }
  ~Synth() {

  };

  void playNote(int voice, int note, int velocity) {
    float frequency = tune_frequencies2_PGM[note];

    this->voices[voice]->osc1.frequency(frequency);
    this->voices[voice]->osc2.frequency(frequency);
    this->voices[voice]->env.noteOn();

    float freqMod = (((note - 64) / 12.0) / freqModRange);
  }
  void releaseNote(int voice, int note, int velocity) {
    this->voices[voice]->env.noteOff();
  }
};

#endif
