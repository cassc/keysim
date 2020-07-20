
#include <USBComposite.h>

const int USE_MOUSE = 0;

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
const unsigned long MAX_RAND_WAIT_MS = 10000;

unsigned long prevTs = 0;
unsigned long interval = 1000;


USBHID HID;
HIDKeyboard Keyboard(HID);
HIDMouse Mouse(HID); 
 
void setup() {
    HID.begin(HID_KEYBOARD_MOUSE);
    // while(!USBComposite);
}


void loop() {
    unsigned long now = millis();
    if (now - prevTs > interval) {
        if (USE_MOUSE){
            // https://www.arduino.cc/reference/en/language/functions/usb/mouse/mousemove/
            Mouse.click();
        }else{
            Keyboard.write(KC);
        }
    
        prevTs = now;
        interval = random(MIN_RAND_WAIT_MS, MAX_RAND_WAIT_MS);
    }
}
