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
#define SWISH_A4 0xBCD2259E
#define SWISH_A5 0xDB37307D
#define SWIPE_A1 0xD7E84B1B
#define SWIPE_A2 0xFF02FD
#define SWIPE_A3 0x5CDD8FBD

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

// Actions
#define JAB 0
#define SWIPE 1
#define SWISH 2

unsigned int jab_count_a = 0;
unsigned int swish_count_a = 0;
unsigned int swipe_count_a = 0;
unsigned int unknown_action = 0;


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
//     delay(1000);
    }
    if (laserHit[i]) {
      laserHit[i] = false;
    }
  }
  
}


int pumpkin_action() {
  return (rand() % 3);
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

  bool canPrint = true;
  int person_action;
  int pumpkin_action = JAB; //FIXME: pumpkin_action();
  switch(results[index].value)
  {
    case 0xFFFFFFFF:
      canPrint = false;
      break;
    case JAB_A1: 
    case JAB_A2: 
      jab_count_a ++;
      person_action = JAB;
      break;

    case SWISH_A1: 
    case SWISH_A2:
    case SWISH_A3:
    case SWISH_A4:
    case SWISH_A5:
      swish_count_a ++;
      person_action = SWISH;
      break;

    case SWIPE_A1:
    case SWIPE_A2:
    case SWIPE_A3:
      swipe_count_a ++;
      person_action = SWIPE;
      break;

    case JAB_B1:
    case JAB_B2:
      jab_count_b ++;
      person_action = JAB;

      break;

      case SWISH_B1:
      case SWISH_B2:
      case SWISH_B3: 
      case SWISH_B4:
      case SWISH_B5: 
      case SWISH_B6:   
      swish_count_b ++;
      person_action = SWISH;
 
      break;
          
      case SWIPE_B1:
      swipe_count_b ++;
      person_action = SWIPE;

      break;
      
    default:
    Serial.println(results[index].value, HEX);
    unknown_action++;
      break;
  }

  if (canPrint) {
  // A
//  Serial.print("jab_count a: ");
//  Serial.print(jab_count_a);   
//  Serial.print("\n");   
//
//  Serial.print("swish_count a: ");
//  Serial.print(swish_count_a);
//  Serial.print("\n");   
//
//  Serial.print("swipe_count a: ");
//  Serial.print(swipe_count_a);
//  Serial.print("\n");   
//  
//  // B
//  Serial.print("jab_count b: ");
//  Serial.print(jab_count_b);
//  Serial.print("\n"); 
//
//  Serial.print("swish_count b: ");
//  Serial.print(swish_count_b);
//  Serial.print("\n"); 
//
//  Serial.print("swipe_count b: ");
//  Serial.print(swipe_count_b);
//  Serial.print("\n");   
//
//  // unknown
//  Serial.print("unknown action: ");
//  Serial.print(unknown_action);
//  Serial.print("\n");
//  Serial.print("\n");

  Serial.print("pumpkin_action: ");

  switch(pumpkin_action) {
    case JAB:
      Serial.print("jab");
      break;
     case SWISH:
      Serial.print("swish");
      break;
     case SWIPE:
      Serial.print("swipe");
      break;
  }
  
  Serial.print("\n");
  Serial.print("your action: ");


  switch(person_action) {
    case JAB:
      Serial.print("jab");
      break;
     case SWISH:
      Serial.print("swish");
      break;
     case SWIPE:
      Serial.print("swipe");
      break;
  }

  Serial.print("\n");

  Serial.print("winner: ");
  if(pumpkin_action == JAB){
    if (person_action == JAB) {
      Serial.print("tie");
    } else if (person_action == SWIPE) {
      Serial.print("you!");
    } else {
      Serial.print("pumpkin!");
    }
    
  }
 }

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

