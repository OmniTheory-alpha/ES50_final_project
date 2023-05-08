// code from GENED 1080 Lab 7 Fall 2022 was used in order to set up the keyboard

#include <MIDI.h> // including the MIDI library
#include <LiquidCrystal.h> // including library for LCD

// for the synth later
const int pitch = 0; // anything from -4 to 4
const int volume = 127; // anything from 0 to 127

MIDI_CREATE_DEFAULT_INSTANCE(); // creates and binds the MIDI interface to the default hardware Serial port

// defining fixed variables
const int num_keys = 12; // 12-key keyboard (will change if we get another octave)
const int ports[num_keys] = {A1, A2, A3, A4, A5, A6, 0, 1, 2, 3, 4, 5}; // input ports on the arduino that correspond to each key

bool note_state[num_keys] = {0}; // array to keep track of which notes are playing (1 = true) and which aren't (0 = false)

// stuff for displaying to lcd
const int rs = 10, en = 8, db4 = 11, db5 = 12, db6 = 13, db7 = 14;
LiquidCrystal lcd(rs, en, db4, db5, db6, db7);

const int num_notes = 12;
String notes[num_notes] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"}; // order corresponds with the order of the ports array on line 14
String current_note = ""; // the current note being played
String lcd_notes = ""; // list of notes that are being displayed to the lcd screen
int number_of_notes_playing = 0; // keeps track of how many notes are being played (could be a bool, but this is more flexible)

void setup() {
  // defining pinMode
  for (int i = 0; i < num_keys; i++) {
    pinMode(ports[i], INPUT);
  }

  MIDI.begin();

  // LCD board setup
  lcd.begin(16, 2);
  lcd.print("Note: ");
  lcd.setCursor(6, 0);
  lcd.cursor();
}

void loop() {
  for (int i = 0; i < num_keys; i++) {
    if(read_button_state(ports[i], 10)) { // checks if the button is being pressed
      if (!note_state[i]) { // check if the note is not playing
        note_state[i] = true; // set the note_state[i] to true (that is, "the ith note is playing") 
        MIDI.sendNoteOn(60 + i + 12*pitch, volume, 1);

        number_of_notes_playing++;
        
        // for debugging purposes
        Serial.print("Number of notes playing: ");
        Serial.println(number_of_notes_playing);

        Serial.print("Current i: ");
        Serial.println(i);

        if (number_of_notes_playing > 1) {
          Serial.println("ith note (> 1): " + notes[i]); // debugging
          lcd_notes += "," + notes[i]; // adds the note to the LCD Notes string
          Serial.println("LCD Notes: " + lcd_notes); // debugging
          lcd.clear();
          lcd.print("Note: " + lcd_notes); // prints out the current LCD Notes string to the LCD screen
        }
        else if (number_of_notes_playing == 1) {
          Serial.println("ith note (== 1): " + notes[i]); // debugging
          lcd_notes += notes[i]; // 
          Serial.println("LCD Notes: " + lcd_notes); // debugging
          lcd.clear();
          lcd.print("Note: " + lcd_notes); // prints out the current LCD Notes string to the LCD screen
        }

        /*
          we need to display the note being played, which is slightly more involved than one would initially think because this is keyboard is polyphonic
          first we check if the note is being played (which is done above)
          then, we need to check if there are other notes being played (this can be done with some sort of counter that ticks up and down depending on what is clicked)
            -if other notes are being played, we need to print a comma before the note
            -if no other notes are being played, we just print the note
          after printing a note, the cursor position should be right after everything printed (this should happen by default, but I don't know for sure)
          in addition, we'll need a string that represents what is actually being printed to the screen and concatenate notes[i] to that string
        */        
      }
    }
    else { // if the button isn't being pressed
      if (note_state[i]) { // checks if the note is playing
        note_state[i] = false; // set the note_state[i] to false (that is, "the ith note is not playing")
        number_of_notes_playing--;
        MIDI.sendNoteOff(60 + i + 12*pitch, volume, 1); // stops the sound

        String cut_note = notes[i]; // note that we're cutting
        char first_character = cut_note[0]; // first letter of the note we're cutting
        bool sharp = false; // the note we're looking at is a sharp, true or false?
        
        // these are the symbols we are looking for when parsing the LCD Notes string
        String sharp_symbol = "#";
        const char * comma = ",\0";

        for (int j = 0; j < lcd_notes.length(); j++) {
          if (lcd_notes[j] == first_character) { // loops until we find the character we're looking for (there may be multiple contenders)

            /*
            for debugging purposes: 

            Serial.print("LCD Notes[j + 1]: ");
            Serial.println(lcd_notes[j + 1]);

            Serial.print("Sharp Symbol: ");
            Serial.println(sharp_symbol);

            if (String(lcd_notes[j + 1]).compareTo(sharp_symbol) == 0) { // checking if the next character is #
              sharp = true; 
              Serial.println("Sharp!");
            }
            else {
              sharp = false;
              Serial.println("No sharp!");
            }
            */

            if (cut_note.length() > 1 && String(lcd_notes[j + 1]).compareTo(sharp_symbol) == 0) { // we're cutting a sharp and it's what we're looking for!
              if (strcmp(lcd_notes[j - 1] + "\000", comma) != 0) { // this means it is at the beginning of the string
                Serial.println("Beginning of string!");
                delay(50);
                lcd_notes.remove(0, 3); // removing the first three letters, which are the note, #, and the comma
                Serial.print("New LCD Notes: ");
                Serial.println(lcd_notes);
              }
              else { // if it isn't at the beginning, then the process is identical
                lcd_notes.remove(j - 1, 3); // removing the comma before, the note, and the #
              }
              lcd.clear();
              lcd.print("Note: " + lcd_notes);
            }
            else if (cut_note.length() == 1 && String(lcd_notes[j + 1]).compareTo(sharp_symbol) != 0) { // we're cutting a normal note and we've found it!
              // check for comma, perform the cut and print to lcd
              if (strcmp(lcd_notes[j - 1] + "\0", comma) != 0) { // this means it is at the beginning of the string
                Serial.println("Beginning of string!");
                delay(50);
                lcd_notes.remove(0, 2); // removing the first two letters, which are the note, #, and the comma
                Serial.print("New LCD Notes: ");
                Serial.println(lcd_notes);
              }
              else { // if it isn't at the beginning, then the process is identical
                lcd_notes.remove(j - 1, 2); // removing the comma before and the note
              }
              lcd.clear();
              lcd.print("Note: " + lcd_notes);
            }
          }
        }

        // we want to do a hard reset to avoid characters escaping
        if (number_of_notes_playing == 0) {
          lcd.clear();
          lcd_notes = "";
          lcd.print("Note: " + lcd_notes);

          Serial.print("LCD Notes after Reset: ");
          Serial.println(lcd_notes);
        }
      }
    }
  }

}

// this function just helps us read the state of each button, hence the name button_state
bool read_button_state(int button_pin, int button_delay) {
  // Serial.print("Reading button ");
  // Serial.print(button_pin);
  // Serial.print(" with state ");
  bool button_state = digitalRead(button_pin); // reads the state of the button
  // Serial.println(button_state);
  if (button_state == HIGH) { // if the button_state is high, return true
    delay(button_delay);
    button_state = digitalRead(button_pin); // reads the state of the button again
    if (button_state == HIGH) {
      return true; // okay, it really was pressed then      
    }
  }
  else {
    return false; // button was not really pressed
  }
}
