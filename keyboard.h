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

struct PinKey
{
	const uint8_t gpio_pin; // pico pin number
	const uint8_t keypad_key; // HID_KEY_*
	const uint8_t multimedia_key;
	const uint8_t osu_key;
};

class KeyBoard
{
private:
	// ===========================================================================
	const static size_t num_pins = 10;
	const PinKey pins[num_pins] = {
		{GPIO_ENCODER_SW, HID_KEY_0, 0, 0},
		{GPIO_KEY_7, HID_KEY_7, 0, HID_KEY_ESCAPE}, {GPIO_KEY_8, HID_KEY_8, 0, HID_KEY_F8}, {GPIO_KEY_9, HID_KEY_9, 0, HID_KEY_GRAVE},
		{GPIO_KEY_4, HID_KEY_4, HID_USAGE_CONSUMER_MUTE, 0}, {GPIO_KEY_5, HID_KEY_5, GPIO_KEY_5, 0}, {GPIO_KEY_6, HID_KEY_6, 0, 0},
		{GPIO_KEY_1, HID_KEY_1, HID_USAGE_CONSUMER_SCAN_PREVIOUS, HID_KEY_Z}, {GPIO_KEY_2, HID_KEY_2, HID_USAGE_CONSUMER_PLAY_PAUSE, HID_KEY_X}, {GPIO_KEY_3, HID_KEY_3, HID_USAGE_CONSUMER_SCAN_NEXT, HID_KEY_Z},
	};
	// ===========================================================================

	uint8_t get_key_for_mode(uint8_t current_mode, PinKey pin_key)
	{
		switch (current_mode)
		{
		case MODE_KEYPAD:
			return pin_key.keypad_key;
			break;
		case MODE_MULTIMEDIA:
			return pin_key.multimedia_key;
		case MODE_OSU:
			return pin_key.osu_key;
		
		default:
			return pin_key.gpio_pin;
			break;
		}
	}

public:
	uint8_t keys_pressed[6] = {0}; // we can send max 6 keycodes per hid-report

	KeyBoard()
	{
		// set all pins to pulled up inputs
		for (size_t i = 0; i < num_pins; i++)
		{
			gpio_init(pins[i].gpio_pin);
			gpio_pull_up(pins[i].gpio_pin);
			gpio_set_dir(pins[i].gpio_pin, GPIO_IN);
		}
	}

	virtual ~KeyBoard() {}

	bool update(uint8_t current_mode)
	{
		// clear keycodes
		for (size_t i = 0; i < 6; i++)
		{
			keys_pressed[i] = 0;
		}

		// read pins and set max 6 keycodes
		uint8_t index = 0;
		bool pressed = false;
		for (size_t i = 0; i < num_pins; i++)
		{
			int pin_state = gpio_get(pins[i].gpio_pin); // read pin for this key
			if (pin_state == 0) // pressed
			{
				keys_pressed[index] = get_key_for_mode(current_mode, pins[i]); // set keycode
				pressed = true;
				index++;
				if (index == 6) // max is 6 key presses
				{
					break;
				}
			}
		}

		return pressed;
	}
};