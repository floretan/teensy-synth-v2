#ifndef note_dispatcher_H
#define note_dispatcher_H

#include <list>
#include <string>
#include <sstream>

using namespace std;

enum NoteDispatcherMode {
  POLYPHONIC,
  MONOPHONIC,
  LEGATO,
};

class NoteDispatcher {
  using NoteCallback = void (*)(int voice, int note, int velocity, bool isFirstOrLast);

  struct NoteEntry {
    int note;
    int velocity;
    bool isPressed;
    bool isPlaying;
    int voiceIndex;
  };

private:
  int voiceCount = 8;
  list<NoteDispatcher::NoteEntry> entries;
  bool isSustainPressed = false;

  NoteCallback noteOnCallback;
  NoteCallback noteOffCallback;

  NoteDispatcherMode mode = NoteDispatcherMode::POLYPHONIC;

  bool isNotePressed(int note) {
    for (NoteDispatcher::NoteEntry& entry : this->entries) {
      if (entry.note == note && entry.isPressed) {
        return true;
      }
    }

    return false;
  }

  int getNextVoice(int note) {
    if (this->mode == NoteDispatcherMode::MONOPHONIC || this->mode == NoteDispatcherMode::LEGATO) {
      return 0;
    }

    // Find the next available voice index that isn't playing.
    for (int voiceIndex = 0; voiceIndex < this->voiceCount; voiceIndex++) {

      bool isUsed = false;
      for (auto &entry : this->entries) {
        if (entry.voiceIndex == voiceIndex && entry.isPlaying) {
          // Already used, try the next voice.
          isUsed = true;
          break;
        }
      }

      if (!isUsed) {
        return voiceIndex;
      }
    }

    // Nothing available, use the voice of the oldest used entry.
    auto entry = this->entries.front();
    entry.isPlaying = false;
    this->noteOffCallback(entry.voiceIndex, entry.note, 0, false);
    return entry.voiceIndex;
  }

public:
  NoteDispatcher(){};
  NoteDispatcher(int voiceCount)
  {
    this->voiceCount = voiceCount;
  };
  ~NoteDispatcher(){};

  void setNoteOnCallback(NoteCallback callback) {
    this->noteOnCallback = callback;
  }
  void setNoteOffCallback(NoteCallback callback) {
    this->noteOffCallback = callback;
  };

  void setMode(NoteDispatcherMode m) {
    this->mode = m;
    this->entries.clear();
  };

  void pressNote(int note, int velocity);
  void releaseNote(int note);

  void pressSustainPedal() {
    this->isSustainPressed = true;
  };
  void releaseSustainPedal() {
    this->isSustainPressed = false;

    // Remove any key which isn't currently pressed.
    for (auto entry = this->entries.begin(); entry != this->entries.end(); ) {
      if (!entry->isPressed) {
        this->noteOffCallback(entry->voiceIndex, entry->note, 0, this->entries.size() == 1);
        this->entries.erase(entry++);
      }
      else {
        ++entry;
      }
    }
  };

  string debugState() {
    stringstream output;

    for (NoteDispatcher::NoteEntry& entry : this->entries) {
      output << "Voice: " << entry.voiceIndex;
      output << " Note: " << entry.note;
      output << " Velocity: " << entry.velocity;
      output << " Pressed: " << entry.isPressed;
      output << "\n";
    }

    return output.str();
  }
};

void NoteDispatcher::pressNote(int note, int velocity) {
  if (this->isNotePressed(note)) {
    return;
  }

  if (this->mode == NoteDispatcherMode::MONOPHONIC && !this->entries.empty()) {
    // Stop the previously playing note.
    auto lastEntry = this->entries.back();
    lastEntry.isPlaying = false;
    this->noteOnCallback(lastEntry.voiceIndex, lastEntry.note, lastEntry.velocity, false);
  }

  NoteDispatcher::NoteEntry entry = NoteDispatcher::NoteEntry();
  entry.note = note;
  entry.velocity = velocity;
  entry.voiceIndex = this->getNextVoice(note);
  entry.isPressed = true;
  entry.isPlaying = true;

  this->entries.push_back(entry);
  this->noteOnCallback(entry.voiceIndex, note, velocity, this->entries.size() == 1);
}

void NoteDispatcher::releaseNote(int note) {
  for (auto entry = this->entries.begin(); entry != this->entries.end(); ) {
    if (entry->note == note) {
      if (this->isSustainPressed) {
        // Simply mark the note as not pressed.
        entry->isPressed = false;
        entry++;
      }
      else {
        this->noteOffCallback(entry->voiceIndex, entry->note, 0, this->entries.size() == 1);
        this->entries.erase(entry++);
      }
    }
    else {
      ++entry;
    }
  }

  if (this->mode == NoteDispatcherMode::MONOPHONIC || this->mode == NoteDispatcherMode::LEGATO) {
    // Go back to playing last pressed note.
    if (!this->entries.empty()) {
      auto lastEntry = this->entries.back();
      lastEntry.isPlaying = true;
      this->noteOnCallback(lastEntry.voiceIndex, lastEntry.note, lastEntry.velocity, false);
    }
  }
}

#endif
