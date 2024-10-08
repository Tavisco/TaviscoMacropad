#include "hardware/gpio.h" // gpio_*

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

class KeyBoard
{
private:
	// ===========================================================================
	const static size_t num_pins = 10;
	const uint8_t pins[num_pins] = {
		{GPIO_ENCODER_SW},
		{GPIO_KEY_1},
		{GPIO_KEY_2},
		{GPIO_KEY_3},
		{GPIO_KEY_4},
		{GPIO_KEY_5},
		{GPIO_KEY_6},
		{GPIO_KEY_7},
		{GPIO_KEY_8},
		{GPIO_KEY_9}
	};
	// ===========================================================================

public:
	uint8_t keys_pressed[6] = {0}; // we can send max 6 keycodes per hid-report

	KeyBoard()
	{
		// set all pins to pulled up inputs
		for (size_t i = 0; i < num_pins; i++)
		{
			gpio_init(pins[i]);
			gpio_pull_up(pins[i]);
			gpio_set_dir(pins[i], GPIO_IN);
		}
	}

	virtual ~KeyBoard() {}

	bool update()
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
			int pin_state = gpio_get(pins[i]); // read pin for this key
			if (pin_state == 0) // pressed
			{
				keys_pressed[index] = pins[i]; // set keycode
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