#include <IRremote.h>

// Constants
#define BASE_COUNT 1
#define BLUE_LASER 0
#define RED_LASER 1
#define DEBUG_MODE true
#define MIN_DELAY 3000
#define DELAY_CEIL 4000

#define JAB_A_FRONT 0xFF629D
#define JAB_A_BACK 0x511DBB
#define SWISH_A_DOWN 0x87C6D1F
#define SWISH_A_UP 0x52A3D41F
#define SWIPE_A 0xD7E84B1B

#define JAB_B 0x32C6FDF7 //#3 in remote

unsigned int jab_count = 0;
unsigned int swish_count = 0;
unsigned int swipe_count = 0;


// Define all the devices as global variables
IRrecv *irrecvs[BASE_COUNT];
int ledPins[] = {2}; // 2, 7, 4
int receiverPins[] = {8}; //8, 13, 12

// Define all time related variables
unsigned long startMillis;
unsigned long elapsedTime;


// State variables
int activeReceiver;
bool laserHit = false;

// Store scores
int scores[] = {0,0};
decode_results result;

// methods
void setup() {
  Serial.begin(9600);
  initSensors();
//  initLeds();
}

void initSensors() {
  for (int i = 0; i < BASE_COUNT; i++) {
    irrecvs[i] = new IRrecv(receiverPins[i]);
    irrecvs[i]->enableIRIn();
  }
}

void initLeds() {
  for (int i = 0; i < BASE_COUNT; i++)
    pinMode(ledPins[i], OUTPUT);
}

void loop() {
  toggleSensors();
  if (!laserHit && irrecvs[activeReceiver]->decode(&result)) {
    recordHit(activeReceiver);
  }
}

int interval() {
  // generate random intervel between 500ms to 2500ms
    return (rand() % DELAY_CEIL) + MIN_DELAY;
}

void toggleSensors() {
  elapsedTime = millis() - startMillis;
  
  if (elapsedTime > interval()) {
    activeReceiver = rand() % BASE_COUNT;
    updateReceiverState();
  }
}

void updateReceiverState() {
  for (int i = 0; i < BASE_COUNT; i++) {
    if(i == activeReceiver)
      turnOnReceiver(i);
    else
      turnOffReceiver(i);
  }
  laserHit = false;
  startMillis = millis();
}

void turnOffReceiver(int index) {
  digitalWrite(ledPins[index], LOW);
}

void turnOnReceiver(int index) {
  digitalWrite(ledPins[index], HIGH);
  irrecvs[index]->resume();
}



void recordHit(int index) {


  switch(result.value)
  {
    case JAB_A_BACK: 
    case JAB_A_FRONT: 
      jab_count ++;
      Serial.println("jab_count");
      Serial.println(jab_count);   
      break;

    case SWISH_A_DOWN: 
    case SWISH_A_UP: 
      swish_count ++;
      Serial.println("swish_count");
      Serial.println(swish_count);
      break;

    case SWIPE_A:
      swipe_count ++;
      Serial.println("swipe_count");
      Serial.println(swipe_count);
      break;
      
    default:
      Serial.println(result.value, HEX);
      Serial.println("default value");
      break;
  }
  laserHit = true;
  turnOffReceiver(index);
  
  #if 0
  
  printRawbufLast(rawbuf_last);
  printHit(index);
  printScores();
    #endif

}

void printScores() {
  return;
  if(DEBUG_MODE) {
    Serial.println("Scores:");
    Serial.print("BLUE: ");
    Serial.print(scores[BLUE_LASER]);
    Serial.print(" RED: ");
    Serial.println(scores[RED_LASER]);
  }
}

void printRawbufLast(unsigned int rawbuf_last) {
  if(DEBUG_MODE) {
    Serial.print(" rawbuf_last: ");
    Serial.println(rawbuf_last);
  }
}

void printHit(int index) {
  if(DEBUG_MODE) {
      
     Serial.println(result.value, HEX);
//    String logger = "hit index: " + String(index) + " value: " + String(result.value);
//    Serial.println(logger);
  }
}


