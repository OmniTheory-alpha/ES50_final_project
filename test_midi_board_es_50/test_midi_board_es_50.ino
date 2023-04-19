// code from GENED 1080 Lab 7 Fall 2022 was used in order to set up the keyboard

#include <MIDI.h> // including the MIDI library

// for the synth later
const int pitch = 0; // anything from -4 to 4
const int volume = 127; // anything from 0 to 127

MIDI_CREATE_DEFAULT_INSTANCE(); // creates and binds the MIDI interface to the default hardware Serial port

// defining fixed variables
const int num_keys = 12; // 12-key keyboard (will change if we get another octave)
const int ports[num_keys] = {A1, A2, A3, A4, A5, A6, 0, 1, 2, 3, 4, 5}; // input ports on the arduino that correspond to each key

bool note_state[num_keys] = {0}; // array to keep track of which notes are playing (1 = true) and which aren't (0 = false)

void setup() {
  // defining pinMode
  for (int i = 0; i < num_keys; i++) {
    pinMode(ports[i], INPUT);
  }

  MIDI.begin();
}

void loop() {
  for (int i = 0; i < num_keys; i++) {
    if(read_button_state(ports[i], 10)) { // checks if the button is being pressed
      if (!note_state[i]) { // check if the note is not playing
        note_state[i] = true; // set the note_state[i] to true (that is, "the ith note is playing") 
        MIDI.sendNoteOn(60 + i + 12*pitch, volume, 1);
      }
    }
    else { // if the button isn't being pressed
      if (note_state[i]) { // checks if the note is playing
        note_state[i] = false; // set the note_state[i] to false (that is, "the ith note is not playing")
        MIDI.sendNoteOff(60 + i + 12*pitch, volume, 1); // stops the sound
      }
    }
  }

}

// this function just helps us read the state of each button, hence the name button_state
bool read_button_state(int button_pin, int button_delay) {
  bool button_state = digitalRead(button_pin); // reads the state of the button
  if (button_state == HIGH) { // if the button_state is high, return true
    return true;
  }

}
