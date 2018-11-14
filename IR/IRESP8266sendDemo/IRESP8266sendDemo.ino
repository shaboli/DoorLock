/* IRremoteESP8266: IRsendDemo - demonstrates sending IR codes with IRsend.
 *
 * Version 1.0 April, 2017
 * Based on Ken Shirriff's IrsendDemo Version 0.1 July, 2009,
 * Copyright 2009 Ken Shirriff, http://arcfn.com
 *
 * An IR LED circuit *MUST* be connected to the ESP8266 on a pin
 * as specified by kIrLed below.
 *
 * TL;DR: The IR LED needs to be driven by a transistor for a good result.
 *
 * Suggested circuit:
 *     https://github.com/markszabo/IRremoteESP8266/wiki#ir-sending
 *
 * Common mistakes & tips:
 *   * Don't just connect the IR LED directly to the pin, it won't
 *     have enough current to drive the IR LED effectively.
 *   * Make sure you have the IR LED polarity correct.
 *     See: https://learn.sparkfun.com/tutorials/polarity/diode-and-led-polarity
 *   * Typical digital camera/phones can be used to see if the IR LED is flashed.
 *     Replace the IR LED with a normal LED if you don't have a digital camera
 *     when debugging.
 *   * Avoid using the following pins unless you really know what you are doing:
 *     * Pin 0/D3: Can interfere with the boot/program mode & support circuits.
 *     * Pin 1/TX/TXD0: Any serial transmissions from the ESP8266 will interfere.
 *     * Pin 3/RX/RXD0: Any serial transmissions to the ESP8266 will interfere.
 *   * ESP-01 modules are tricky. We suggest you use a module with more GPIOs
 *     for your first time. e.g. ESP-12 etc.
 */

#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <IRremoteESP8266.h>
#include <IRsend.h>

const int BUTTON_PIN = 5;
const uint16_t kIrLed = 4;  // ESP8266 GPIO pin to use. Recommended: 4 (D2).
const int khz = 38; // 38kHz carrier frequency for the NEC protocol

IRsend irsend(kIrLed);  // Set the GPIO to be used to sending the message.

int lastButtonToggle = 0;
int lastButtonValue = LOW;

void setup() {
  irsend.begin();
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
  pinMode(BUTTON_PIN, INPUT);
}

void loop() {
  if(digitalRead(BUTTON_PIN) == HIGH) {
    if(lastButtonValue == LOW) {
      lastButtonValue = HIGH;

      if(lastButtonToggle == 0) {
        lastButtonToggle = 1;
        //irsend.sendSAMSUNG(0xE0E0E01F, 32);

        uint16_t irSignal[] = {8968, 4420, 680, 1632, 652, 516, 652, 544, 652, 544, 652, 1660, 652, 516, 652, 544, 652, 544, 648, 1664, 676, 492, 652, 544, 652, 1660, 676, 492, 652, 544, 648, 548, 648, 548, 648, 548, 648, 548, 648, 544, 652, 544, 652, 544, 652, 1660, 676, 492, 652, 544, 652, 544, 648, 548, 648, 548, 648, 548, 648, 1664, 676, 492, 648, 1664, 676, 492, 652, 544, 648, 1664, 676, 492, 648, 19864, 680, 512, 652, 544, 652, 544, 652, 544, 648, 548, 652, 544, 648, 548, 648, 548, 648, 544, 652, 544, 652, 544, 652, 544, 652, 544, 648, 548, 648, 548, 652, 544, 648, 548, 648, 548, 648, 544, 652, 544, 652, 544, 652, 544, 652, 544, 648, 548, 648, 548, 648, 544, 652, 548, 648, 544, 652, 544, 652, 544, 652, 1660, 660, 508, 652};
        irsend.sendRaw(irSignal, sizeof(irSignal) / sizeof(irSignal[0]), khz); //Note the approach used to automatically calculate the size of the array.
        
        Serial.println("Off");
      }
      else {
        lastButtonToggle = 0;
        //irsend.sendSAMSUNG(0xE0E0D02F, 32);

        uint16_t irSignal[] = {8992, 4396, 684, 1628, 656, 512, 676, 520, 676, 1636, 680, 1604, 660, 512, 676, 520, 676, 520, 676, 516, 676, 520, 676, 520, 676, 1636, 680, 488, 680, 516, 676, 520, 676, 520, 676, 520, 676, 520, 676, 520, 676, 520, 8, 156, 512, 516, 680, 1632, 684, 1604, 680, 488, 676, 520, 676, 520, 676, 520, 672, 520, 680, 1632, 656, 516, 676, 1636, 676, 492, 676, 520, 676, 1636, 680, 488, 676, 19836, 704, 488, 676, 520, 676, 520, 676, 520, 676, 520, 676, 520, 676, 520, 676, 520, 676, 516, 680, 516, 680, 1632, 656, 516, 676, 516, 680, 516, 680, 516, 676, 520, 680, 516, 676, 520, 676, 520, 676, 520, 676, 520, 676, 520, 676, 520, 676, 516, 680, 516, 676, 520, 676, 520, 676, 520, 676, 1636, 656, 1628, 680, 488, 676, 1636, 680, 42356, 6016, 2940, 680, 512, 700, 496, 676, 1636, 656, 1628, 688, 1600, 652, 516, 676, 520, 676, 520, 676, 1636, 676, 492, 676, 1636, 652, 516, 676, 520, 676, 1636, 652, 516, 676, 1636, 676};
        irsend.sendRaw(irSignal, sizeof(irSignal) / sizeof(irSignal[0]), khz); //Note the approach used to automatically calculate the size of the array.

        Serial.println("On");
      }
    }
  }
  else {
    lastButtonValue = LOW;
  }
  
}
