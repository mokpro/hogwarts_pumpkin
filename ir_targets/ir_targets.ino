#include <IRremote.h>

// Constants
#define BASE_COUNT 1
#define DEBUG_MODE true

// Spell: JAB
#define JAB_A1 0xFF629D
#define JAB_A2 0x511DBB
#define JAB_B1 0xFF906F
#define JAB_B2 0xE5CFBD7F
#define JAB_A3 0x2EC19612

// Spell SWISH
#define SWISH_A1 0x87C6D1F
#define SWISH_A2 0x52A3D41F
#define SWISH_A3 0xFF22DD
#define SWISH_A4 0xBCD2259E
#define SWISH_A5 0xDB37307D
#define SWISH_6 0xF9EC0295
#define SWISH_B1 0xF92F9847
#define SWISH_B2 0x1D320F8A
#define SWISH_B3 0x6653181C
#define SWISH_B4 0xF076C13B
#define SWISH_B5 0xFFE01F
#define SWISH_B6 0xF9EC0294

//Spell: SWIPE
#define SWIPE_A1 0xD7E84B1B
#define SWIPE_A2 0xFF02FD
#define SWIPE_A3 0x5CDD8FBD
#define SWIPE_A4 0xA3C8EDDB
#define SWIPE_B1 0xFFA857
#define SWIPE_5 0x3F15A646
#define SWIPE_6 0x1EF51DF5 
#define SWIPE_7 0x98473814
#define SWIPE_8 0xC845A620
#define SWIPE_9 0x8D828EEA
#define SWIPE_10 0x87FFECB2
#define SWIPE_11 0x25AE7EE0
#define SWIPE_12 0x1EF51DF6
#define SWIPE_13 0x4AB0F7B7


// Actions
#define JAB 'a'
#define SWIPE 'w'
#define SWISH 'f'
#define INVALID 'n'

// Define all the devices as global variables
IRrecv *irrecvs[BASE_COUNT];
int ledPins[] = {2}; // 2, 7, 4
int receiverPins[] = {8}; //8, 13, 12

// State variables
bool laserHit = false;

// Store scores
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
  for (int i = 0; i < BASE_COUNT; i++) {
    if (!laserHit && irrecvs[i]->decode(&result)) {
      recordHit(i);
    }

    if (laserHit) {
      laserHit = false;
    }
  }
}

void recordHit(int index) {
  bool canPrint = true;
  char pSpell = detectSpell(index);

  if (pSpell != INVALID) {
    if (DEBUG_MODE) {
      printSpell(pSpell);
    } else {
      writeSpellToSerial(pSpell);
    }
  }

  resetReceiver(index);
}


void resetReceiver(int index) {
  laserHit = true;
  irrecvs[index]->resume();
}

void writeSpellToSerial(char spell) {
  Serial.write(spell);
}

char detectSpell(int index) {
  char pSpell;
  switch(result.value)
  {
    case 0xFFFFFFFF:
    pSpell = INVALID;
    break;

    case JAB_A1:
    case JAB_A2:
    case JAB_B1:
    case JAB_B2:
    case JAB_A3:
    pSpell = JAB;
    break;

    case SWISH_A1:
    case SWISH_A2:
    case SWISH_A3:
    case SWISH_A4:
    case SWISH_A5:
    case SWISH_B1:
    case SWISH_B2:
    case SWISH_B3:
    case SWISH_B4:
    case SWISH_B5:
    case SWISH_B6:
    case SWISH_6:
    pSpell = SWISH;
    break;

    case SWIPE_A1:
    case SWIPE_A2:
    case SWIPE_A3:
    case SWIPE_A4:
    case SWIPE_B1:
    case SWIPE_5:
    case SWIPE_6:
    case SWIPE_7:
    case SWIPE_8:
    case SWIPE_9:
    case SWIPE_10:
    case SWIPE_11:
    case SWIPE_12:
    pSpell = SWIPE;
    break;

  }

  return pSpell;
}

void printSpell(char pSpell) {
  Serial.print("\n");
  Serial.print("your action: ");

  switch(pSpell) {
    case JAB:
    Serial.print("jab");
    break;
    case SWISH:
    Serial.print("swish");
    break;
    case SWIPE:
    Serial.print("swipe");
    break;

    default:
    Serial.print(result.value, HEX);
  }

  Serial.print("\n");
}

