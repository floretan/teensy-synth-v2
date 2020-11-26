#include <unity.h>

#include "../src/NoteDispatcher.h"

/**
 * Global variable used for testing purposes.
 */
int playingNotes[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/**
 * Test callbacks.
 */
void myCustomNoteOnCallback(int voice, int note, int velocity) {
  playingNotes[note] = velocity;
}
void myCustomNoteOffCallback(int voice, int note, int velocity) {
  playingNotes[note] = 0;
}

void test_note_dispatcher(void) {
  NoteDispatcher nd = NoteDispatcher();

  nd.setNoteOnCallback(myCustomNoteOnCallback);
  nd.setNoteOffCallback(myCustomNoteOffCallback);

  nd.noteOn(1, 123);
  TEST_ASSERT_EQUAL(playingNotes[1], 123);

  nd.noteOff(1);
  TEST_ASSERT_EQUAL(playingNotes[1], 0);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_note_dispatcher);
    UNITY_END();

    return 0;
}
