#include <USBComposite.h>

const int USE_MOUSE = 1;

// mouse initial location
const int CURSOR_X = 126;
const int CURSOR_Y = 50;
int mouse_init_done = 0;

const unsigned long MAX_WORK_TIME_MS = 20 * 60 * 1000;


/* See ~/Arduino/libraries/Keyboard/src/Keyboard.h */
/* #define KEY_PAGE_UP     0xD3 */
/* #define KEY_PAGE_DOWN   0xD6 */
/* #define KEY_UP_ARROW    0xDA */
/* #define KEY_DOWN_ARROW    0xD9 */
/* #define KEY_LEFT_ARROW    0xD8 */
/* #define KEY_RIGHT_ARROW   0xD7 */
const unsigned int KC = 0xD7;


// intervals between two clicks
const unsigned long MIN_RAND_WAIT_MS = 1000;
const unsigned long MAX_RAND_WAIT_MS = 6000;

unsigned long prevTs = 0;
unsigned long interval = 1000;
unsigned long ledOffTs = 0;
unsigned long workTs = 0;
int ledState = 0; // default led off

USBHID HID;
HIDKeyboard Keyboard(HID);
HIDMouse Mouse(HID); 
 
void setup() {
    workTs = millis() + MAX_WORK_TIME_MS;
    pinMode(PC13, OUTPUT);
    digitalWrite(PC13, HIGH); // turn off led
  
    HID.begin(HID_KEYBOARD_MOUSE);
    
    // while(!USBComposite);
}


void loop() {
    unsigned long now = millis();

    if (now > workTs || now < ledOffTs){ // turn led on
        if (!ledState){
            digitalWrite(PC13, LOW);
            ledState = 1;
        }
    }
    else { 
        if (ledState){
            digitalWrite(PC13, HIGH);
            ledState = 0;
        }
    }
    
    if (now - prevTs > interval) {
        ledOffTs = now + 500;
        if (USE_MOUSE){
            if (!mouse_init_done){
                Mouse.move(CURSOR_X, CURSOR_Y);
                mouse_init_done = 1;
            }
            
            // https://www.arduino.cc/reference/en/language/functions/usb/mouse/mousemove/
            Mouse.move(random(-2, 4), random(-5, 5));
            /* Mouse.move(CURSOR_X, CURSOR_Y);  */
            Mouse.click();
        }else{
            Keyboard.write(KC);
        }
    
        prevTs = now;
        interval = random(MIN_RAND_WAIT_MS, MAX_RAND_WAIT_MS);
    }
}
