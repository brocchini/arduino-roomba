
#include <SoftwareSerial.h>

// Include BLE files.
#include <SPI.h>
#include <boards.h>
#include <RBL_nRF8001.h>
#include <services.h>

// Define input/output pins for BLE
#define INPUT_PIN A0
#define LED_PIN 2

//Roomba
int rxPin = 5;
int txPin = 3;
SoftwareSerial roomba(rxPin,txPin);  // RX,TX
int ddPin = 7;
int ledPin = 13;
byte zeroByte = 0x00;

char sensorsState[10];
#define bumpRight (sensorsState[0] & 0x01)
#define bumpLeft  (sensorsState[0] & 0x02)

void setup()
{
  // Writing to an analog input pin sets baseline for later input.
  digitalWrite(INPUT_PIN, HIGH);

  // Default pins set to 9 and 8 for REQN and RDYN
  // Set your REQN and RDYN here before ble_begin() if you need
  //ble_set_pins(3, 2);

  ble_begin();

  // Set a BLE name.
  ble_set_name("arduinoble");

   roombaSetup();
}

void roombaSetup() {
  pinMode(ddPin,  OUTPUT);   // sets the pins as output
  pinMode(ledPin, OUTPUT);   // sets the pins as output
  Serial.begin(57600);
  roomba.begin(57600);
  digitalWrite(ledPin, HIGH); // say we're alive
  
  // BAUD
  roomba.write(129);  
  roomba.write(10);
  delay(500);
  
  // wake up the robot
  digitalWrite(ddPin, HIGH);
  delay(100);
  digitalWrite(ddPin, LOW);
  delay(500);
  digitalWrite(ddPin, HIGH);
  delay(2000);
  
  // set up ROI to receive commands  
  roomba.write(128);  // START as defined in manual(adim) 
  delay(50);
  roomba.write(130);  // CONTROL adim
  delay(50);
  digitalWrite(ledPin, LOW);  
}

void loop()
{
  readSensors();
   
  // Reac to sensor state
  if(bumpLeft) {
    spinRight();
    delay(1000);
    goForward();
  }
  else if(bumpRight) {
    spinLeft();
    delay(1000);
    goForward();
  }
  
  // If there is a BLE input
  while (ble_available())
  {
      // Read BLE input.
      int c = ble_read();
      switch (c) {
  	case 1:
          goForward();
          break;
          
         case 2:
          goBackward();
          break;
          
         case 3:
  	  spinLeft();
          break;
          
         case 4:
          spinRight();
          break;
          
         default:
          halt();
          break;
        }		
  }

 // Process BLE events.
 ble_do_events();
}

void goForward() {
  roomba.write(137); 
  roomba.write(zeroByte);   
  roomba.write(0xc8);
  roomba.write(0x80);
  roomba.write(zeroByte);
}

void goBackward() {
  roomba.write(137);
  // 0xff38 = -200
   roomba.write(0xff);
  roomba.write(0x38);
  roomba.write(0x80);
  roomba.write(zeroByte);
}

void spinLeft() {
  roomba.write(137); 
  roomba.write(zeroByte);   // 0x00c8 = 200
  roomba.write(0xc8);
  roomba.write(zeroByte);
  roomba.write(0x01); // 0x0001 = spin left
}
void spinRight() {
  roomba.write(137);   
  roomba.write(zeroByte);   // 0x00c8 == 200
  roomba.write(0xc8);
  roomba.write(0xff);
  roomba.write(0xff);   // 0xffff = -1 = spin right
}

void halt(){
  roomba.write(137);   
  roomba.write(zeroByte);   
  roomba.write(zeroByte);
  roomba.write(zeroByte);
  roomba.write(zeroByte);
}

void readSensors() {
  
  roomba.write(142);
  roomba.write(1);  // sensor packet 1, 10 bytes
  delay(100);  

  char i = 0;
  while(roomba.available()) {
    int c = roomba.read();
    if( c==-1 ) {
      for( int i=0; i<5; i ++ ) {
        delay(50);
        // TODO : show error via the LED
        delay(50);
      }
    }
    
    sensorsState[i++] = c;
  }    

}
