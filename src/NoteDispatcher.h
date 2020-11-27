#ifndef note_dispatcher_H
#define note_dispatcher_H

#include <list>

using namespace std;

enum NoteDispatcherMode {
  POLYPHONIC,
  MONOPHONIC,
};

class NoteDispatcher {
  using NoteCallback = void (*)(int voice, int note, int velocity);

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
    if (this->mode == NoteDispatcherMode::MONOPHONIC) {
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
    this->noteOffCallback(entry.voiceIndex, entry.note, 0);
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
};

void NoteDispatcher::pressNote(int note, int velocity) {
  if (this->isNotePressed(note)) {
    return;
  }

  if (this->mode == NoteDispatcherMode::MONOPHONIC && this->entries.size() > 0) {
    // Stop the previously playing note.
    auto lastEntry = this->entries.back();
    lastEntry.isPlaying = false;
    this->noteOnCallback(lastEntry.voiceIndex, lastEntry.note, lastEntry.velocity);
  }

  NoteDispatcher::NoteEntry entry = NoteDispatcher::NoteEntry();
  entry.note = note;
  entry.velocity = velocity;
  entry.voiceIndex = this->getNextVoice(note);
  entry.isPressed = true;
  entry.isPlaying = true;

  this->entries.push_back(entry);
  this->noteOnCallback(entry.voiceIndex, note, velocity);
}

void NoteDispatcher::releaseNote(int note) {
  for (auto it = this->entries.begin(); it != this->entries.end(); ) {
    auto entry = *it;
    if (entry.note == note) {
      this->noteOffCallback(entry.voiceIndex, entry.note, 0);
      this->entries.erase(it++);
    }
    else {
      ++it;
    }
  }

  if (this->mode == NoteDispatcherMode::MONOPHONIC) {
    // Go back to playing last pressed note.
    auto lastEntry = this->entries.back();
    lastEntry.isPlaying = true;
    this->noteOnCallback(lastEntry.voiceIndex, lastEntry.note, lastEntry.velocity);
  }
}

#endif
