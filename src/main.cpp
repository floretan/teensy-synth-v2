#include "Arduino.h"
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>

#include "AudioSetup.h"
#include "NoteDispatcher.h"
#include "Synth.h"

NoteDispatcher nd = NoteDispatcher();
Synth synth = Synth();

// Callbacks to handle midi note events.
void midiNoteOn(byte channel, byte note, byte velocity) {
  nd.pressNote(note, velocity);
}
void midiNoteOff(byte channel, byte note, byte velocity) {
  nd.releaseNote(note);
}

// Callback to handle notes being played and released.
void playNote(int voice, int note, int velocity) {
  synth.playNote(voice, note, velocity);
}
void releaseNote(int voice, int note, int velocity) {
  synth.releaseNote(voice, note, velocity);
}

void setup(void) {
  // Setup midi note callbacks.
  usbMIDI.setHandleNoteOn(midiNoteOn);
  usbMIDI.setHandleNoteOff(midiNoteOff);

  // Setup callbacks for the synth.
  nd.setNoteOnCallback(playNote);
  nd.setNoteOffCallback(releaseNote);

  audio_setup();
}

void loop() {
   while (usbMIDI.read()) {
     delay(1);
  }
}


