#include <IRremote.h>

// Constants
#define BASE_COUNT 2
#define BLUE_LASER 0
#define RED_LASER 1
#define DEBUG_MODE true
#define MIN_DELAY 3000
#define DELAY_CEIL 4000

//Spells from WAND A
#define JAB_A1 0xFF629D
#define JAB_A2 0x511DBB
#define SWISH_A1 0x87C6D1F
#define SWISH_A2 0x52A3D41F
#define SWISH_A3 0xFF22DD
#define SWIPE_A1 0xD7E84B1B
#define SWIPE_A2 0xFF02FD

//Spells from WAND B
#define JAB_B1 0xFF906F
#define JAB_B2 0xE5CFBD7F
#define SWISH_B1 0xF92F9847
#define SWISH_B2 0x1D320F8A
#define SWISH_B3 0x6653181C
#define SWISH_B4 0xF076C13B
#define SWISH_B5 0xFFE01F
#define SWISH_B6 0xF9EC0294
#define SWIPE_B1 0xFFA857

unsigned int jab_count_a = 0;
unsigned int swish_count_a = 0;
unsigned int swipe_count_a = 0;


unsigned int jab_count_b = 0;
unsigned int swish_count_b = 0;
unsigned int swipe_count_b = 0;

// Define all the devices as global variables
IRrecv *irrecvs[BASE_COUNT];
int ledPins[] = {2, 7}; // 2, 7, 4
int receiverPins[] = {8, 12}; //8, 13, 12

// Define all time related variables
unsigned long startMillis;
unsigned long elapsedTime;


// State variables
int activeReceiver;
bool laserHit[BASE_COUNT] = {false, false};

// Store scores
int scores[] = {0,0};
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
     recordHit(i);
     delay(1000);
    }
    if (laserHit) {
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
  Serial.println("this is from sensor:");
  Serial.println(index);

  switch(results[index].value)
  {
    case JAB_A1: 
    case JAB_A2: 
      jab_count_a ++;
      Serial.println("jab_count a");
      Serial.println(jab_count_a);   
      break;

    case SWISH_A1: 
    case SWISH_A2: 
    case SWISH_A3:
      swish_count_a ++;
      Serial.println("swish_count a");
      Serial.println(swish_count_a);
      break;

    case SWIPE_A1:
    case SWIPE_A2:
      swipe_count_a ++;
      Serial.println("swipe_count a");
      Serial.println(swipe_count_a);
      break;

    case JAB_B1:
    case JAB_B2:
      jab_count_b ++;
      Serial.println("jab_count b");
      Serial.println(jab_count_b);   
      break;

      case SWISH_B1:
      case SWISH_B2:
      case SWISH_B3: 
      case SWISH_B4:
      case SWISH_B5: 
      case SWISH_B6:   
      swish_count_b ++;
      Serial.println("swish_count b");
      Serial.println(swish_count_b);   
      break;
          
      case SWIPE_B1:
      swipe_count_b ++;
      Serial.println("swipe_count b");
      Serial.println(swipe_count_b);   
      break;
      

      
    default:
      Serial.println(results[index].value, HEX);
      Serial.println("default value");
      break;
  }
  laserHit[index] = true;
  startMillis = millis();
  turnOnReceiver(index);
}


// need this? 

void turnOffReceiver(int index) {
  digitalWrite(ledPins[index], LOW);
}

void turnOnReceiver(int index) {
  digitalWrite(ledPins[index], HIGH);
  irrecvs[index]->resume();
}

