// STM32自动翻页打卡
// 支持继电器，键盘或鼠标模式。
// 使用继电器模式时，输出连接至PA8。

#include <USBComposite.h>

// 是否使用继电器。 1：使用继电器，0：使用键盘或鼠标
const int USE_RELAY = 0;
// 是否使用鼠标，USE_RELAY=1时本设置无效。 1：使用鼠标，0：使用键盘
const int USE_MOUSE = 1;

// 使用鼠标点击时，光标初始位置，根据手机屏幕调整
const int CURSOR_X = 126;
const int CURSOR_Y = 50;
int mouse_init_done = 0;

// 打卡时长
const unsigned long MAX_WORK_TIME_MS = 30 * 60 * 1000;

/* 键盘按键定义，可以参考 ~/Arduino/libraries/Keyboard/src/Keyboard.h */
/* #define KEY_PAGE_UP     0xD3 */
/* #define KEY_PAGE_DOWN   0xD6 */
/* #define KEY_UP_ARROW    0xDA */
/* #define KEY_DOWN_ARROW    0xD9 */
/* #define KEY_LEFT_ARROW    0xD8 */
/* #define KEY_RIGHT_ARROW   0xD7 */
// 可根据软件调整为上面的其他值。
const unsigned int KC = 0xD7;


// 两次翻页的最小间隔（毫秒）
const unsigned long MIN_RAND_WAIT_MS = 2000;
// 两次翻页的最大间隔（毫秒）
const unsigned long MAX_RAND_WAIT_MS = 5000;

// 翻页时继电器最短开启时常
const unsigned long MIN_RELAY_ON_MS = 150;
// 翻页时继电器最大开启时常
const unsigned long MAX_RELAY_ON_MS = 160;

unsigned long prevTs = 0;
unsigned long interval = 1000;
unsigned long ledOffTs = 0;
unsigned long workTs = 0;
unsigned long switchOffTs = 0;
int ledState = 0; // default led off
int switchState = 0; 

USBHID HID;
HIDKeyboard Keyboard(HID);
HIDMouse Mouse(HID); 
 
void setup() {
    workTs = millis() + MAX_WORK_TIME_MS;
    pinMode(PC13, OUTPUT);
    digitalWrite(PC13, HIGH); // set high to turn off led

    if (USE_RELAY){
        pinMode(PA8, OUTPUT);
        digitalWrite(PA8, HIGH);     
    }
  
    HID.begin(HID_KEYBOARD_MOUSE);
}


void clickOrTap(){
    if (USE_MOUSE){
        if (!mouse_init_done){
            Mouse.move(CURSOR_X, CURSOR_Y);
            mouse_init_done = 1;
        }
            
        // https://www.arduino.cc/reference/en/language/functions/usb/mouse/mousemove/
        Mouse.move(random(-2, 4), random(-5, 5));
        Mouse.click();
    }else{
        Keyboard.write(KC);
    } 
}

void loop() {
    unsigned long now = millis();

    // 翻页时LED开启。打卡时长到时开启。
    if (now > workTs || now < ledOffTs){
        if (!ledState){
            digitalWrite(PC13, LOW);
            ledState = 1;
        }
    } else if (ledState){ 
        digitalWrite(PC13, HIGH);
        ledState = 0;
    }

    if (USE_RELAY){
        // 开启继电器 (低电平触发)
        if (now < switchOffTs){
            if (!switchState){
                digitalWrite(PA8, LOW);
                switchState = 1;
            }
        }else if (switchState){
            digitalWrite(PA8, HIGH);
            switchState = 0;
        }
    }


    // mouse click and key tap
    if (now - prevTs > interval) {
        ledOffTs = now + 500;
        switchOffTs = now + random(MIN_RELAY_ON_MS, MAX_RELAY_ON_MS);

        if (!USE_RELAY){
            // 点击鼠标或按下按键
            clickOrTap();   
        }
    
        prevTs = now;
        interval = random(MIN_RAND_WAIT_MS, MAX_RAND_WAIT_MS);
    }
}
