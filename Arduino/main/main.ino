/*
  This a simple example of the aREST Library for Arduino (Uno/Mega/Due/Teensy)
  using the Ethernet library (for example to be used with the Ethernet shield).
  See the README file for more details.
  Written in 2014 by Marco Schwartz under a GPL license.
*/

// Libraries
#include <SPI.h>
#include <Ethernet.h>
#include <aREST.h>
#include <avr/wdt.h>

// Enter a MAC address for your controller below.
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0E, 0xFE, 0x40 };

// IP address in case DHCP fails
IPAddress ip(10,0,0,3);

// Ethernet server
EthernetServer server(80);

// Create aREST instance
aREST rest = aREST();

int actualMotorSpeed;
bool DHCP_ENABLE = false; 

//Motorsheild
int EN1 = 6;
int EN2 = 5;  //Roboduino Motor shield uses Pin 9
int IN1 = 7;
int IN2 = 4; //Latest version use pin 4 instead of pin 8


// Declare functions to be exposed to the API
int dispenserControl(String duration);
int setMotorSpeed(String command);
  

void setup(void)
{
  // Start Serial
  Serial.begin(115200);

  // Init variables and expose them to REST API
  actualMotorSpeed = 200;
  rest.variable("motorSpeed",&actualMotorSpeed);

  // Function to be exposed
  rest.function("dispense",dispenserControl);
  rest.function("setMotorSpeed",setMotorSpeed);

  // Give name & ID to the device (ID should be 6 characters long)
  rest.set_id("008");
  rest.set_name("candy_dispenser");

  if (DHCP_ENABLE) {
    // Start the Ethernet connection and the server
    if (Ethernet.begin(mac) == 0) {
      Serial.println("Failed to configure Ethernet using DHCP");
      // no point in carrying on, so do nothing forevermore:
      // try to congifure using IP address instead of DHCP:
      Ethernet.begin(mac, ip);
    }
  } else {
     Ethernet.begin(mac, ip);
  }
  
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());

  // Start watchdog
  wdt_enable(WDTO_4S);
}

void loop() {

  // listen for incoming clients
  EthernetClient client = server.available();
  rest.handle(client);
  wdt_reset();

}

// Custom function accessible by the API
int dispenserControl(String command) {

  // Get state from command
  int duration = command.toInt();

  Motor1(actualMotorSpeed, false);
  delay(duration);
  Motor1(0, false);
  
  return duration;
}


// Custom function accessible by the API
int setMotorSpeed(String command) {

  int value = command.toInt();

  if (value < 0) {
    actualMotorSpeed = 0;
  }else if (value > 255) {
    actualMotorSpeed = 255;
  } else {
    actualMotorSpeed = value;     
  }
  
  return actualMotorSpeed;
}



void Motor1(int pwm, boolean reverse) {
  analogWrite(EN2, pwm); //set pwm control, 0 for stop, and 255 for maximum speed
  if (reverse)  {
    digitalWrite(IN2, HIGH);
  }
  else  {
    digitalWrite(IN2, LOW);
  }
}
