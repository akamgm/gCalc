#include "SoftwareSerial.h"
#include <Keypad.h>

// Control values for Parallax 2x16 serial LCD, #27977-RT
// Data sheet: http://parallax.com/downloads/parallax-serial-lcd-product-guide
#define CLEAR 12
#define BACKLIGHT_ON 17
#define CURSOR_OFF 22

// Set up the keyboard map - for a Kootek 4x4 keypad
const byte rows = 4;
const byte cols = 4;

char keys[rows][cols] = {
  {'1', '2', '3', '+'},
  {'4', '5', '6', '-'},
  {'7', '8', '9', 'x'},
  {'C', '0', '=', '/'}
};

byte colPins[rows] = {25, 24, 23, 22};
byte rowPins[cols] = {29, 28, 27, 26};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, rows, cols);

// Set up serial out to display: RX3, TX3 on the v1 ADK
SoftwareSerial screenOut(15,14);

long currentVal = 0;
long storedVal = 0;
char operation;

void setup() {
  screenOut.begin(9600);
  delay(100);
  screenOut.write(CURSOR_OFF); 
  screenOut.write(CLEAR);
  screenOut.write(BACKLIGHT_ON);
  screenOut.println(currentVal, DEC);
}

long execute(long v1, long v2, char op) {
 switch (op) {
   case '+':
     return v1 + v2;
   case '-':
     return v1 - v2;
   case 'x':
     return v1 * v2;
   case '/':
     return v1 / v2;
   default:
     return -1;
 } 
}

/* give visual feedback by clearing the screen, pausing,
** then displaying <val>
*/
void bounceScreen(long val) {
  screenOut.write(CLEAR);
  delay(100);
  screenOut.println(val, DEC);
}

void loop() {
  char key = keypad.waitForKey();
  switch (key) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      if (!currentVal) {
        screenOut.write(CLEAR); 
      }
      currentVal *= 10;
      currentVal += (int)(key - '0');
      screenOut.println(currentVal, DEC);
      break;
    case '+':
    case '-':
    case 'x':
    case '/':
      bounceScreen(currentVal);      
      operation = key;
      storedVal = currentVal;
      currentVal = 0;
      break;
    case '=':
      storedVal = execute(storedVal, currentVal, operation);
      bounceScreen(storedVal);
      currentVal = storedVal;
      break;
    case 'C':
      storedVal = 0;
      currentVal = 0;  
      bounceScreen(0);
    default:
      break;
  }
}

