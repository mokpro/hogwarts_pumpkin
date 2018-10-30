

/* LED Score Keeper

   This sketch tracks the score between two players: 'R' (red) and 'B' (blue). The
   current score is depicted as two series of red and blue dots which accumulate at
   opposite ends of the LED strip and move toward the center. The first color to
   reach the center wins and triggers a fancy pattern display in the winning color.

 */

#include "FastLED.h"
#include <SoftwareSerial.h>

// sound
#define ARDUINO_RX 5 //should connect to TX of the Serial MP3 Player module
#define ARDUINO_TX 6 //connect to RX of the module
#define CMD_SEL_DEV 0X09
#define DEV_TF 0X02

SoftwareSerial mySerial(ARDUINO_RX, ARDUINO_TX); //init the serial protocol, tell to myserial wich pins are TX and RX
static int8_t Send_buf[8] = {0};

#define DATA_PIN 6
#define LED_TYPE WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS 60 // This ain't gonna work unless this number is even
#define BRIGHTNESS 96

// timing
#define FRAMES_PER_SECOND 1000

// LED array
CRGB leds[NUM_LEDS];

// Secondary elements
// #define FAN 1
// #define PUMPKIN_SPOT 2
// #define IN_PUMPKIN_WHT 3
// #define IN_PUMPKIN_RED 3

// Sound elements
#define GAME_KICKOFF "s-kickoff!"
#define SPELL_MISCAST "s-miscast!"
#define SPELL_CAST_FIRE "s-fire"
#define SPELL_CAST_WATER "s-water"
#define SPELL_CAST_AIR "s-air"
#define TAUNT "s-taunt"
#define SPELLS_MEET "s_meet"
#define SPELLS_TIE "s_tie"
#define SPEL_WINS "s_win"
#define C_LOSES "s_c_lose"
#define C_WINS "s_c_win"

// LED array
// uncommentCRGB leds[NUM_LEDS];

// spell values
#define FIRE_VAL 1
#define WATER_VAL 2
#define AIR_VAL 3

// Game state
bool gameIdle;
char kickoffSpell;
int kickoffCount;
int gameRound;
int baseTime;

// for loop position trackers set to negative 1 o represent a null value
int cIndex;
int hIndex;
// For tracking which spell was cast
int cSpell;
int hSpell;

// Color Settings
int fireHue = 0;    // red (starts at "data in" end of LED strip)
int waterHue = 160; // blue (starts at "data out" end of LED strip)
int airHue = 320;   // #TODO change this to actuval value for white (starts at "data out" end of LED strip)

int TAUNT_COUNTER = 0;

// Timing settings

void setup()
{
  // sanity check delay - allows reprogramming if accidently blowing power w/leds
  delay(FRAMES_PER_SECOND * 5);
//
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  baseTime = FRAMES_PER_SECOND/10;
  //set the computer index to one beforejhhnuyhuyh6gumj6gmuy oil the last led position
  //set the human index to one past the last led position
  resetGameState();
//  playSound(GAME_KICKOFF);
  Serial.begin(9600);

  // sounds initialization
  mySerial.begin(9600); //Start our Serial coms for THE MP3
  delay(500); //Wait chip initialization is complete
  playSound(CMD_SEL_DEV, DEV_TF); //select the TF card
  delay(200);
  playSound(0x06, 0X0F);

  Serial.print("INITIALIZED!!!\n");
}

// ==============================================================================Main Loop
void loop()
{
  delay(6000);
  if (gameIdle == true){
    Serial.print("entered idle \n");

    idleMode();
  } else if (cIndex == NUM_LEDS - 1 || hIndex == 0)
  {
    displayWinner();
  }
  else
  {
    // Serial.print("in game\n");
    gamePlay();
  }
}

// ==============================================================================Main Functions
void resetGameState()
{
  hSpell = 0;
  cSpell = 0;
  hIndex = 999;
  cIndex = -999;
  gameIdle = true;
  kickoffSpell = 0;
  kickoffCount = 0;
  gameRound = 1;
}

// Hit registered event signaled on serial port
void serialEvent()
{
    Serial.print("out serial");
  while (Serial.available())
  {
    Serial.print("in serial");
      char ch = Serial.read();
      Serial.print(ch);
      if (gameIdle == true){
        Serial.print("Game is idle\n");
        Serial.print("3 consecutive spells of the same type will wake it up.\n");
        // if the game is idle 3 consecutive spells of the same type will wake it up
        if (ch == 'f' || ch == 'w' || ch == 'a'){
          if (ch == kickoffSpell){
            kickoffCount = kickoffCount + 1;
            if (kickoffCount > 2)
            {
              Serial.print("Waking up!\n");
              gameIdle = false;
            }
          } else {
            kickoffSpell = ch;
          }
        }
      }
      //only allow a spell if there isnt one currently
      else if (hSpell == 0){
        if (ch == 'f')
        {
          Serial.print("f\n");

          hSpell = FIRE_VAL;
          hCastSpell();
        }
        else if (ch == 'w')
        {
          Serial.print("w\n");

          hSpell = WATER_VAL;
          hCastSpell();
        }
        else if (ch == 'a')
        {
          Serial.print("a\n");

          hSpell = AIR_VAL;
          hCastSpell();
        }
    }
    else
    {
      //TODO prevent this from going off constantly
//      playSound(SPELL_MISCAST);
    }
  }
}

void idleMode()
{
  // TODO review and improve show
  // Serial.print("Idle Display\n");
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS - 1);
  if (random(1,110) > 99){
      if (TAUNT_COUNTER == 0) {
        playSound(0X0F, 0X0701);
      } else if (TAUNT_COUNTER == 1) {
        playSound(0X0F, 0X0801);
      } else if (TAUNT_COUNTER == 2) {
        playSound(0X0F, 0X0901);
      } else if (TAUNT_COUNTER == 3) {
        playSound(0X0F, 0X0A01);
      } else if (TAUNT_COUNTER == 4) {
        playSound(0X0F, 0X0B01);
      } else if (TAUNT_COUNTER == 5) {
        playSound(0X0F, 0X0C01);
      } else if (TAUNT_COUNTER == 6) {
        playSound(0X0F, 0X0D01);
        TAUNT_COUNTER = -1;
      }
      TAUNT_COUNTER = TAUNT_COUNTER + 1;
    delay(FRAMES_PER_SECOND * 1);
  }
  delay(FRAMES_PER_SECOND * 1);
}

void gamePlay()
{
  int cWait = 0;
  Serial.print(gameRound);
  Serial.print("\n");
  //check for spells
  //if none then computer casts
  //if computer only then wait for human to cast or lose
  //if human only then wait 1-3 loops then cast
  Serial.print("compy spell");
  Serial.print(cSpell);
  Serial.print("\n");
  if (cIndex < NUM_LEDS - 1 || hIndex > 0)
  {
    // check for existence of computer spell
    if (cSpell == 0) {
      // if it's round 1 or compy has waited for three turns, then go
      if (cWait > 2 || gameRound == 1){
        cCastSpell();
        cWait = 0;
      }
      // if compy has waited at least once then gie him a one in 3 chance of going again
      else if(cWait > 1 && random(1, 4) > 2)
      {
        cCastSpell();
        cWait = 0;
      } else {
        cWait = cWait +1;
      }
    }
    else
    {
      //if it exists, move it towards the center
      cIndex = cIndex + 1;
      leds[cIndex] = CHSV(getHue(cSpell), 255, 192);
    }

    // check for existence of human spell
    if (hSpell != 0){
        //if it's there and has the default index then cast
        if (hIndex == 999)
        {
          hCastSpell;
        }
        //if it exists but has another index move it towards the center
        hIndex = hIndex - 1;
        leds[hIndex] = CHSV(getHue(hSpell), 255, 192);
    }

    FastLED.show();
    // if the two positions are about to meet then
    if (cIndex == hIndex || cIndex - hIndex == 1 || cIndex - hIndex == -1)
    {
      Serial.print("------------INDEXES EQUAL ");
      resolveSpells();
    }
    //standard delay
    Serial.print("------------cindex ");
    Serial.print(cIndex);
    Serial.print("\n");
    Serial.print("-------------hindex ");
    Serial.print(hIndex);
    Serial.print("\n");
    delay(FRAMES_PER_SECOND / gameRound);
  }
}

void displayWinner()
{
  Serial.print("-------------WINNER DECLARED ");

  // "sinelon" pattern - a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS - 1);
  //computer wins
  if (cIndex == NUM_LEDS - 1)
  {
    leds[pos] += CHSV(getHue(cSpell), 255, 192);
    Serial.print("-------------COMPUTER WINS ");
    //cWinsLights()
    cWinsSounds();
  }
  else
  {
    leds[pos] += CHSV(getHue(hSpell), 255, 192);
    Serial.print("-------------HUMAN WINS ");
    //hWinsLights()
    hWinsSounds();
  }
  gameRound = gameRound + 1;

  // Send the 'leds' array out to the actual LED strip
  FastLED.show();
  // Insert a delay to keep the framerate modest
  FastLED.delay(FRAMES_PER_SECOND*10);
  resetGameState();
  // reset game state
}

// ==============================================================================Lookup Functions
int getHue(int spell)
{
  if (spell == 1)
  {
    return fireHue;
  }
  else if (spell == 2)
  {
    return waterHue;
  }
  else
  {
    return airHue;
  }
}

// ==============================================================================Secondary Functions
void cCastSpell()
{
  // TODO all of these delays should equal the current game delay
  Serial.print("cast computer spell\n");
  // set index to start pos
  cIndex = -1;
  // if there is no human spell then a random spell is cast, otherwise the correct counter spell is cast
  if (hSpell == 0) {
  cSpell = random(1, 4);
  } else if (hSpell == 1){
    cSpell = 2;
  } else if (hSpell == 2){
    cSpell = 3;
  } else if (hSpell == 3){
    cSpell = 1;
  }
  Serial.print(cSpell);
  delay(FRAMES_PER_SECOND * 2);
  playSound(0X0F, 0X0101);

  // starts black
  leds[1] = CHSV(getHue(cSpell), 255, 192);
  FastLED.show();
  delay(baseTime);
  leds[2] = CHSV(getHue(cSpell), 255, 192);
  FastLED.show();
  delay(baseTime);
  leds[3] = CHSV(getHue(cSpell), 255, 192);
  FastLED.show();
  delay(baseTime);
  leds[3] = CRGB::Black;
  FastLED.show();
  delay(baseTime);
  leds[2] = CRGB::Black;
  FastLED.show();
  delay(baseTime);
}

void hCastSpell()
{
  Serial.print("cast human spell\n");
  // set index to start pos
  hIndex = NUM_LEDS;
  Serial.print(hIndex);
  playSpellSound(hSpell);

  // starts black
  leds[NUM_LEDS - 1] = CHSV(getHue(hSpell), 255, 192);
  FastLED.show();
  delay(baseTime);
  leds[NUM_LEDS - 2] = CHSV(getHue(hSpell), 255, 192);
  FastLED.show();
  delay(baseTime);
  leds[NUM_LEDS - 3] = CHSV(getHue(hSpell), 255, 192);
  FastLED.show();
  delay(baseTime);
  leds[NUM_LEDS-3] = CRGB::Black;
  FastLED.show();
  delay(baseTime);
  leds[NUM_LEDS-2] = CRGB::Black;
  FastLED.show();
  delay(baseTime);
}

void playSpellSound(int spell)
{
  if (spell == 1)
  {
    playSound(0X0F, 0X0301);
  }
  else if (spell == 2)
  {
    playSound(0X0F, 0X0401);
  }
  else
  {
    playSound(0X0F, 0X0201);
  }
}

void resolveSpells()
{
  Serial.print("Spells Resovling\n");
  // char winner;
  int result = cSpell - hSpell;

  switch (result)
  {
  case 1:
  case -2:
    cWinRound();
    Serial.print("-------------COMPUTER WINS ROUND");

    hIndex = 999;
    hSpell = 0;
    break; /* optional */

  case -1:
  case 2:
    hWinRound();
    Serial.print("-------------HUMAN WINS ROUND");
    cIndex = -999;
    cSpell = 0;
    break; /* optional */

  // if it's a tie, dispaly the tie animation and set both indexes to their end, setting off the tie game end
  default: /* Optional */
    tieDisplay();
    //TODO possibly move them back
    cIndex = -999;
    cSpell = 0;
    hIndex = 999;
    hSpell = 0;
  }
}

void playSound(int8_t command, int16_t dat)
{
   delay(20);
   Send_buf[0] = 0x7e; //starting byte
   Send_buf[1] = 0xff; //version
   Send_buf[2] = 0x06; //the number of bytes of the command without starting byte and ending byte
   Send_buf[3] = command; //
   Send_buf[4] = 0x00;//0x00 = no feedback, 0x01 = feedback
   Send_buf[5] = (int8_t)(dat >> 8);//datah
   Send_buf[6] = (int8_t)(dat); //datal
   Send_buf[7] = 0xef; //ending byte
   for(uint8_t i=0; i<8; i++)//
   {
     mySerial.write(Send_buf[i]) ;//send bit to serial mp3
     Serial.print(Send_buf[i],HEX);//send bit to serial monitor in pc
   }
   Serial.println();
}

void hWinRound()
{
  // human spell pushes compy spell back
  printf("human Wins");
  if (cIndex > 0) {
  leds[cIndex] = CHSV(getHue(hSpell), 255, 192);
  leds[cIndex - 1] = CHSV(getHue(cSpell), 255, 192);
  FastLED.show();
  delay(baseTime);

  leds[cIndex - 1] = CRGB::Black;
  if (cIndex > 1)
  {
    leds[cIndex - 2] = CHSV(getHue(cSpell), 255, 192);
    FastLED.show();
    delay(baseTime);
    leds[cIndex - 2] = CRGB::Black;
    FastLED.show();
  }
  delay(FRAMES_PER_SECOND);
  }
}

void cWinRound()
{
  // computer spell pushes human spell back
  printf("compy Wins");
  leds[hIndex] = CHSV(getHue(cSpell), 255, 192);
  leds[hIndex + 1] = CHSV(getHue(hSpell), 255, 192);
  FastLED.show();
  delay(baseTime);

  leds[hIndex + 1] = CRGB::Black;
  if (cIndex < (NUM_LEDS-2))
  {
    leds[cIndex + 2] = CHSV(getHue(hSpell), 255, 192);
    FastLED.show();
    delay(baseTime);
    leds[cIndex + 2] = CRGB::Black;
    FastLED.show();
  }
  delay(FRAMES_PER_SECOND);
}

void tieDisplay()
{
  // flash three times, the hard way
  printf("tie");
  playSound(0X0F, 0X0601);
  leds[cIndex] = CRGB::Black;
  leds[hIndex] = CRGB::Black;
  FastLED.show();
  delay(baseTime);
  leds[cIndex] = CHSV(getHue(cSpell), 255, 192);
  leds[hIndex] = CHSV(getHue(hSpell), 255, 192);
  FastLED.show();
  delay(baseTime);
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS - 1);
  FastLED.show();
  delay(baseTime);
}

void cWinsSounds(){
  playSound(0X0F, 0X0501);
  delay(FRAMES_PER_SECOND *3);
}

void hWinsSounds() {
  playSound(0X0F, 0X0501);
  delay(FRAMES_PER_SECOND *3);
}
