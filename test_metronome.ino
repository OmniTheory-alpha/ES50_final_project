const int buzz = 6; // pin for the buzzer
 //const int initialize = A0; // pin for control button
unsigned long bpm = 0;
unsigned long time_between_beats = 0;

void setup() {
  pinMode(buzz, OUTPUT); // sets the buzzer's pin as output
  // pinMode(initialize, INPUT);
  Serial.begin(9600);
}

void loop() {
  Serial.print("BPM?"); // prompts user for bpm
  while (Serial.available() == 0) { // makes sure code doesn't do anything until user inputs something
  }
  bpm = Serial.parseInt(); // takes user input
  Serial.println(bpm);
  time_between_beats = (60 * 1000) / bpm;
  Serial.print(time_between_beats);

  while (true) {
    tone(buzz, 500);
    delay(50);
    noTone(buzz);
    delay(time_between_beats); 
  }

  /*
  tone(buzz, 500);
  delay(50);
  noTone(buzz);
  delay((60*1000)/bpm);
  */
}

/*
// function to control starting and restarting the program
int on_off (initialize) {
  if (initialize == HIGH) {
    Serial.print("BPM?: "); // prompts user for bpm
    while (Serial.available() == 0) { // makes sure code doesn't do anything until user inputs something
    }
    int bpm = Serial.parseInt(); // takes user input
    return bpm;
  }

  return bpm;
}
*/
