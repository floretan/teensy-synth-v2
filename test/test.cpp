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
  playingNotes[voice] = velocity;
}
void myCustomNoteOffCallback(int voice, int note, int velocity) {
  playingNotes[voice] = 0;
}

void test_note_dispatcher_poly(void) {
  NoteDispatcher nd = NoteDispatcher(2);

  nd.setNoteOnCallback(myCustomNoteOnCallback);
  nd.setNoteOffCallback(myCustomNoteOffCallback);

  nd.pressNote(12, 123);
  TEST_ASSERT_EQUAL(playingNotes[0], 123);
  TEST_ASSERT_EQUAL(playingNotes[1], 0);
  nd.pressNote(23, 124);
  TEST_ASSERT_EQUAL(playingNotes[0], 123);
  TEST_ASSERT_EQUAL(playingNotes[1], 124);
  nd.releaseNote(12);
  TEST_ASSERT_EQUAL(playingNotes[0], 0);
  TEST_ASSERT_EQUAL(playingNotes[1], 124);
  nd.releaseNote(23);
  TEST_ASSERT_EQUAL(playingNotes[0], 0);
  TEST_ASSERT_EQUAL(playingNotes[1], 0);

  // Press more notes than voices available.
  nd.pressNote(12, 102);
  nd.pressNote(13, 103);
  nd.pressNote(14, 104);
  TEST_ASSERT_EQUAL(playingNotes[0], 104);
  TEST_ASSERT_EQUAL(playingNotes[1], 103);

  nd.releaseNote(12);
  nd.releaseNote(13);
  nd.releaseNote(14);

  // Test sustain pedal.
  nd.pressNote(12, 102);
  nd.pressSustainPedal();
  nd.pressNote(13, 103);
  nd.releaseNote(13);
  TEST_ASSERT_EQUAL(playingNotes[0], 102);
  TEST_ASSERT_EQUAL(playingNotes[1], 103);
  nd.releaseNote(12);
  TEST_ASSERT_EQUAL(playingNotes[0], 102);
  TEST_ASSERT_EQUAL(playingNotes[1], 103);
  nd.releaseSustainPedal();
  TEST_ASSERT_EQUAL(playingNotes[0], 0);
  TEST_ASSERT_EQUAL(playingNotes[1], 0);
}

void test_note_dispatcher_mono(void) {
  NoteDispatcher nd = NoteDispatcher();

  nd.setMode(NoteDispatcherMode::MONOPHONIC);

  nd.setNoteOnCallback(myCustomNoteOnCallback);
  nd.setNoteOffCallback(myCustomNoteOffCallback);

  nd.pressNote(1, 123);
  TEST_ASSERT_EQUAL(playingNotes[0], 123);

  nd.releaseNote(1);
  TEST_ASSERT_EQUAL(playingNotes[0], 0);

  nd.pressNote(1, 123);
  TEST_ASSERT_EQUAL(playingNotes[0], 123);
  nd.pressNote(2, 124);
  TEST_ASSERT_EQUAL(playingNotes[0], 124);
  nd.pressNote(3, 125);
  TEST_ASSERT_EQUAL(playingNotes[0], 125);
  nd.pressNote(4, 126);
  TEST_ASSERT_EQUAL(playingNotes[0], 126);
  nd.releaseNote(4);
  TEST_ASSERT_EQUAL(playingNotes[0], 125);
  nd.releaseNote(2);
  TEST_ASSERT_EQUAL(playingNotes[0], 125);
  nd.releaseNote(3);
  TEST_ASSERT_EQUAL(playingNotes[0], 123);
  nd.releaseNote(1);
  TEST_ASSERT_EQUAL(playingNotes[0], 0);
}

void test_note_dispatcher_repeated_note(void) {
  NoteDispatcher nd = NoteDispatcher();

  nd.setNoteOnCallback(myCustomNoteOnCallback);
  nd.setNoteOffCallback(myCustomNoteOffCallback);

  nd.pressNote(1, 123);
  TEST_ASSERT_EQUAL(playingNotes[0], 123);

  nd.pressNote(1, 124);
  // Pressing an already pressed note does nothing.
  TEST_ASSERT_EQUAL(playingNotes[0], 123);
  TEST_ASSERT_EQUAL(playingNotes[1], 0);
  nd.releaseNote(1);
  TEST_ASSERT_EQUAL(playingNotes[0], 0);
  TEST_ASSERT_EQUAL(playingNotes[1], 0);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_note_dispatcher_poly);
    RUN_TEST(test_note_dispatcher_mono);
    RUN_TEST(test_note_dispatcher_repeated_note);
    UNITY_END();

    return 0;
}
