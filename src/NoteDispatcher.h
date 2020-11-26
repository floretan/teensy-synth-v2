
class NoteDispatcher {
  using NoteCallback = void (*)(int voice, int note, int velocity);
private:
  NoteCallback noteOnCallback;
  NoteCallback noteOffCallback;
public:
  NoteDispatcher();
  ~NoteDispatcher();

  void setNoteOnCallback(NoteCallback callback);
  void setNoteOffCallback(NoteCallback callback);

  void noteOn(int note, int velocity);
  void noteOff(int note);
};

NoteDispatcher::NoteDispatcher() {
}

NoteDispatcher::~NoteDispatcher() {
}

void NoteDispatcher::setNoteOnCallback(NoteCallback callback) {
  this->noteOnCallback = callback;
}
void NoteDispatcher::setNoteOffCallback(NoteCallback callback) {
  this->noteOffCallback = callback;
}

void NoteDispatcher::noteOn(int note, int velocity) {
  // Simply play the first voice.
  this->noteOnCallback(0, note, velocity);
}

void NoteDispatcher::noteOff(int note) {
  // Simply stop the first voice.
  this->noteOffCallback(0, note, 0);
}