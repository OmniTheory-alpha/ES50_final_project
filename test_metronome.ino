#include <LiquidCrystal.h>

// initializing LCD pins with the arduino pin it's associated with
const int rs = 10, en = 8, db4 = 5, db5 = 4, db6 = 3, db7 = 2;
LiquidCrystal lcd(rs, en, db4, db5, db6, db7);

const int buzz = 6; // pin for the buzzer
const int bpm_button = 7; // pin for control button

int time_between_beats = 0; // the time between beats
const int click_duration = 50; // the actual duration of the metronome click in milliseconds

const int number_of_digits = 3; // just in case we want to have higher bpms
int bpm_array[number_of_digits] = {0, 0, 0}; // each place corresponds with a digit of the bpm (on the actual screen, the digits are at (0,6), (0.7), (0,8))
int integer_bpm = 0; // the actual bpm in integer form 
String string_bpm = ""; // bpm in string form for easy digit concatenation

unsigned long press_duration = 0; // this will keep track of how long the button has been pressed
bool bpm_pressed = false; // variable to keep track of if the bpm button has been pressed
unsigned long start_time = 0; // time when button was pressed
unsigned long end_time = 0; // time when button was released

void setup() {
  Serial.begin(9600);

  pinMode(buzz, OUTPUT); // sets the buzzer's pin as output
  pinMode(bpm_button, INPUT); // sets the button's pin as input

  lcd.begin(16, 2); // we're using 16x2 display (with extras)
  lcd.print("BPM: "); // replace with "BPM: "
}

void loop() {
  for (int i = 0; i < number_of_digits; i++) { // iterating through each of the digits in the bpm array
    // reset the variables
    bpm_pressed = false;
    press_duration = 0;
    start_time = 0;
    end_time = 0;

    lcd.setCursor(7 - i, 0); // sets the cursor to the current location
    lcd.cursor(); // displays cursor

    while (press_duration < 1000) {
      // resetting variables (if we come back here, then that means we've gone through a short press or nothing has happened; either way, we need to reset)
      bpm_pressed = false;
      press_duration = 0;
      start_time = 0;
      end_time = 0;

      if (digitalRead(bpm_button) == HIGH) { // checking that we're pressing the bpm button
        bpm_pressed = true; // confirming we're pressing the bpm_button
        start_time = millis(); // checking the time the program has been running at the moment the button is pressed

        // debugging stuff
        Serial.print("Start Time: ");
        Serial.println(start_time);
        Serial.println("Button pressed!");

        // wait until the button has been released before doing anything to avoid Arduino from reading HIGH more than once
        while (digitalRead(bpm_button) == HIGH) {
        }
      }
      
      if (digitalRead(bpm_button) == LOW && bpm_pressed == true) { // checking that the button has been released, not just that it's off
        end_time = millis(); // checking the time the program has been running the moment the button is released

        // debugging stuff
        Serial.print("End Time: ");
        Serial.println(end_time);
        Serial.println("Button released!");
      }

      if (end_time != 0 && start_time != 0) { // this should only be happening while the other two are not 0; otherwise, weirdness can happen with negative numbers
        press_duration = end_time - start_time; // finds the difference between the two reference times

        // debugging stuff
        Serial.print("Press Duration: ");
        Serial.println(press_duration);
      }

      if (press_duration < 1000 && digitalRead(bpm_button) == LOW && bpm_pressed == true) { // this is a short press, so the digit will change (bpm_button must be LOW so this only triggers on release)
        bpm_array[i]++;

        if (bpm_array[i] > 9) { // if the digit would go over 9, set it back to 0
          bpm_array[i] = 0;
        }

        // debugging stuff
        Serial.print("Current digital value: ");
        Serial.println(bpm_array[i]);
        Serial.println("Short press!");

        // print value to screen and keep the cursor in place
        lcd.print(bpm_array[i]);
        lcd.setCursor(7 - i, 0);
      }

      if (press_duration >= 1000) { // millis is calculated in milliseconds; this is a long press
        // the buzzer makes a noise to indicate a long press has happened
        tone(buzz, 500);
        delay(2 * click_duration); // duration is largerly arbirary here; just made it double the click_duration to make it distinguishable
        noTone(buzz);      
      }
      
    }
  }

  // now that the array bpm is sorted out, we need to put the digits together in an actual number
  // the the easiest method is turning each of the values to characters, concatenating them, then turning the result into an integer
  for (int i = 0; i < number_of_digits; i++) {
    string_bpm += String(bpm_array[number_of_digits - 1 - i]);
  }
  integer_bpm = string_bpm.toInt();

  // debugging stuff
  Serial.print("Integer BPM: ");
  Serial.println(integer_bpm);

  /*  
  weird issue here: take a theoretical 60 BPM metronome. With a delay of 50 (that is, the note lasts for 50 ms), the beat will take up 300 ms in total over every minute.
  this is actually noticeable, so to fix this, we need to adjust for this lag by subtracting the time_between_beats by the click_length
  */

  time_between_beats = ((60 * 1000) / integer_bpm) - click_duration;
  Serial.print("Time Between Beats: ");
  Serial.println(time_between_beats);

  // playing the metronome
  while (true) {
    tone(buzz, 500);
    delay(click_duration);
    noTone(buzz);
    delay(time_between_beats);  
  }
}
