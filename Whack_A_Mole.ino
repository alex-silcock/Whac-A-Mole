// Whack A Mole Coursework

// import the Servo library
#include <Servo.h>


// assign LEDs for each player to pins
int ledPin1[] = {A5, A4, A3};
int ledPin2[] = {6, 7, 8};
int ledPin3[] = {10, A0, 12};

// create a score for each player
int score1 = 0;
int score2 = 0;
int score3 = 0;

// assign buttons for each player to pins
int playerOneButton = 0;
int playerTwoButton = 1;
int playerThreeButton = A2;

// create a variable which holds the value of the analog output for the third player button
int playerThreeButtonValue = 0;

// declare the servo and assign it to a pin
Servo servo;
int servoPin = 4;

// assign the piezo to a pin, used for creating sounds for whether the player hits the button at the correct time or not
int piezo = 11;

// define the pin for the potentiometer, and create a variable which handles the analog value for the potentiometer, used to change the delay between LEDs coming on an off
int potPin = A1;
int valuePotentiometer = 0;

// assign the LEDs pins that light up when the player gets a point
int blueLED1 = 5;
int blueLED2 = 9;
int blueLED3 = 13;

// declare variable for a delay of 300ms
int delayTime = 300;

// declare random number variables which are used to turn on LEDs for each player
int randNumber1;
int randNumber2;
int randNumber3;

// declare variables for the state of each blue LED, used to check whether the LED is on or off 
int blueLED1state = LOW;
int blueLED2state = LOW;
int blueLED3state = LOW;


// create variables which store the time that each button was last pressed
// using unsigned long as int can only go to (2^15)-1 
unsigned long timeLastPress1 = millis();
unsigned long timeLastPress2 = millis();
unsigned long timeLastPress3 = millis();


// variable used to determine when to turn the LEDs on and off
unsigned long previousMillis = 0;

// variable which holds the number of passes of the loop() that have been completed
int pass = 0;

//setup interrupt, button input and LED outputs
void setup() { 
  Serial.begin(9600);

  // setup the servo
  servo.attach(servoPin);

  // interrupts for button presses for player one and two
  attachInterrupt(0, playerOneInput, FALLING);
  attachInterrupt(1, playerTwoInput, FALLING);

  // for loop to easily set up each LED for each player
  for (int i=0; i<3; i++){
    pinMode(ledPin1[i], OUTPUT);
    pinMode(ledPin2[i], OUTPUT);
    pinMode(ledPin3[i], OUTPUT);
  }
  
  // setup each player button 
  pinMode(playerOneButton, INPUT);
  pinMode(playerTwoButton, INPUT);
  pinMode(playerThreeButton, INPUT);

  
  // setup each blue LED
  pinMode(blueLED1, OUTPUT);
  pinMode(blueLED2, OUTPUT);
  pinMode(blueLED3, OUTPUT);

  // setup the piezo
  pinMode(piezo, OUTPUT);


}

//run main program loop
void loop() {

  // only change the value of the random variables every second pass of the loop()
  if (pass % 2 == 0) {
    randNumber1 = random(3); 
    randNumber2 = random(3); 
    randNumber3 = random(3);
  }

  // call endOfGame function if the score of any player reaches 10
  if (score1 >= 10){
    endOfGame(1);
  } 
  if (score2 >= 10) {
    endOfGame(2);
  }
  if (score3 >= 10) {
    endOfGame(3);
  }

  
  // read the value of the potentiometer
  valuePotentiometer = analogRead(potPin);
  // read the value of the P3 button
  playerThreeButtonValue = analogRead(playerThreeButton);



  // set the servo to 90 each loop
  servo.write(90);

  // determine which player has the highest score then point the servo at that player
  // if there are two or more players drawing then point the servo at nobody
  if (score1 > score2 && score1 > score3) {
    servo.write(120);
    delay(10);
  }
  else if (score2 > score1 && score2 > score3) {
    servo.write(90);
    delay(10);
  }
  else if (score3 > score1 && score3 > score2) {
    servo.write(50);
    delay(10);
  }
  else if (score1 == score2 && score2 == score3) {
    servo.write(0);
    delay(10);
  }
  else if (score1 == score2) {
    servo.write(0);
    delay(10);
  }
  else if (score2 == score3) {
    servo.write(0);
    delay(10);
  }



  // if any of the blue LEDs for each player are HIGH, change them to LOW state
  if (blueLED1state == HIGH){
    digitalWrite(blueLED1, LOW);
  } 
  if (blueLED2state == HIGH) {
    digitalWrite(blueLED2, LOW);
  }
  if (blueLED3state == HIGH) {
    digitalWrite(blueLED3, LOW);
  }


  // handles inputs for the third player
  unsigned long currentTime3 = millis();
  // if the button has been pressed, with more than a 300ms delay between each button presses, and one of the yellow LEDs are lit up
  if ((playerThreeButtonValue < 500) && (currentTime3 > timeLastPress3 + delayTime) && (digitalRead(ledPin3[randNumber3] == HIGH))) {
    digitalWrite(blueLED3, HIGH);
    timeLastPress3 = millis();
    playHitSound();
    score3 ++;
  }
  else if (playerThreeButtonValue < 500 && currentTime3 > timeLastPress3 + delayTime && digitalRead(ledPin3[randNumber3] == LOW)) {
    playMissSound();
  }


  // handles turning on the coloured LEDs for each player for when the value of the potentiometer changes
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= valuePotentiometer) {

    digitalWrite(ledPin1[randNumber1], !digitalRead(ledPin1[randNumber1]));
    digitalWrite(ledPin2[randNumber2], !digitalRead(ledPin2[randNumber2]));
    digitalWrite(ledPin3[randNumber3], !digitalRead(ledPin3[randNumber3]));

    previousMillis = currentMillis;
    pass++;
  }

  // read each blue LED to get their state
  blueLED1state = digitalRead(blueLED1);
  blueLED2state = digitalRead(blueLED2);
  blueLED3state = digitalRead(blueLED3);
}



// handles inputs for player one
void playerOneInput() {
  unsigned long currentTime = millis(); // gives the time of when the interrupt was called

  // if a red LED is high and there's been more than 300ms between button presses
  if (digitalRead(ledPin1[randNumber1]) == HIGH && currentTime > timeLastPress1 + delayTime) {
    score1 ++;
    digitalWrite(blueLED1, HIGH);
    timeLastPress1 = millis();
    playHitSound();
  }
  else {
    if (currentTime > timeLastPress1 + 100) {
      playMissSound();
    }
  }
}

// handles inputs for player two
void playerTwoInput() {
  unsigned long currentTime = millis(); // gives the time of when the interrupt was called

  // if a green LED is high and there's been more than 300ms between button presses
  if (digitalRead(ledPin2[randNumber2]) == HIGH && currentTime > timeLastPress2 + delayTime) {
    score2 ++;
    digitalWrite(blueLED2, HIGH);
    timeLastPress2 = millis();
    playHitSound();
  }
  else {
    playMissSound();
  }
}

// handles inputs for player three
void playerThreeInput() {
  unsigned long currentTime = millis(); // gives the time of when the interrupt was called

  // if a yellow LED is high and there's been more than 300ms between button presses
  if (digitalRead(ledPin3[randNumber3]) == HIGH && currentTime > timeLastPress3 + delayTime) {
    score3 ++;
    digitalWrite(blueLED3, HIGH);
    timeLastPress3 = millis();
    playHitSound();
  }
  else {
    playMissSound();
  }
}


// hit sound function
void playHitSound() {
  tone(piezo, 1000, 50);
}

// miss sound function
void playMissSound() {
  tone(piezo, 500, 50);
}

// end of game function, takes player as a parameter to determine which LEDs to light up
void endOfGame(int player) {
  // flash the LEDs 5 times once the score reaches 10
  if (player == 1) {
   for (int i=0; i<5; i++){
     digitalWrite(ledPin1[0], HIGH);
     digitalWrite(ledPin1[1], HIGH);
     digitalWrite(ledPin1[2], HIGH); 
     
     delay(100);
     
     digitalWrite(ledPin1[0], LOW);
     digitalWrite(ledPin1[1], LOW);
     digitalWrite(ledPin1[2], LOW);
     delay(100); 
   } 
 }
  else if (player == 2) {
   
   for (int i=0; i<5; i++){
     digitalWrite(ledPin2[0], HIGH);
     digitalWrite(ledPin2[1], HIGH);
     digitalWrite(ledPin2[2], HIGH); 
     
     delay(100);
     
     digitalWrite(ledPin2[0], LOW);
     digitalWrite(ledPin2[1], LOW);
     digitalWrite(ledPin2[2], LOW);
     
     delay(100); 
   }
  }
   else if (player == 3) {
   
   for (int i=0; i<5; i++){
     digitalWrite(ledPin3[0], HIGH);
     digitalWrite(ledPin3[1], HIGH);
     digitalWrite(ledPin3[2], HIGH); 
     
     delay(100);
     
     digitalWrite(ledPin3[0], LOW);
     digitalWrite(ledPin3[1], LOW);
     digitalWrite(ledPin3[2], LOW);
     
     delay(100); 
   }
}
   // reset score to 0 to continue the game
   score1 = 0;
   score2 = 0;
   score3 = 0;
   
   // reset the number of the passes of the loop() to 0
   pass = 0;
   
   // turn all LEDs off
   for (int i = 0; i < 3; i++){
    digitalWrite(ledPin1[i], LOW);
    digitalWrite(ledPin2[i], LOW);
    digitalWrite(ledPin3[i], LOW);
   }
}
