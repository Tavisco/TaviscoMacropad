#include "hardware/gpio.h" // gpio_*
#include "class/hid/hid.h" // HID_KEY_*

#define GPIO_KEY_9		22
#define GPIO_KEY_8		20
#define GPIO_KEY_7		13
#define GPIO_KEY_6		21
#define GPIO_KEY_5		19
#define GPIO_KEY_4		14
#define GPIO_KEY_3		16
#define GPIO_KEY_2		18
#define GPIO_KEY_1		15
#define GPIO_ENCODER_SW	26

#define NUM_KEYS 10

const uint8_t gpio_pins[NUM_KEYS] = {26, 13, 20, 22, 14, 19, 21, 15, 18, 16};

// Custom mappings, each item in the array is a button. The first is the knob, then  7 8 9 4 5 6 1 2 3 (picture the keys position in the numpad)
const uint8_t keypad_keys[NUM_KEYS] = {HID_KEY_0, HID_KEY_7, HID_KEY_8, HID_KEY_9, HID_KEY_4, HID_KEY_5, HID_KEY_6, HID_KEY_1, HID_KEY_2, HID_KEY_3};
const uint8_t multimedia_keys[NUM_KEYS] = {0, 0, 0, 0, HID_USAGE_CONSUMER_MUTE, HID_KEY_5, 0, HID_USAGE_CONSUMER_SCAN_PREVIOUS, HID_USAGE_CONSUMER_PLAY_PAUSE, HID_USAGE_CONSUMER_SCAN_NEXT};
const uint8_t osu_keys[NUM_KEYS] = {0, HID_KEY_ESCAPE, HID_KEY_F8, HID_KEY_GRAVE, 0, 0, 0, HID_KEY_Z, HID_KEY_X, HID_KEY_Z};
const uint8_t arrowpad_keys[NUM_KEYS] = {0, HID_KEY_ESCAPE, HID_KEY_ARROW_UP, 0, HID_KEY_ARROW_LEFT, HID_KEY_ARROW_DOWN, HID_KEY_ARROW_RIGHT, HID_KEY_CONTROL_LEFT, 0, HID_KEY_SPACE};
const uint8_t wasd_keys[NUM_KEYS] = {0, HID_KEY_ESCAPE, HID_KEY_W, 0, HID_KEY_A, HID_KEY_S, HID_KEY_D, HID_KEY_CONTROL_LEFT, 0, HID_KEY_SPACE};
const uint8_t ide_keys[NUM_KEYS] = {0, HID_KEY_F13, HID_KEY_F14, HID_KEY_F15, HID_KEY_F16, HID_KEY_F17, HID_KEY_F18, HID_KEY_F19, HID_KEY_F20, HID_KEY_F21};

class KeyBoard {
public:
    uint8_t keys_pressed[6] = {0};

    KeyBoard() {
        for (uint8_t i = 0; i < NUM_KEYS; i++) {
            gpio_init(gpio_pins[i]);
            gpio_pull_up(gpio_pins[i]);
            gpio_set_dir(gpio_pins[i], GPIO_IN);
        }
    }

    bool update(uint8_t mode) {
        uint8_t index = 0;
        bool pressed = false;
        memset(keys_pressed, 0, sizeof(keys_pressed));

        for (uint8_t i = 0; i < NUM_KEYS; i++) {
            if (!gpio_get(gpio_pins[i])) {
                keys_pressed[index++] = get_key_for_mode(mode, i);
                pressed = true;
                if (index == 6) break;
            }
        }
        return pressed;
    }

private:
    uint8_t get_key_for_mode(uint8_t mode, uint8_t index) {
        switch (mode) {
            case MODE_NUMPAD: 
                return keypad_keys[index];
            case MODE_MULTIMEDIA: 
                return multimedia_keys[index];
            case MODE_OSU:
                return osu_keys[index];
			case MODE_ARROWPAD:
                return arrowpad_keys[index];
            case MODE_WASD:
                return wasd_keys[index];
            case MODE_IDE:
            case MODE_IDE_2:
                return ide_keys[index];
            default:
                return gpio_pins[index];
        }
    }
};
