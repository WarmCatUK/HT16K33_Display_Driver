/* HT16K33 Display Driver Basic Example  
 by: Kris Winer
 date: May 1, 2014
 license: Beerware - Use this code however you'd like. If you 
 find it useful you can buy me a beer some time.
 
 Demonstrate basic functionality including parameterizing the register addresses, initializing the display, 
 writing integers to one or all of four seven segment LED displays.
 
 SDA and SCL should have external pull-up resistors (to 3.3V).
 10k resistors are on the HT16K33 breakout board.
 
 Hardware setup:
 HT16K33 Breakout --------- Arduino
 3.3V --------------------- 3.3V
 SDA ----------------------- A4
 SCL ----------------------- A5
 GND ---------------------- GND
 
 Note: The HT16K33 is an I2C display driver and uses the Arduino Wire library. 
 The display driver is 5V tolerant, but we are using a 3.3 V 8 MHz Pro Mini or a 3.3 V Teensy 3.1.
 We have disabled the internal pull-ups used by the Wire library in the Wire.h/twi.c utility file.
 We are also using the 400 kHz fast I2C mode by setting the TWI_FREQ  to 400000L /twi.h utility file.
 */
#include "Wire.h" 

#define HT16K33_ADDRESS         0x70
#define HT16K33_ON              0x21  // Commands
#define HT16K33_STANDBY         0x20
#define HT16K33_DISPLAYON       0x81
#define HT16K33_DISPLAYOFF      0x80
#define HT16K33_BLINKON         0x85 // Blink is off (00), 2 Hz (01), 1 Hz (10), or 0.5 Hz (11) for bits (21) 
#define HT16K33_BLINKOFF        0x81
#define HT16K33_DIM             0xE0 | 0x08 // Set dim from 0x00 (1/16th duty ccycle) to 0x0F (16/16 duty cycle)

// Arrangement for display 1 (4 digit bubble display)
// 
//               a = A0
//             _________
//            |         |
//   f = A2   |  g = A4 | b = A1
//            |_________|
//            |         |
//   e = A5   |         | c = A6
//            |_________|
//               d = A3        DP = A7

// Hookup of QDSP-6064 bubble display
//
// Display Pin          HT16K33 pin
//  1                      C0
//  2                      A5
//  3                      A6
//  4                      C2
//  5                      A7
//  6                      C3
//  7                      A4
//  8                      A3
//  9                      A2
//  10                     C1
//  11                     A1
//  12                     A0
//
// and so on... each cathode can drive two display cathodes. A total of four, 4-digit, 7-segment displays
// can be driven with this one chip with just 2 TWI wires and power and ground. That is a lot of display
// without taking up a lot of the GPIO pins.


static const byte numberTable[] =
{
  0x6F, // 0 = 0
  0x42, // 1 = 1, etc
  0x3B, // 2
  0x5B, // 3
  0x56, // 4
  0x5D, // 5
  0x7D, // 6
  0x43, // 7
  0x7F, // 8
  0x57, // 9
  0x80, // decimal point
  0x00, // blank
  0x10, // minus sign
  0x77,  // A = 13
  0x7F,  // B = 14
  0x2D,  // C = 15
  0x6F,  // D = 16
  0x3D,  // E = 17
  0x35,  // F = 18
  0x5F,  // G = 19
  0x76,   // H = 20
  0x42,   // I = 21
  0x6A,   // J = 22
  0x00,   // No K!
  0x2C,   // L = 24
  0x00,   // No M!
  0x67,   // N = 26
  0x6F,   // O = 27
  0x37,   // P = 28
  0x57,   // Q = 29
  0x77,   // R = 30
  0x5D    // S = 31
};

#define display1 1
#define display2 2
#define display3 3
#define display4 4

void setup()
{
  Wire.begin();
  Serial.begin(38400);

  initHT16K33();  // initialize display
  
  clearDsplay(display1);  // clear display 1
  clearDsplay(display2);  // clear display 2
  clearDsplay(display3);  // clear display 3
  clearDsplay(display4);  // clear display 4

  writeInteger(display1, 8888); // Test display, turn on all segments of display 1
  blinkHT16K33(2); // Blink for 2 seconds
  clearDsplay(display1);
  writeInteger(display2, 8888); // Test display, turn on all segments of display 2
  blinkHT16K33(2); // Blink for 2 seconds
  clearDsplay(display2);
  writeInteger(display3, 8888); // Test display, turn on all segments of display 3
  blinkHT16K33(2); // Blink for 2 seconds
  clearDsplay(display3);
  writeInteger(display4, 8888); // Test display, turn on all segments of display 4
  blinkHT16K33(2); // Blink for 2 seconds
  clearDsplay(display4);
  
// Test of character write; these bubble displays, like all seven-segment LED displays
// are best at displaying numbers. That doesn't mean we can't get creative and display letters also!
  writeDigit(display2, 1, 13, 0);
  writeDigit(display2, 2, 14, 0);
  writeDigit(display2, 3, 15, 0);
  writeDigit(display2, 4, 16, 0);
        
  writeDigit(display1, 1, 17, 0);
  writeDigit(display1, 2, 18, 0);
  writeDigit(display1, 3, 19, 0);
  writeDigit(display1, 4, 20, 0);  
  
  writeDigit(display4, 1, 22, 0);
  writeDigit(display4, 2, 24, 0);
  writeDigit(display4, 3, 26, 0);
  writeDigit(display4, 4, 27, 0);
        
  writeDigit(display3, 1, 28, 0);
  writeDigit(display3, 2, 29, 0);
  writeDigit(display3, 3, 30, 0);
  writeDigit(display3, 4, 31, 0);
  delay(2000);
  clearDsplay(display1);
  clearDsplay(display2);
  clearDsplay(display3);
  clearDsplay(display4);
}


void loop ()
{
   for(int j = -100; j < 100; j++) {
   writeFloat(display1, (float)(-j)/10., 1);   // write float count to display 1
   writeInteger(display2, +j);   // write integer count to display 2
   writeInteger(display3, -j);   // write integer count to display 3
   writeFloat(display4, (float)(+j)/10., 1);   // write float count to display 4
   delay(200);
   }
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++ Useful Functions++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void writeInteger(uint8_t dsply, int data)
{
  char string[10] = "";                             // define character array to hold the digits
  itoa(data, string);                               // get ascii character string representation of the integer to be displayed
  uint8_t length = strlen(string);                  // get the length of the string; number of digits in integer
  uint8_t blanks = 4 - length;                      // how many blanks do we have?

  if (length > 4) return;                           // if length greater than 4 digits we can't display on a four-digit display!

  for (uint8_t digit = 0; digit < blanks; digit++)  // scroll through each digit to determine what to write to the display
  {
      writeDigit(dsply, digit + 1, 11, 0);          // clear digit wherever there are blanks
  }

  for (uint8_t digit = 0; digit < 4; digit++)       // scroll through each digit to determine what to write to the display
  {
      char ch = string[digit];                      // get the ascii character of the next string segment

      if (ch == '-') {
      writeDigit(dsply, digit + 1 + blanks, 12, 0); // check if negative sign needed
      } 
      else {                                        // character must be a digit
      ch -= '0';                                    // convert it to an integer
      writeDigit(dsply, digit + 1 + blanks, ch, 0); // write it to the display; right justify the integer
      } 
  }
}

void writeFloat(uint8_t dsply, float data, uint8_t dp)
{
  char string[10] = "";  // define character array to hold the digits
  int datanew = 0;
  
  switch (dp)
  {
    case 0:
    datanew = (int )(1.*data);
    break;
 
    case 1:
    datanew = (int )(10.*data);
    break;

    case 2:
    datanew = (int )(100.*data);
    break;
 
    case 3:
    datanew = (int )(1000.*data);
    break;
   }
   
  
  itoa(datanew, string);                                    // get ascii character string representation of the integer to be displayed
  uint8_t length = strlen(string);                          // get the length of the string; number of digits in integer
  uint8_t blanks = 4 - length;                              // how many blanks do we have?

  if (length > 4) return;                                   // if length greater than 4 digits we can't display on a four-digit display!

// scroll through each digit to determine what to write to the display
for (uint8_t digit = 0; digit < blanks; digit++)            // first the blanks
  {
          if( (digit + 1) == (4 - dp) ) {                   // handle special case where blank coincides with decimal point
            writeDigit(dsply, digit + 1, 0, 0x80);          // add leading zero before decimal place
          }
          else {
            writeDigit(dsply, digit + 1, 11, 0x00);         // otherwise clear digit wherever there are blanks
          }
  }

  for (uint8_t digit = 0; digit < 4; digit++)               // now the characters to determine what to write to the display
  {
      char ch = string[digit];                              // get the ascii character of the next string segment

      if (ch == '-') {
        if((digit + 1 + blanks) == (4 - dp) ) {
          writeDigit(dsply, digit + 1 + blanks,  0, 0x80);  // check if negative sign needed, add a decimal point
          writeDigit(dsply, digit + 0 + blanks, 12, 0x00);  // add a leading zero
        }
        else {
          writeDigit(dsply, digit + 1 + blanks, 12, 0x00);  // check if negative sign needed, no decimal point
        }
        }
      else  {                                               // character must be a digit
        ch -= '0';                                          // convert it to an integer
        if((digit + 1 + blanks) == (4 - dp) ) {
          writeDigit(dsply, digit + 1 + blanks, ch, 0x80);  // write it to the display with decimal point; right justify the integer
        } 
        else {
          writeDigit(dsply, digit + 1 + blanks, ch, 0x00);  // write it to the display; right justify the integer
        } 
     }
  }
}
  

void writeDigit(uint8_t dsply, uint8_t digit, uint8_t data, uint8_t dp) 
{
if(dsply == 1) {
  digit = (digit - 1)*2 + 0; 
} 
if(dsply == 2) {
  digit = (digit - 1)*2 + 8 ;
}
if(dsply == 3) {
  digit = (digit - 1)*2 + 1;
}
if(dsply == 4) {
  digit = (digit - 1)*2 + 9;
}
writeByte(HT16K33_ADDRESS, digit, numberTable[data] | dp);
}


void clearDsplay(int dsply) 
{
  for(int i = 0; i < 8; i++)  {
  writeDigit(dsply, i, 11, 0);  // Clear display, 11 is blank in the numberTable above
  }
}


void initHT16K33()
{
  writeCommand(HT16K33_ADDRESS, HT16K33_ON);         // Turn on system oscillator
  writeCommand(HT16K33_ADDRESS, HT16K33_DISPLAYON);  // Display on
  writeCommand(HT16K33_ADDRESS, HT16K33_DIM);        // Set brightness

}


void blinkHT16K33(int time) 
{
  writeCommand(HT16K33_ADDRESS, HT16K33_BLINKON);  // Turn on blink
  delay(1000*time);
  writeCommand(HT16K33_ADDRESS, HT16K33_BLINKOFF);  // Turn on blink
}


 /* itoa:  convert n to characters in s */
 void itoa(int n, char s[])
 {
     int i, sign;
 
     if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
     i = 0;
     do {       /* generate digits in reverse order */
         s[i++] = n % 10 + '0';   /* get next digit */
     } while ((n /= 10) > 0);     /* delete it */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
 } 
 
 /* reverse:  reverse string s in place */
 void reverse(char s[])
 {
     int i, j;
     char c;
 
     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }

        // Wire.h read and write protocols
        void writeCommand(uint8_t address, uint8_t command)
  {
	Wire.beginTransmission(address);  // Initialize the Tx buffer
	Wire.write(command);              // Put command in Tx buffer
	Wire.endTransmission();           // Send the Tx buffer
}
        void writeByte(uint8_t address, uint8_t subAddress, uint8_t data)
{
	Wire.beginTransmission(address);  // Initialize the Tx buffer
	Wire.write(subAddress);           // Put slave register address in Tx buffer
	Wire.write(data);                 // Put data in Tx buffer
	Wire.endTransmission();           // Send the Tx buffer
}

        uint8_t readByte(uint8_t address, uint8_t subAddress)
{
	uint8_t data; // `data` will store the register data	 
	Wire.beginTransmission(address);         // Initialize the Tx buffer
	Wire.write(subAddress);	                 // Put slave register address in Tx buffer
	Wire.endTransmission(false);             // Send the Tx buffer, but send a restart to keep connection alive
	Wire.requestFrom(address, (uint8_t) 1);  // Read one byte from slave register address 
	data = Wire.read();                      // Fill Rx buffer with result
	return data;                             // Return data read from slave register
}

        void readBytes(uint8_t address, uint8_t subAddress, uint8_t count, uint8_t * dest)
{  
	Wire.beginTransmission(address);   // Initialize the Tx buffer
	Wire.write(subAddress);            // Put slave register address in Tx buffer
	Wire.endTransmission(false);       // Send the Tx buffer, but send a restart to keep connection alive
	uint8_t i = 0;
        Wire.requestFrom(address, count);  // Read bytes from slave register address 
	while (Wire.available()) {
        dest[i++] = Wire.read(); }         // Put read results in the Rx buffer
}
