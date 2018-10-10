#include <IRremote.h>

// Constants
#define BASE_COUNT 3
#define BLUE_LASER 0
#define RED_LASER 1
#define DEBUG_MODE false
#define MIN_DELAY 500
#define DELAY_CEIL 2500


// Define all the devices as global variables
IRrecv *irrecvs[BASE_COUNT];
int ledPins[] = {2,7,4};
int receiverPins[] = {13,8,12};

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
  initLeds();
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

void toggleSensors() {
  elapsedTime = millis() - startMillis;
  
  if (elapsedTime > interval()) {
    activeReceiver = rand() % BASE_COUNT;
    updateReceiverState();
  }
}

int interval() {
  // generate random intervel between 500ms to 2500ms
    return (rand() % DELAY_CEIL) + MIN_DELAY;
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
  unsigned int rawbuf_last = result.rawbuf[result.rawlen-1];
  
  if(rawbuf_last < 10){
    scores[BLUE_LASER]++;
    Serial.write('B');
  } else {
    scores[RED_LASER]++;
    Serial.write('R');
  }

  laserHit = true;
  turnOffReceiver(index);
  printRawbufLast(rawbuf_last);
  printHit(index);
  printScores();
}

void printScores() {
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
    String logger = "hit index: " + String(index) + " value: " + String(result.value);
    Serial.println(logger);
  }
}


