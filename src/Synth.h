#ifndef synth_H
#define synth_H

#include "Voice.h"
#include "Frequencies.h"
#include <vector>
#include "effect_platervbstereo.h"

#define voiceCount 8
#define freqModRange 4

using namespace std;

class Synth {
private:
  float detuneAmount = 1.0;

  AudioSynthWaveformDc noteInput;
  AudioSynthWaveformDc pitchBend; 
  AudioSynthWaveform lfo;
  AudioSynthWaveform lfoOffset;
  AudioAnalyzePeak lfoPeak;
  float lastLfoPeakLevel = 0;
  AudioMixer4 modulation;

  // Mix 16 voices down to 4 mixers which are then mixed together.
  vector<Voice*> voices;
  vector<AudioMixer4*> voiceMixers;

  AudioMixer4 finalVoiceMixer;
  AudioFilterStateVariable filter;
  AudioEffectMultiply ampMod;
  AudioEffectBitcrusher bitcrusher;
  AudioEffectPlateReverb reverb;
  AudioMixer4 delayMixer;
  AudioEffectDelay delay;
  AudioMixer4 left;
  AudioMixer4 right;
  AudioOutputI2S output;

  vector<AudioConnection*> patchCords;

  void updateOscillatorParameters() {
    for (auto voice : this->voices) {
      float baseFrequency = tune_frequencies2_PGM[voice->currentNote];

      voice->osc1.frequency(baseFrequency / this->detuneAmount);
      voice->osc2.frequency(baseFrequency * this->detuneAmount);
    }
  }

public:
  Synth() {
    // Define modulation sources and mix them together.
    this->patchCords.push_back(new AudioConnection(this->noteInput, 0, this->modulation, 0));
    this->patchCords.push_back(new AudioConnection(this->pitchBend, 0, this->modulation, 1));
    this->patchCords.push_back(new AudioConnection(this->lfo, 0, this->modulation, 2));

    // Define the individual synth voices.
    for (int i = 0; i < voiceCount; i++) {
      int voiceMixerIndex = i / 4;
      if (i % 4 == 0) {
        // Add a new 4-voice mixer.
        this->voiceMixers.push_back(new AudioMixer4());
        this->patchCords.push_back(new AudioConnection(*(this->voiceMixers[voiceMixerIndex]), 0, this->finalVoiceMixer, voiceMixerIndex));
        this->finalVoiceMixer.gain(voiceMixerIndex, 4.0 / (voiceCount));
      }
      auto voice = new Voice();
      voices.push_back(voice);
      this->patchCords.push_back(new AudioConnection(this->modulation, 0, voice->osc1, 0));
      this->patchCords.push_back(new AudioConnection(this->modulation, 0, voice->osc1, 1));
      this->patchCords.push_back(new AudioConnection(this->modulation, 0, voice->osc2, 0));
      this->patchCords.push_back(new AudioConnection(this->modulation, 0, voice->osc2, 1));
      this->patchCords.push_back(new AudioConnection(voice->env, 0, *(this->voiceMixers[voiceMixerIndex]), i % 4));
      this->voiceMixers[voiceMixerIndex]->gain(i % 4, 0.25);
    }

    // Connect the mixed voices to the bitcrusher.
    this->patchCords.push_back(new AudioConnection(this->finalVoiceMixer, 0, this->bitcrusher, 0));

    // The bitcrusher is very harsh, put it before the filter.
    this->patchCords.push_back(new AudioConnection(this->bitcrusher, 0, this->filter, 0));
    this->bitcrusher.bits(16);

    this->patchCords.push_back(new AudioConnection(this->filter, 0, this->delayMixer, 0));
    this->patchCords.push_back(new AudioConnection(this->delayMixer, 0, this->delay, 0));
    this->patchCords.push_back(new AudioConnection(this->delay, 0, this->delayMixer, 1));

    this->patchCords.push_back(new AudioConnection(this->delayMixer, 0, this->reverb, 0));

    this->patchCords.push_back(new AudioConnection(this->delayMixer, 0, this->left, 0));
    this->patchCords.push_back(new AudioConnection(this->reverb, 0, this->left, 1));
    this->patchCords.push_back(new AudioConnection(this->delay, 0, this->left, 2));

    this->patchCords.push_back(new AudioConnection(this->delayMixer, 0, this->right, 0));
    this->patchCords.push_back(new AudioConnection(this->reverb, 1, this->right, 1));
    this->patchCords.push_back(new AudioConnection(this->delay, 0, this->right, 2));

    this->patchCords.push_back(new AudioConnection(this->left, 0, this->output, 0));
    this->patchCords.push_back(new AudioConnection(this->right, 0, this->output, 1));
  }
  ~Synth() {

  };

  void playNote(int voice, int note, int velocity) {
    this->voices[voice]->currentNote = note;
    this->updateOscillatorParameters();
    this->voices[voice]->env.noteOn();

    //float freqMod = (((note - 64) / 12.0) / freqModRange);
  }
  void releaseNote(int voice, int note, int velocity) {
    this->voices[voice]->env.noteOff();
  }

  void setDetune(float amount) {
    this->detuneAmount = amount;
    this->updateOscillatorParameters();
  }

  void setLfoSpeed(float frequency) {
    this->lfo.frequency(frequency);
    this->lfoOffset.frequency(frequency);
  }

  void setLfoAmplitude(float amplitude) {
    this->lfo.amplitude(amplitude);
  }

  void setBitCrush(int bits) {
    this->bitcrusher.bits(bits);
  }

  void setFilterFrequency(float frequency) {
    this->filter.frequency(frequency);
  }

  float getLfoLevel() {
    if (this->lfoPeak.available()) {
      this->lastLfoPeakLevel = this->lfoPeak.read();
    }
    return this->lastLfoPeakLevel;
  }
};

#endif
