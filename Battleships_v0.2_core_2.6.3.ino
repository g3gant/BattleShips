/******************************* BATTLESHIP ********************************
 *  Version 0.2b
 *  Author Gegeniger Anton
 *  Changes from v0.1
 *    - Render engine was redone, now insead of using one array for everything I have 4 arrays (arr_1 - main array for ships, arr_2 - show selected ship, arr_3 - collisions, arr_4 - final render
 *    - Collisions checking every Draw() cycle for each ship
 *    - Added WiFi connection (only connection, no data transfer yet)
 *    - Added animation before start (while WiFi connection establishing)
 *    - Added Game Mode variable
 *    
****************************************************************************/

#include "AiEsp32RotaryEncoder.h"
#include "Arduino.h"
#define FASTLED_INTERRUPT_RETRY_COUNT 0
#define FASTLED_ALLOW_INTERRUPTS 0
#define FASTLED_ESP8266_RAW_PIN_ORDER
#include <FastLED.h>
#include "classes.h"
#include <AceButton.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>



// Set WiFi credentials
#define WIFI_SSID "******"
#define WIFI_PASS "**********"
#define UDP_PORT 4210

// UDP
WiFiUDP UDP;
char packet[255];
char reply_hit[] = "Hit!";
char reply_miss[] = "Miss!";
bool paired = false;
IPAddress ip(10, 0, 0, 111);
IPAddress gwIP(10, 0, 0, 1);
IPAddress msk(255, 255,255, 0);
IPAddress brdIP(10,0,0,255);

// GAME VARIABLES
bool readyToShoot = false;
bool OpponentReadyToShoot = false;


#define ROTARY_ENCODER_X_A_PIN 12
#define ROTARY_ENCODER_X_B_PIN 14
#define ROTARY_ENCODER_X_BUTTON_PIN 2
// Buttons configuration
#define LEFT_BUTTON_PIN D3
#define RIGHT_BUTTON_PIN D7
using namespace ace_button;
//
#define ROTARY_ENCODER_Y_A_PIN 4
#define ROTARY_ENCODER_Y_B_PIN 5
#define ROTARY_ENCODER_Y_BUTTON_PIN 2

#define ROTARY_ENCODER_X_VCC_PIN -1 /* 27 put -1 of Rotary encoder Vcc is connected directly to 3,3V; else you can use declared output pin for powering rotary encoder */
#define ROTARY_ENCODER_Y_VCC_PIN -1
//depending on your encoder - try 1,2 or 4 to get expected behaviour
//#define ROTARY_ENCODER_X_STEPS 1
//#define ROTARY_ENCODER_X_STEPS 2
#define ROTARY_ENCODER_X_STEPS 4
#define ROTARY_ENCODER_Y_STEPS 4



#define LED_PIN     D8
#define NUM_LEDS    100


AceButton leftButton(LEFT_BUTTON_PIN);
AceButton rightButton(RIGHT_BUTTON_PIN);

 byte x,y;
 byte old_x,old_y;
 //bool set_arr[100];
 byte game_mode = 0; //Game mode:
                     //           1 - Connection
                     //           2 - Ships allocation
                     //           3 - You shooting - Draw Aim and shots that were made
                     //           4 - Opponent shooting - Draw your ships and shots made by opponent
                     //           5 - Waiting for shoot/Receiving a shot
                     //           6 - 
 bool set;
 byte sel_ship = 0; // sellected ship
 bool chg_sel = false; //flag if selection was changed

byte arr_1[10][10];
byte arr_2[10][10]; // SELECTED SHIP
byte arr_3[10][10]; // COLLISION
byte arr_4[10][10]; //FINAL RENDER
byte arr_5[10][10]; //SHIP STORAGE
byte arr_6[10][10]; //SHOOTING FIELD
byte arr_7[10][10]; 

//byte ship_set[10][10];
byte ships_3[100] =  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
 
CRGB leds[NUM_LEDS];
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

//instead of changing here, rather change numbers above
//AiEsp32RotaryEncoder rotaryEncoderX = AiEsp32RotaryEncoder(ROTARY_ENCODER_X_A_PIN, ROTARY_ENCODER_X_B_PIN, ROTARY_ENCODER_X_VCC_PIN, ROTARY_ENCODER_X_STEPS);
AiEsp32RotaryEncoder rotaryEncoderX = AiEsp32RotaryEncoder(ROTARY_ENCODER_X_A_PIN, ROTARY_ENCODER_X_B_PIN, ROTARY_ENCODER_X_BUTTON_PIN, ROTARY_ENCODER_X_VCC_PIN, ROTARY_ENCODER_X_STEPS);
//AiEsp32RotaryEncoder rotaryEncoderY = AiEsp32RotaryEncoder(ROTARY_ENCODER_Y_A_PIN, ROTARY_ENCODER_Y_B_PIN, ROTARY_ENCODER_Y_VCC_PIN, ROTARY_ENCODER_Y_STEPS);
AiEsp32RotaryEncoder rotaryEncoderY = AiEsp32RotaryEncoder(ROTARY_ENCODER_Y_A_PIN, ROTARY_ENCODER_Y_B_PIN, ROTARY_ENCODER_Y_BUTTON_PIN, ROTARY_ENCODER_Y_VCC_PIN, ROTARY_ENCODER_Y_STEPS);


// Declaring SHIPS object array

SHIP ship1;
SHIP ships[5];
// ================== RIGHT BUTTON CLICK ===================
void right_Click()
{

  switch (game_mode)
  {
    case 2: RotateShip();
    break;
    case 3: MakeShot(x,y);
  }
  
}
//===========================================================

// ================== LEFT BUTTON CLICK ===================
void left_Click()
{

    switch (game_mode)
    {
      case 2: SelectNextShip();
      break;
    }
    
}
//===========================================================




//========================= ENCODER EVENTS =============================

void rotary_loop()
{

    switch (game_mode)
    {
      case 2: MoveShip();
      break;
      case 3: MoveAim();
      break;
    }

}
//===================================================================


// NEW EVENT HANDLER FOR BUTTONS
void handleEvent(AceButton* button, uint8_t eventType,
    uint8_t /*buttonState*/) {
  uint8_t pin = button->getPin();
  
  if (pin == LEFT_BUTTON_PIN) {
    switch (eventType) {
      // Interpret a Released event as a Pressed event, to distiguish it
      // from a LongPressed event.
        case AceButton::kEventReleased:
        Serial.print("LEFT BUTTON SHORT RELEASE");
        left_Click();
        break;

        // LongPressed goes in and out of edit mode.
        case AceButton::kEventLongReleased:
        Serial.print("LEFT BUTTON LONG RELEASE");
        
        break;
    }
      } else if (pin == RIGHT_BUTTON_PIN) {
        switch (eventType) {
        case AceButton::kEventReleased:
        //Serial.print("RIGHT BUTTON SHORT RELEASE");
        right_Click();
        break;

        // LongPressed goes in and out of edit mode.
        case AceButton::kEventLongReleased:
        //Serial.print("RIGHT BUTTON LONG RELEASE");
        //isOverlap(&ships[sel_ship]);
        SwitchGame();
        break;
    }
  }
  
  
  
  }


void IRAM_ATTR readEncoderISRX()
{
  rotaryEncoderX.readEncoder_ISR();
}
void IRAM_ATTR readEncoderISRY()
{
  rotaryEncoderY.readEncoder_ISR();
}


void setup()
{
  Serial.begin(115200);
  FastLED.addLeds<WS2812, 15, GRB>(leds, NUM_LEDS);
  //we must initialize rotary encoder
  rotaryEncoderX.begin();
  rotaryEncoderX.setup(readEncoderISRX);
  rotaryEncoderY.begin();
  rotaryEncoderY.setup(readEncoderISRY);

  // Button uses the built-in pull up register.
  pinMode(LEFT_BUTTON_PIN, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON_PIN, INPUT_PULLUP);

  // Configure the ButtonConfig with the event handler.
  ButtonConfig* buttonConfig = ButtonConfig::getSystemButtonConfig();
  buttonConfig->setEventHandler(handleEvent);
  buttonConfig->setFeature(ButtonConfig::kFeatureLongPress);
  buttonConfig->setFeature(ButtonConfig::kFeatureRepeatPress);
  buttonConfig->setFeature(ButtonConfig::kFeatureSuppressAfterLongPress);

  
  x=0;
  y=0;
  
  bool circleValues = false;
  rotaryEncoderX.setBoundaries(0, 9, circleValues); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)
  rotaryEncoderY.setBoundaries(0, 9, circleValues); //minValue, maxValue, circleValues true|false (when max go to min and vice versa)
  rotaryEncoderX.disableAcceleration(); //acceleration is now enabled by default - disable if you dont need it
  rotaryEncoderY.disableAcceleration();
  //rotaryEncoder.setAcceleration(250); //or set the value - larger number = more accelearation; 0 or 1 means disabled acceleration
  
    for (int i = 0; i<=9; i++)
  {
    for (int j = 0; j<=9; j++)
    {
      arr_1[j][i] = 0;
      arr_2[j][i] = 0;
      arr_3[j][i] = 0;
      arr_4[j][i] = 0;
      arr_5[j][i] = 0;
      arr_6[j][i] = 0;
//      ship_set[i][j] = 0;

    }

  }

game_mode = 1;

//ship1.create(0,0,5,1);
ships[0].create(0,0,5,0);
Serial.print(sizeof(ships));

ships[1].create(0,2,4,0);
Serial.print(sizeof(ships));
ships[2].create(0,4,3,0);
Serial.print(sizeof(ships));
ships[3].create(0,6,2,0);
Serial.print(sizeof(ships));
ships[4].create(0,8,2,0);
Serial.print(sizeof(ships));
//sel_ship=0;
ships[sel_ship].set = false;

}


//========================SET ENCODER ====================
void set_encoder(byte set_x, byte set_y)  //RESET ENCODER WITH GIVEN X,Y
{
  rotaryEncoderY.setEncoderValue(set_y);
  rotaryEncoderX.setEncoderValue(set_x);
  x=set_x;
  y=set_y;
};
//=========================================================










//=====================================LOOP==============================================
void loop()
{
  //WiFi.config(ip,gwIP,msk);
  //WiFi.setSleepMode(WIFI_NONE_SLEEP);
  bool opReady = false;
  while (game_mode == 1)
  {
        // Begin WiFi

     //WiFi.config(ip,gwIP,msk);
     WiFi.begin(WIFI_SSID, WIFI_PASS);
     while (WiFi.status() != WL_CONNECTED)
        {
          confetti();

          Serial.print(".");

        }
      Serial.println(WiFi.localIP()); 
    // Begin listening to UDP port
    UDP.begin(UDP_PORT);
    Serial.print("Listening on UDP port ");
    Serial.println(UDP_PORT);
        while (!paired)
        {
              UDP.beginPacket(brdIP, UDP_PORT);
              UDP.write("Connected");
              UDP.endPacket();
              
              int packetSize = UDP.parsePacket();

              
              if (packetSize) {
                    int len = UDP.read(packet, 255);
                    if (len > 0)
                    {
                      packet[len] = '\0';
                    }
                    Serial.print("Packet received: ");
                    Serial.println(packet);
                    Serial.println("From: ");
                    Serial.print(UDP.remoteIP());
                    Serial.print(":");
                    Serial.print(UDP.remotePort());
                    String myString = String(packet);
                    if (myString == "Connected")
                    {
                      paired = true;
                    }
                    // Send return packet
                    UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
                    UDP.write("Connection Established");
                    UDP.endPacket();
                    
                    
              }
          delay(50);
              
          confetti();

        }
    game_mode = 2;
  }

  while (game_mode == 2)
  {
      draw_ships();
      rotary_loop();
      leftButton.check();
      rightButton.check();
      delay(2);
      opReady = isOpponentReady();
      if (opReady) {
        OpponentReadyToShoot = true;
      }
  }
    while (game_mode == 3)
  {
      if (!opReady) {
        opReady = isOpponentReady();
        confetti();
    
      } else {
      
      rotary_loop();
      leftButton.check();
      rightButton.check();
      draw_aim(x,y);
      delay(2);
      };
  }
  
 
}
