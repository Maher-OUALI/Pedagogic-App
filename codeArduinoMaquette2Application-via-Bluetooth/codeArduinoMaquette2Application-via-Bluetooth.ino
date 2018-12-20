#include <Servo.h>
#include <SoftwareSerial.h>

/* Constants related to the whole project ( variables' names must be clear and understandable): */
// Constants related to LED
const byte LED[] = {2,13,4,5,3}; // list of LED pins
const int time_btwFlashes = 200;
// Constants related to Servo
const byte servo[] = {}; //list of servos pins
// Constants related to Buzzers
const byte buzzer[] = {6}; // list of buzzers pins 
const int frequency = 1000; // the frequency of toning 1KHz
// Constants related to buttons
const byte button[] = {11,12}; // list of buttons pins 
// Constants related to LDR 
const byte LDR[] = {A3}; // list of LDR components 
const int seuilHaut = 500; // luminosity > seuil Haut means a HIGH 
const int seuilBas = 100; // luminosity < seuil Bas means a LOW
// constants related to the InfraRed Module
const byte RECEIVER[] = {A5,A4}; // list of RECEIVER pins
const byte GNDIR[] = {A2,A2};    // list of GNDIR pins 
const byte VCCIR[] = {A0,A0};  // list of VCCIR pins 
const int seuilDetectionIR = 200; // distance max de détection en mm


 
// implement a new serial port
SoftwareSerial mySerial(7,8); // tx , rx

// bluetooth connection's variables:
String actuator;    
String action;
bool actuatorChoice = false; bool actionChoice = false; bool numberChoice = false; bool durationChoice = false; bool receiving = false;
// end of bluetooth connection's variables

// Variables will change:
int buttonPushCounter = 0;   // counter for the number of button presses
int buttonState = 1;         // current state of the button
int lastButtonState = 0;     // previous state of the button


// set up a servo-type variable
Servo myServo;
int lastPos[] = {0}; // same number as servos list

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
}

void loop() {
  if (mySerial.available()>0) { 
      receiving = true;
      char msgRecieved = mySerial.read();
      if(msgRecieved == '@') {actuatorChoice = true; durationChoice = false;  }  // ASCII code of @
      if(msgRecieved == '#') {numberChoice = true; actuatorChoice = false; }  // ASCII code of #
      if(msgRecieved == '>') {actionChoice = true; numberChoice = false; }  // ASCII code of >
      if(msgRecieved == '%') {durationChoice = true; actionChoice = false; }  // ASCII code of %
      if(msgRecieved > 47 and msgRecieved < 58) {if(numberChoice == true){actuator += (String)msgRecieved;
                                                                          }
                                                 if(durationChoice == true){action += (String)msgRecieved;
                                                                           }
                                               }  // ASCII code of a number
       /* each letter in the sector below stands for something either an actuator, a sensor or an action;
        L : LED, actions related to LED are :- AX for "Allumer" with X the amount of time in seconds 
                                             - CX for "Clignoter" with X the amount of blinks knowing that the time of a blink is fixed by the team
        S : Servo, actions related to ServoMotors are:- TX for "Tourner" with X the angle of turning that is between 0 and 360( could change later)
        B : Buzzer, actions related to Buzzers are:- SX for "Sonner" with X the amount of time in seconds 

        now when it comes to sensors, when the arduino card receives the message, it waits until the sensor accomplish the mission i.e detects required variations and the sends back a 
        confirmation message which is a "1", if the sensor spends more than "n" seconds without accomplishing that mission the card will send an error message to arduino app which is a "0"
        ("n" will be determined later >10)
        P : Poussoir(Button), missions required from a button are:- CX for "Cliquer" with X the number of clicks 
        I : InfraRed, missions required from an InfraRed sensor are:- P for "Passer"
        D : light Diode resistance, missions required from a LDR are:- LB for "Luminosité" with B is a bit (0 for LOW luminosity and 1 for HIGH luminosity)
        */
      if((msgRecieved == 'L' or msgRecieved == 'S' or msgRecieved == 'B' or msgRecieved == 'P' or msgRecieved == 'I' or msgRecieved == 'D' ) and actuatorChoice == true ) {actuator += (String)msgRecieved;} 
      if((msgRecieved == 'A' or msgRecieved == 'C' or msgRecieved == 'S' or msgRecieved == 'T' or msgRecieved == 'P' or msgRecieved == 'L' ) and actionChoice == true ) {action.concat((String)msgRecieved);}
 }
 if (mySerial.available()==0 and receiving == true){
 switch(actuator[0]){
  case 'L' : // LED   
   switch(action[0]){
    case 'A': // Lighting Up (example message sent by the app @L#13>A%9)
        lightUpLED(actuator.substring(1).toInt(),action.substring(1).toInt());
    break;
    
    case 'C': // Blinking (example message sent by the app @L#5>C%7)
        blinkLED(actuator.substring(1).toInt(),action.substring(1).toInt());
    break;

    default:
        mySerial.write("transmission error");
    break;
   }
  break;
  
  case 'S': // Servo  (example message sent by the app @S#8>T%90) 
    switch(action[0]){
      case 'T':
        turnServo(actuator.substring(1).toInt(),action.substring(1).toInt());
      break;

      default:
        mySerial.write("transmission error");
      break;
    }  
  break;

  case 'B': // Buzzer  (example message sent by the app @B#4>S%9)
    switch(action[0]){
      case 'S':
        toneBuzzer(actuator.substring(1).toInt(),action.substring(1).toInt());  
      break;

      default:
        mySerial.write("transmission error");
      break;
    }
  break;

  case 'P': // Button  (example message sent by the app @P#1>C%3)
    switch(action[0]){ 
      case 'C':
        clickButton(actuator.substring(1).toInt(), action.substring(1).toInt());
      break;

      default:
        mySerial.write("transmission error");
      break;
    }
  break;

  case 'I': // InfraRed  (example message sent by the app @I#13>P%anything because it won't matter)
     switch(action[0]){
      case 'P':   
        detectInfraRed(actuator.substring(1).toInt());
        delay(10);
      break;
      
      default:
        mySerial.write("transmission error");
      break;
     }
  break;

  case 'D': // PhotoDiodeResistance   (example message sent by the app @D#13>L%1/0 for HIGH or LOW)
    switch(action[0]){
      case 'L':
        Serial.println("true");
        detectLuminosity(actuator.substring(1).toInt(),action.substring(1).toInt());
        delay(10);
      break;

      default:
        mySerial.write("transmission error");
      break;
    }
  break;

  default: // Transmission ERROR (any other character different from the list above )
        mySerial.write("Transmission Error");
        delay(10);
  break;
 }
 receiving = false;
 actuator = "";
 action = "";}
 delay(10); // this delay is important because it lets us wait till the buffer is empty so that we don't get "crazy" values in "actuator" and "action" variables ^^
}

// functions 
void lightUpLED(int actuator, int action){
        pinMode(LED[actuator-1],OUTPUT); // set the operator mode of the LED with the correspondant pin number recieved within the message...
        digitalWrite(LED[actuator-1],HIGH);
        if(action != 0){ 
          delay(action);
          digitalWrite(LED[actuator-1],LOW); 
          delay(10);
        }
        mySerial.write("1"); // after the accomplishement of a function the arduino sends back to android app a "1" 
}

void blinkLED(int actuator, int action){
        pinMode(LED[actuator-1],OUTPUT); // set the operator mode of the LED with the correspondant pin number recieved within the message... 
        for (int i=1; i <= action; i++){
        digitalWrite(LED[actuator-1],HIGH); 
        delay(time_btwFlashes);
        digitalWrite(LED[actuator-1],LOW);
        delay(time_btwFlashes);
          } 
        mySerial.write("1");
        delay(10);
}

void turnServo(int actuator, int action){
        myServo.attach(servo[actuator-1]); // attach the servo to the correspondant pin number recieved within the message...
        for (int position = 0; position <= action; position++) {
        myServo.write(position+lastPos[actuator - 1]);
        delay(10);
           }
        mySerial.write("1");
        myServo.detach(); // detach the servo so that it can be used again ...
        lastPos[actuator - 1] += action;
}

void toneBuzzer(int actuator, int action){
        pinMode(buzzer[actuator-1],OUTPUT); // set the operator mode of the buzzer with the correspondant pin number recieved within the message...
        for (int position = 0; position <= action; position++) {
            // play a note on pin 6 for 200 ms:
             tone(buzzer[actuator-1], random(20,1000), 200);
             delay(200);
             // play a note on pin 7 for 500 ms:
             tone(buzzer[actuator-1], random(20,1000), 500);
             delay(500);
             // play a note on pin 8 for 300 ms:
             tone(buzzer[actuator-1], random(20,1000), 300);
             delay(300);
             noTone(buzzer[actuator-1]);
}
        mySerial.write("1");
        delay(10); 
}

void clickButton(int actuator, int action){
        pinMode(button[actuator-1],INPUT_PULLUP); // set the operator mode of the button with the correspondant pin number recieved within the message...
        while(buttonPushCounter < action){
            buttonState = digitalRead(button[actuator-1]);
            // compare the buttonState to its previous state
            if (digitalRead(button[actuator-1]) != lastButtonState) {
            // if the state has changed, increment the counter
            buttonState = digitalRead(button[actuator-1]);
            if (buttonState == HIGH) {
              // if the current state is HIGH then the button went from off to on:
              buttonPushCounter++;
          } else {
              // if the current state is LOW then the button went from on to off:
          }
           // Delay a little bit to avoid bouncing
           delay(100); // this delay is very important due to button pushs sensitivity
          }
          // save the current state as the last state, for next time through the loop
          lastButtonState = buttonState;  
        }
        mySerial.write("1");
        buttonPushCounter = 0;   
}

void detectLuminosity(int actuator, int action){
  int tension;
  if(action == 1 ){  // detect HIGH Luminosity
    tension = analogRead (LDR[actuator-1]) ;
    while(tension < seuilHaut){
      tension = analogRead(LDR[actuator-1]);
    }
    mySerial.write("1");
  }
  else{  //detect LOW Luminosity
    tension = analogRead (LDR[actuator-1]) ;
    while(tension > seuilBas){
      tension = analogRead(LDR[actuator-1]);
    }
    mySerial.write("1");
  }
}

void detectInfraRed(int actuator){
  // set the operator mode of pins   VCCIR / GNDIR /RECEIVER
  pinMode(RECEIVER[actuator-1],INPUT);
  pinMode(GNDIR[actuator-1],OUTPUT);
  pinMode(VCCIR[actuator-1],OUTPUT);
  // start the emmission of IR
  digitalWrite(VCCIR[actuator-1],HIGH);
  digitalWrite(GNDIR[actuator-1],LOW);
  // check the contact
  while(analogRead(RECEIVER[actuator-1]) > seuilDetectionIR){
  Serial.println(analogRead(RECEIVER[actuator-1]));
  delay(10);
   }
    mySerial.write("1");
}

