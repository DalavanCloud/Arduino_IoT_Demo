/*
 * This is part of application own meterology
 * This is for receiver. It gets data from station via radio waves
 * using library RFM12B.h
 */

#include "RFM12B.h"

// You will need to initialize the radio by telling it what ID it has and what network it's on
// The NodeID takes values from 1-127, 0 is reserved for sending broadcast messages (send to all nodes)
// The Network ID takes values from 0-255
// By default the SPI-SS line used is D10 on Atmega328. You can change it by calling .SetCS(pin) where pin can be {8,9,10}
#define NODEID           1  //network ID used for this unit
#define NETWORKID       99  //the network ID we are on
#define SERIAL_BAUD 9600

typedef struct {
  float temperature; // measured tempreture in Celsius
  float humidity;    // measured humidity
} Payload;
Payload theData;

//encryption is OPTIONAL
//to enable encryption you will need to:
// - provide a 16-byte encryption KEY (same on all nodes that talk encrypted)
// - to call .Encrypt(KEY) to start encrypting
// - to stop encrypting call .Encrypt(NULL)
uint8_t KEY[] = "YCBcfd5sd4yp76lD";

// Need an instance of the Radio Module
RFM12B radio;
void setup()
{
  radio.Initialize(NODEID, RF12_868MHZ, NETWORKID);
  radio.Encrypt(KEY);
  Serial.begin(SERIAL_BAUD);
  Serial.println("Listening...");
}

void loop()
{
  if (radio.ReceiveComplete())
  {
    if (radio.CRCPass())
    {
      if (*radio.DataLen != sizeof(Payload))
        Serial.print(F("Invalid payload received, not matching Payload struct!"));
      else
      {
        theData = *(Payload*)radio.Data; //assume radio.DATA actually contains our struct and not something else

        Serial.print(F("Temperature: "));
        Serial.print(theData.temperature);
        Serial.print(F(" C, Humidity: "));
        Serial.print(theData.humidity);
        Serial.print(F(" %\n"));

        if (radio.ACKRequested())
        {
          radio.SendACK();
          Serial.print(" - ACK sent");
        }
      }
    }
    else
      Serial.print("BAD-CRC");

    Serial.println();
  }
}
