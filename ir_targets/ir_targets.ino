#include <IRremote.h>

// Constants
#define BASE_COUNT 1
#define BLUE_LASER 0
#define RED_LASER 1
#define DEBUG_MODE true
#define MIN_DELAY 3000
#define DELAY_CEIL 4000


// Define all the devices as global variables
IRrecv *irrecvs[BASE_COUNT];
int ledPins[] = {2, 7}; // 2, 7, 4
int receiverPins[] = {8}; //8, 13, 12

// Define all time related variables
unsigned long startMillis;
unsigned long elapsedTime;
unsigned long totalSpellCount = 0;


// State variables
int activeReceiver;
bool laserHit[BASE_COUNT] = {false};

// Store scores
int scores[] = {0,0};
long int values[] = {0,0};

decode_results results[BASE_COUNT];

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

  for (int i = 0; i < BASE_COUNT; i++) {
    if (!laserHit[i] && irrecvs[i]->decode(&results[i])) {
     totalSpellCount++;
     recordHit(i);
//     delay(1000);
    }
    if (laserHit[i]) {
      laserHit[i] = false;
    }
  }
  
}

int interval() {
  // generate random intervel between 500ms to 2500ms
    return (rand() % DELAY_CEIL) + MIN_DELAY;
}

bool isdoneSleeping() {
  elapsedTime = millis() - startMillis;
  return elapsedTime > interval();
}

void recordHit(int index) {
//  Serial.print("this is from sensor: ");
//  Serial.print(index);
//  Serial.print("\n");


  if (results[index].value != 0xFFFFFFFF) {
    Serial.println(results[index].value, DEC);

    int wandCode = totalSpellCount%2;
    values[wandCode] = results[index].value;
    if (values[0] > values[1]) {
        scores[0]++;
      } else {
        scores[1]++;
    }
  }



  Serial.print("Scores WandA: ");
  Serial.print(scores[0]);
  Serial.print(", Scores WandB: ");
  Serial.print(scores[1]);
  Serial.print("\n");

  Serial.print("\n");

  laserHit[index] = true;
  startMillis = millis();
  resetReceiver(index);
}


// need this? 

void turnOffReceiver(int index) {
  digitalWrite(ledPins[index], LOW);
}

void resetReceiver(int index) {
  irrecvs[index]->resume();
}

