#include <list>

using namespace std;

class NoteDispatcher {
  using NoteCallback = void (*)(int voice, int note, int velocity);
private:
  list<int> pressedNotes;
  NoteCallback noteOnCallback;
  NoteCallback noteOffCallback;
public:
  NoteDispatcher(){};
  ~NoteDispatcher(){};

  void setNoteOnCallback(NoteCallback callback) {
    this->noteOnCallback = callback;
  }
  void setNoteOffCallback(NoteCallback callback) {
    this->noteOffCallback = callback;
  };

  void pressNote(int note, int velocity);
  void releaseNote(int note);
};

void NoteDispatcher::pressNote(int note, int velocity) {
  this->pressedNotes.push_back(note);

  // Simply play the first voice.
  this->noteOnCallback(0, note, velocity);
}

void NoteDispatcher::releaseNote(int note) {
  for (list<int>::iterator it=this->pressedNotes.begin(); it != this->pressedNotes.end(); it++) {
    int pressedNote = *it;

    if (pressedNote == note) {
      this->pressedNotes.erase(it);

      this->noteOffCallback(0, note, 0);
      break;
    }
  }
}