#ifndef keyboard_H
#define keyboard_H

#include "Arduino.h"

const int inputs[] = {24, 25, 26, 27, 28, 29, 30, 31};

const int multiplexAPin = 1;
const int multiplexBPin = 2;
const int multiplexCPin = 3;
const int chipSelect = 0;

const int propdelay = 10;

// Possible key states
typedef enum {
  KEY_IS_UP,
  KEY_IS_DOWN,
  KEY_IS_GOING_UP,    // We increment the timer in this state
  KEY_IS_GOING_DOWN,  // We increment the timer in this state
} state_t;

typedef struct {
  int midi_note;
  state_t state; // Bit fields
  long t; // Microsecond timestamp
} Key;

class Keyboard {
  private:

  using KeyEventCallback = void (*)(byte channel, byte note, byte velocity);
  KeyEventCallback noteOnCallback;
  KeyEventCallback noteOffCallback;

  // The interval between the first and second switch while hitting keys with the
  // highest velocity.
  const float shortestInterval = 2000.0;

  // Every time the interval between the first and second switch doubles, lower
  // the midi velocity by this much:
  const int velocityAttenuation = 20;

  // Keep track of state.
  Key keys[88];

  bool isBlackKey(byte midi_note) {
    const bool isKeyBlack[] = {0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0};
    return isKeyBlack[midi_note % 12];
  }

  void readKeys(int note_offset) {

    for (int i = 0; i < 4; i++) {

      int key_index = 3 + note_offset * 4 + i;

      bool switchA = !digitalRead(inputs[i * 2]);
      bool switchB = !digitalRead(inputs[i * 2 + 1]);

      if (switchA) {

        if (switchB && keys[key_index].state == KEY_IS_GOING_DOWN) {
          keys[key_index].state = KEY_IS_DOWN;

          unsigned long t = micros() - keys[key_index].t;
          if (t < shortestInterval) {
            t = shortestInterval;
          }

          if (isBlackKey(keys[key_index].midi_note)) {
            // Black keys are shorter, we need to compensate to get an even velocity.
            t = t*2;
          }

          int velocity = 127 - log(t / shortestInterval) / log(2) * velocityAttenuation;

          if (velocity < 0) {
            velocity = 0;
          }

          this->noteOnCallback(0, keys[key_index].midi_note, 0);
        }

        if (!switchB) {
          if (keys[key_index].state == KEY_IS_UP) {
            keys[key_index].state = KEY_IS_GOING_DOWN;
            keys[key_index].t = micros();
          }
          else if (keys[key_index].state == KEY_IS_DOWN) {
            keys[key_index].state = KEY_IS_GOING_UP;
          }
        }
      }
      else if (!switchB) { // If switchA is off, switchB should also be off, but this check avoids noise errors.

        if (keys[key_index].state == KEY_IS_DOWN ) {
          keys[key_index].state = KEY_IS_GOING_UP;
        }

        else if (keys[key_index].state == KEY_IS_GOING_UP ) {
          this->noteOffCallback(0, keys[key_index].midi_note, 0);

          keys[key_index].state = KEY_IS_UP;
        } 
      }
    }
  }

  public:

  void setNoteOnCallback(KeyEventCallback callback) {
    this->noteOnCallback = callback;
  }
  void setNoteOffCallback(KeyEventCallback callback) {
    this->noteOffCallback = callback;
  };

  void setup() {
    // put your setup code here, to run once:
    pinMode(multiplexAPin, OUTPUT);
    pinMode(multiplexBPin, OUTPUT);
    pinMode(multiplexCPin, OUTPUT);
    pinMode(chipSelect, OUTPUT);

    pinMode(4, OUTPUT);
    digitalWrite(4, LOW);

    for (int i = 0; i < 8; i++) {
      pinMode(inputs[i], INPUT_PULLUP);
    }

    // Init keys
    for ( int key = 0; key < 88; key++) {
      keys[key].midi_note = 21 + key;
      keys[key].t = 0;
      keys[key].state = KEY_IS_UP;
    }

    digitalWrite(multiplexAPin, LOW);
    digitalWrite(multiplexBPin, LOW);
    digitalWrite(multiplexCPin, LOW);
  }

  void update() {
    for (int chip = 0; chip <= 1; chip++) {
      digitalWrite(chipSelect, chip);
      // 0
      digitalWrite(multiplexCPin, LOW);
      delayMicroseconds(propdelay);
      readKeys(0 + chip * 8);

      // 1
      digitalWrite(multiplexAPin, HIGH);
      delayMicroseconds(propdelay);
      readKeys(1 + chip * 8);

      //  3
      digitalWrite(multiplexBPin, HIGH);
      delayMicroseconds(propdelay);
      readKeys(3 + chip * 8);

      // 2
      digitalWrite(multiplexAPin, LOW);
      delayMicroseconds(propdelay);
      readKeys(2 + chip * 8);

      // 6
      digitalWrite(multiplexCPin, HIGH);
      delayMicroseconds(propdelay);
      readKeys(6 + chip * 8);

      // 7
      digitalWrite(multiplexAPin, HIGH);
      delayMicroseconds(propdelay);
      readKeys(7 + chip * 8);

      // 5
      digitalWrite(multiplexBPin, LOW);
      delayMicroseconds(propdelay);
      readKeys(5 + chip * 8);

      // 4
      digitalWrite(multiplexAPin, LOW);
      delayMicroseconds(propdelay);
      readKeys(4 + chip * 8);
    }
  };
};

#endif