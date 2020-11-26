
class NoteDispatcher {
  using NoteCallback = void (*)(int voice, int note, int velocity);
private:
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
  // Simply play the first voice.
  this->noteOnCallback(0, note, velocity);
}

void NoteDispatcher::releaseNote(int note) {
  // Simply stop the first voice.
  this->noteOffCallback(0, note, 0);
}