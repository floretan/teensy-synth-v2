#include "Arduino.h"
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>

#include "AudioSetup.h"
#include "Keyboard.h"
#include "NoteDispatcher.h"
#include "Synth.h"

Keyboard keyboard = Keyboard();
NoteDispatcher nd = NoteDispatcher();
Synth synth = Synth();

// Callbacks to handle midi note events.
void midiNoteOn(byte channel, byte note, byte velocity) {
  nd.pressNote(note, velocity);
}
void midiNoteOff(byte channel, byte note, byte velocity) {
  nd.releaseNote(note);
}
void midiControlChange(byte channel, byte control, byte value) {
  switch (control) {
    // Mod wheel.
    case 1:
      synth.setDetune(1.0 + 0.01 * value / 127);
      break;

    // Sustain pedal.
    case 64:
      if (value == 127) {
      nd.pressSustainPedal();
      }
      else {
        nd.releaseSustainPedal();
      }
      break;

  default:
    Serial.print("CC: ");
    Serial.print(control);
    Serial.print(":");
    Serial.println(value);
    break;
  }
}

// Callback to handle notes being played and released.
void playNote(int voice, int note, int velocity, bool isFirstOrLast) {
  synth.playNote(voice, note, velocity, isFirstOrLast);
}
void releaseNote(int voice, int note, int velocity, bool isFirstOrLast) {
  synth.releaseNote(voice, note, velocity, isFirstOrLast);
}

void setup(void) {
  Serial.begin(9600);

  // Setup midi note callbacks.
  usbMIDI.setHandleNoteOn(midiNoteOn);
  usbMIDI.setHandleNoteOff(midiNoteOff);
  usbMIDI.setHandleControlChange(midiControlChange);

  // Setup callbacks for the synth.
  nd.setNoteOnCallback(playNote);
  nd.setNoteOffCallback(releaseNote);

  audio_setup();

  keyboard.setup();
  keyboard.setNoteOnCallback(midiNoteOn);
  keyboard.setNoteOffCallback(midiNoteOff);
}

void loop() {
   while (usbMIDI.read()) {
     delay(1);
  }
  keyboard.update();

  analogWrite(LED_BUILTIN, 255 * pow(synth.getLfoLevel(), 2));
}
