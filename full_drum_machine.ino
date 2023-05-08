/***************************************** Drum Machine Code
This code is used for making a functional drum machine with 5 drum sounds and 8 beats in one drum loop. This drum machine is meant to be monophonic,
meaning it can only play one sound per beat.

This code contains the main function loop along with 4 other functions: changeDrumSound() which checks for whether the user pressed the button to switch 
to a different drum sound, play() which plays the sound base, buttonstate() which checks for whether a user clicked one of the eight buttons to turn a sound on or off,
and tempoAdjust() to adjust the BPM of the drum machine based on the potentiometer value that is read, and a setup() for configuration.

Pins 11, A2, A3, A4, A5, A6, 0, 1 are for the 8 user-input buttons when the user wants to turn on or off a drum sound for each beat in the drum sequence.

Pins 2, 3, 4, 5, 6, 7, 8 are for the 8 LEDs that are used to represent the state of each beat in the sequence

Pin A1 was used for input from the potentiometer which is used for setting the BPM of the drum machine.

by: Dean Kim

Date: 5/2/2023

*****************************************/
// relevant libraries
#include <SD.h>
#include <SPI.h>
#include <AudioZero.h>

// chip select for audio
const int chipSelect = 28;

// set the SD Card Root
File root;

const int changeSound = 10; // pin for button for changing sound 
int soundState = 0; // default start sound is the kick (kick, snare, hihat, openhat, clap)
int bpm = 160; // default BPM of 160
int time_between_beats = (60 * 1000) / bpm; // time between each beat

const int rows = 5; // 5 rows for the 5 drum sounds (kick, snare, hihat, openhat, clap)
const int columns = 8; // each beat in our 8 beat loop
const int beats = 8;

int buttonPin[8] = {11, A2, A3, A4, A5, A6, 0, 1}; // pins for user input buttons
int ledPin[8] = {2, 3, 4, 5, 6, 7, 8, 9}; // LED pins for showing states

// storing old and new button states for latching buttons
int buttonOld[rows][columns] = {0}; // 2D array representing each sound and 8 beats, the new button states that are just being read
int buttonNew[rows][columns] = {0}; // 2D array representing each sound and 8 beats, the old button states

// store LED state 
int LEDState[rows][columns] = {0};

// setup function
void setup() {
  // sets the changeSound pin as output
  pinMode(changeSound, INPUT); 

  // button will configure the button pin
  for (int i = 0; i < 8; i++) {
    pinMode(buttonPin[i], INPUT);
    pinMode(ledPin[i], OUTPUT);
    digitalWrite(ledPin[i], LOW);
  }
  // pinMode(initialize, INPUT);
  Serial.begin(9600);

  // wait for Serial Monitor to connect. Needed for native USB port boards only:
  while (!Serial);
  Serial.print("Initializing SD card...");

  // make sure that SD card is initialized successfully
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed. Things to check:");
    Serial.println("1. is a card inserted?");
    Serial.println("2. is your wiring correct?");
    Serial.println("3. did you change the chipSelect pin to match your shield or module?");
    Serial.println("Note: press reset or reopen this serial monitor after fixing your issue!");
    while (true);
  }
  Serial.println("initialization done.");

  // open files in root
  root = SD.open("/");
  Serial.println("done!");
}

// main loop
void loop() {
  // check for tempo adjustment from user
  tempoAdjust();
  // loop through 8 beats
  for (int i = 0; i < beats; i++) {
    // check if drum sound has been switched by user
    changeDrumSound();
    // check for button state presses
    buttonstate();
    // play sound for the beat
    play(i);
    // delay between each beat    
    delay(time_between_beats);
  }
}

// play sound for specific beat in sequence
void play(int counter) {
  // read in button state - check if button is pressed by user
  buttonNew[soundState][counter] = digitalRead(buttonPin[counter]);
  // play kick sound
  if (LEDState[0][counter] == 1) {
    AudioZero.begin(44100);
    File myFile = SD.open("KICK.wav");
    AudioZero.play(myFile);
    AudioZero.end();
    myFile.close();
  }
  // play snare sound
  else if (LEDState[1][counter] == 1) {
    AudioZero.begin(44100);
    File myFile = SD.open("SNAR.wav");
    AudioZero.play(myFile);
    AudioZero.end();
    myFile.close();
  }
  // play hi hat sound
  else if (LEDState[2][counter] == 1) {
    AudioZero.begin(44100);
    File myFile = SD.open("HH.wav");
    AudioZero.play(myFile);
    AudioZero.end();
    myFile.close();
  }
  // play open hat sound
  else if (LEDState[3][counter] == 1) {
    AudioZero.begin(44100);
    File myFile = SD.open("OH.wav");
    AudioZero.play(myFile);
    AudioZero.end();
    myFile.close();
  }
  // play clap sound
  else if (LEDState[4][counter] == 1) {
    AudioZero.begin(44100);
    File myFile = SD.open("CLAP.wav");
    AudioZero.play(myFile);
    AudioZero.end();
    myFile.close();
  }
  // if no sound is on for this beat, then just put in delay equal to length of the sound file played
  else {
    delay(287); // length of the sound exactly
  }
}

// check if user clicked to change drum sound
void changeDrumSound() {
  // check if button is pressed to change sound
  if (digitalRead(changeSound) == HIGH) {
    // we want to sound state to be cyclical, if the user reaches the last sound and presses the button again, then it just goes back to the first sound
    if (soundState < 4) {
      soundState++;
    }
    else {
      soundState = 0;
    }
    // change LED Pin setting based on the pattern that was saved for the next sound (saved states of each sound and its pattern)
    for (int i = 0; i < 8; i++) {
      if (LEDState[soundState][i] == 1) {
        digitalWrite(ledPin[i], HIGH);
      }
      else if (LEDState[soundState][i] == 0) {
        digitalWrite(ledPin[i], LOW);
      }
    }
  }
}

// check if user is changing state of one of the eight buttons by clicking it
void buttonstate() {
  for (int i = 0; i < 8; i++) {
     // if old button state is off, and new button state is on
    if (buttonOld[soundState][i] == 0 && buttonNew[soundState][i] == 1) {
      // set LED state to high, turn on that sound for that beat
      if (LEDState[soundState][i] == 0) {
        digitalWrite(ledPin[i], HIGH);
        // set LEDState to 1
        LEDState[soundState][i] = 1;
      }
      // set LED state to low, turn off that sound for that beat
      else {
        digitalWrite(ledPin[i], LOW);
        // set LEDState to 0
        LEDState[soundState][i] = 0;
      }
    }
    // the new state of the button is now the old state
    buttonOld[soundState][i] = buttonNew[soundState][i]; 
  }
}

// check for user's tempo adjustment of drum machine
void tempoAdjust() {
  // read in analog signal from potentiometer
  int sensorValue = analogRead(A1);
  // calculate BPM (min value: 1 and max value: 1023 of potentiometer)
  // update BPM value accordingly based on potentiometer
  bpm = (int)(100 + (float)400/1023 * sensorValue);
  // update time between beats
  time_between_beats = round((60 * 1000) / bpm);
}