/*
 * IRremote: IRrecvDemo - demonstrates receiving IR codes with IRrecv
 * An IR detector/demodulator must be connected to the input RECV_PIN.
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 */

#include <IRremote.h>

int RECV_PIN = 2;
int BUTTON_PIN = 8;

IRrecv irrecv(RECV_PIN);
IRsend irsend;
int khz = 38; // 38kHz carrier frequency for the NEC protocol
String incomingSerial;
int lastButtonToggle = 0;
int lastButtonValue = LOW;

int resultsArraySize = 0;
decode_results resultsArray[10];
unsigned long irTimePassed = 0;

void setup()
{
  Serial.begin(115200);
  // In case the interrupt driver crashes on setup, give a clue
  // to the user what's going on.
  Serial.println("Enabling IRin");
  irrecv.enableIRIn(); // Start the receiver
  Serial.println("Enabled IRin");
  pinMode(BUTTON_PIN, INPUT);
}


/* Tadiran on - 9810060A  NEC
 * Tadiran off - 8810040A
 * 
 * /
 */
void loop() {

  if (Serial.available() > 0) {
    // read the incoming:
    incomingSerial = Serial.readString();
    // say what you got:
    Serial.println(incomingSerial);
        
    if (incomingSerial == 'on') {
      irsend.sendNEC(0x981006FF, 32);
    }
    else if (incomingSerial == 'off') {
      irsend.sendNEC(0x881004FF, 32);
    }
  }

  if(digitalRead(BUTTON_PIN) == HIGH) {
    if(lastButtonValue == LOW) {
      lastButtonValue = HIGH;

      if(lastButtonToggle == 0) {
        lastButtonToggle = 1;
        //irsend.sendSAMSUNG(0xE0E0E01F, 32);
        //irsend.sendNEC(0x9890060A, 32);

        //unsigned int  irSignal[139] = {4556, 4524, 568, 1716, 540, 1712, 544, 1716, 540, 560, 596, 532, 596, 532, 596, 532, 600, 528, 600, 1684, 544, 1712, 544, 1712, 540, 560, 596, 532, 596, 532, 600, 528, 600, 528, 600, 1684, 544, 1712, 540, 1716, 544, 556, 596, 532, 600, 528, 600, 528, 600, 528, 600, 528, 600, 528, 600, 532, 596, 1684, 544, 1712, 544, 1712, 544, 1712, 544, 1712, 544, 47124, 4580, 4504, 572, 1712, 544, 1712, 544, 1712, 544, 556, 596, 532, 600, 528, 600, 528, 600, 528, 600, 1684, 544, 1712, 544, 1712, 540, 560, 600, 528, 600, 528, 600, 528, 600, 528, 600, 1684, 544, 1712, 544, 1708, 544, 560, 596, 532, 596, 532, 596, 532, 596, 532, 600, 528, 600, 528, 600, 528, 600, 1684, 544, 1712, 544, 1712, 540, 1716, 540, 1712, 544};
        unsigned int  irSignal[] = {8968, 4420, 680, 1632, 652, 516, 652, 544, 652, 544, 652, 1660, 652, 516, 652, 544, 652, 544, 648, 1664, 676, 492, 652, 544, 652, 1660, 676, 492, 652, 544, 648, 548, 648, 548, 648, 548, 648, 548, 648, 544, 652, 544, 652, 544, 652, 1660, 676, 492, 652, 544, 652, 544, 648, 548, 648, 548, 648, 548, 648, 1664, 676, 492, 648, 1664, 676, 492, 652, 544, 648, 1664, 676, 492, 648, 19864, 680, 512, 652, 544, 652, 544, 652, 544, 648, 548, 652, 544, 648, 548, 648, 548, 648, 544, 652, 544, 652, 544, 652, 544, 652, 544, 648, 548, 648, 548, 652, 544, 648, 548, 648, 548, 648, 544, 652, 544, 652, 544, 652, 544, 652, 544, 648, 548, 648, 548, 648, 544, 652, 548, 648, 544, 652, 544, 652, 544, 652, 1660, 660, 508, 652};
        irsend.sendRaw(irSignal, sizeof(irSignal) / sizeof(irSignal[0]), khz); //Note the approach used to automatically calculate the size of the array.

        Serial.println("Off");
      }
      else {
        lastButtonToggle = 0;
        //irsend.sendSAMSUNG(0xE0E0D02F, 32);
        //irsend.sendNEC(0x8890040A, 32);

        unsigned int  irSignal[] = {8992, 4396, 684, 1628, 656, 512, 676, 520, 676, 1636, 680, 1604, 660, 512, 676, 520, 676, 520, 676, 516, 676, 520, 676, 520, 676, 1636, 680, 488, 680, 516, 676, 520, 676, 520, 676, 520, 676, 520, 676, 520, 676, 520, 8, 156, 512, 516, 680, 1632, 684, 1604, 680, 488, 676, 520, 676, 520, 676, 520, 672, 520, 680, 1632, 656, 516, 676, 1636, 676, 492, 676, 520, 676, 1636, 680, 488, 676, 19836, 704, 488, 676, 520, 676, 520, 676, 520, 676, 520, 676, 520, 676, 520, 676, 520, 676, 516, 680, 516, 680, 1632, 656, 516, 676, 516, 680, 516, 680, 516, 676, 520, 680, 516, 676, 520, 676, 520, 676, 520, 676, 520, 676, 520, 676, 520, 676, 516, 680, 516, 676, 520, 676, 520, 676, 520, 676, 1636, 656, 1628, 680, 488, 676, 1636, 680, 42356, 6016, 2940, 680, 512, 700, 496, 676, 1636, 656, 1628, 688, 1600, 652, 516, 676, 520, 676, 520, 676, 1636, 676, 492, 676, 1636, 652, 516, 676, 520, 676, 1636, 652, 516, 676, 1636, 676};
        irsend.sendRaw(irSignal, sizeof(irSignal) / sizeof(irSignal[0]), khz); //Note the approach used to automatically calculate the size of the array.

        Serial.println("V-");
      }
    }
  }
  else {
    lastButtonValue = LOW;
  }

  decode_results results;        // Somewhere to store the results
  if (irrecv.decode(&results)) {  // Grab an IR code
    resultsArraySize = resultsArraySize + 1;
    resultsArray[resultsArraySize - 1] = results;

    Serial.println("New IR");
    irTimePassed = millis();

    irrecv.resume();              // Prepare for the next value
  }

  if (millis() - irTimePassed > 3000) {
    for (int i = 0; i < resultsArraySize; i++) {
      decode_results result = resultsArray[i];
      //dumpInfo(&result);           // Output the results
      dumpRaw(&result);            // Output the results in RAW format
      dumpCode(&result);           // Output the results as source code
      Serial.println("******************************************");           // Blank line between entries
    }

    resultsArraySize = 0;
  }

  /*if (irrecv.decode(&results)){
        Serial.println(results.value, HEX);
        
        switch (results.decode_type){
            case NEC: Serial.println("NEC"); break ;
            case SONY: Serial.println("SONY"); break ;
            case RC5: Serial.println("RC5"); break ;
            case RC6: Serial.println("RC6"); break ;
            case DISH: Serial.println("DISH"); break ;
            case SHARP: Serial.println("SHARP"); break ;
            case JVC: Serial.println("JVC"); break ;
            case SANYO: Serial.println("SANYO"); break ;
            case MITSUBISHI: Serial.println("MITSUBISHI"); break ;
            case SAMSUNG: Serial.println("SAMSUNG"); break ;
            case LG: Serial.println("LG"); break ;
            case WHYNTER: Serial.println("WHYNTER"); break ;
            case AIWA_RC_T501: Serial.println("AIWA_RC_T501"); break ;
            case PANASONIC: Serial.println("PANASONIC"); break ;
            case DENON: Serial.println("DENON"); break ;
          default:
            case UNKNOWN: Serial.println("UNKNOWN"); break ;
          }

    Serial.print(" (");
    Serial.print(results.bits, DEC);
    Serial.println(" bits)"); 
          
        irrecv.resume();
  }*/
}

//+=============================================================================
// Display IR code
//
void  ircode (decode_results *results)
{
  // Panasonic has an Address
  if (results->decode_type == PANASONIC) {
    Serial.print(results->address, HEX);
    Serial.print(":");
  }

  // Print Code
  Serial.print(results->value, HEX);
}

//+=============================================================================
// Display encoding type
//
void  encoding (decode_results *results)
{
  switch (results->decode_type) {
    default:
    case UNKNOWN:      Serial.print("UNKNOWN");       break ;
    case NEC:          Serial.print("NEC");           break ;
    case SONY:         Serial.print("SONY");          break ;
    case RC5:          Serial.print("RC5");           break ;
    case RC6:          Serial.print("RC6");           break ;
    case DISH:         Serial.print("DISH");          break ;
    case SHARP:        Serial.print("SHARP");         break ;
    case JVC:          Serial.print("JVC");           break ;
    case SANYO:        Serial.print("SANYO");         break ;
    case MITSUBISHI:   Serial.print("MITSUBISHI");    break ;
    case SAMSUNG:      Serial.print("SAMSUNG");       break ;
    case LG:           Serial.print("LG");            break ;
    case WHYNTER:      Serial.print("WHYNTER");       break ;
    case AIWA_RC_T501: Serial.print("AIWA_RC_T501");  break ;
    case PANASONIC:    Serial.print("PANASONIC");     break ;
    case DENON:        Serial.print("Denon");         break ;
  }
}

//+=============================================================================
// Dump out the decode_results structure.
//
void  dumpInfo (decode_results *results)
{
  // Check if the buffer overflowed
  if (results->overflow) {
    Serial.println("IR code too long. Edit IRremoteInt.h and increase RAWBUF");
    return;
  }

  // Show Encoding standard
  Serial.print("Encoding  : ");
  encoding(results);
  Serial.println("");

  // Show Code & length
  Serial.print("Code      : ");
  ircode(results);
  Serial.print(" (");
  Serial.print(results->bits, DEC);
  Serial.println(" bits)");
}

//+=============================================================================
// Dump out the decode_results structure.
//
void  dumpRaw (decode_results *results)
{
  // Print Raw data
  Serial.print("Timing[");
  Serial.print(results->rawlen-1, DEC);
  Serial.println("]: ");

  for (int i = 1;  i < results->rawlen;  i++) {
    unsigned long  x = results->rawbuf[i] * USECPERTICK;
    if (!(i & 1)) {  // even
      Serial.print("-");
      if (x < 1000)  Serial.print(" ") ;
      if (x < 100)   Serial.print(" ") ;
      Serial.print(x, DEC);
    } else {  // odd
      Serial.print("     ");
      Serial.print("+");
      if (x < 1000)  Serial.print(" ") ;
      if (x < 100)   Serial.print(" ") ;
      Serial.print(x, DEC);
      if (i < results->rawlen-1) Serial.print(", "); //',' not needed for last one
    }
    if (!(i % 8))  Serial.println("");
  }
  Serial.println("");                    // Newline
}

//+=============================================================================
// Dump out the decode_results structure.
//
void  dumpCode (decode_results *results)
{
  // Start declaration
  Serial.print("unsigned int  ");          // variable type
  Serial.print("rawData[");                // array name
  Serial.print(results->rawlen - 1, DEC);  // array size
  Serial.print("] = {");                   // Start declaration

  // Dump data
  for (int i = 1;  i < results->rawlen;  i++) {
    Serial.print(results->rawbuf[i] * USECPERTICK, DEC);
    if ( i < results->rawlen-1 ) Serial.print(","); // ',' not needed on last one
    if (!(i & 1))  Serial.print(" ");
  }

  // End declaration
  Serial.print("};");  // 

  // Comment
  Serial.print("  // ");
  encoding(results);
  Serial.print(" ");
  ircode(results);

  // Newline
  Serial.println("");

  // Now dump "known" codes
  if (results->decode_type != UNKNOWN) {

    // Some protocols have an address
    if (results->decode_type == PANASONIC) {
      Serial.print("unsigned int  addr = 0x");
      Serial.print(results->address, HEX);
      Serial.println(";");
    }

    // All protocols have data
    Serial.print("unsigned int  data = 0x");
    Serial.print(results->value, HEX);
    Serial.println(";");
  }
}

