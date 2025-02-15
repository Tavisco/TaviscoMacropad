#include <stdio.h>
#include <tusb.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "TaviscoMacropad.hpp"
#include "libs/SSD1306_OLED_PICO/include/ssd1306/SSD1306_OLED.hpp"
#include "usb_descriptors.h"
#include "bsp/board.h"
#include "keyboard.h"
#include "rotary_encoder.h"

const char *modes[]= {"IDE (1/2)", "Git", "Docker", "Numpad", "IoT", "Osu!", "Arrowpad", "WASD", "Multimedia", "IDE (2/2)"};

uint8_t screen_buffer[OLED_SIZE]; // Define a buffer to cover whole screen  128 * 64/8
SSD1306 oled_screen(OLED_WIDTH, OLED_HEIGHT);
int8_t current_mode = 0;
KeyBoard keyboard;
rotary_encoder_t encoder;
uint32_t last_interaction_ms = 0;
bool is_in_screensaver_mode = false;
bool usb_mounted = false;

void keys_task(void);

void draw_key_lines(void) {
    oled_screen.drawLine(43, 16, 43, 64, WHITE);
    oled_screen.drawLine(85, 16, 85, 64, WHITE);
    oled_screen.drawLine(0, 32, 128, 32, WHITE);
    oled_screen.drawLine(0, 48, 128, 48, WHITE);
}

void draw_keypad(const char *keys[3][3]) {
    int x_positions[3] = {0, 43, 85};
    int y_positions[3] = {20, 37, 53};
    // oled_screen.setFont(pFontMega);
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            if (keys[row][col]) {
                int text_width = strlen(keys[row][col]) * 6; // Fixed 6x8 font
                int x = x_positions[col] + (43 - text_width) / 2;
                oled_screen.writeCharString(x, y_positions[row], (char *)keys[row][col]);
            }
        }
    }
    // oled_screen.setFont(pFontDefault);
}

void draw_current_mode(void) {
    oled_screen.fillRect(0, 0, 64, 14, BLACK);
    oled_screen.writeCharString(0, 3, (char *)modes[current_mode]);
    oled_screen.fillRect(0, 16, 128, 48, BLACK);

	if (current_mode == MODE_NUMPAD) {
		oled_screen.setFont(pFontMega);
		oled_screen.writeCharString(35, 16, (char *)"7");
		oled_screen.writeCharString(55, 16, (char *)"8");
		oled_screen.writeCharString(75, 16, (char *)"9");
		oled_screen.writeCharString(95, 16, (char *)"0");
		oled_screen.writeCharString(35, 31, (char *)"4");
		oled_screen.writeCharString(55, 31, (char *)"5");
		oled_screen.writeCharString(75, 31, (char *)"6");
		oled_screen.writeCharString(35, 48, (char *)"1");
		oled_screen.writeCharString(55, 48, (char *)"2");
		oled_screen.writeCharString(75, 48, (char *)"3");
		oled_screen.setFont(pFontDefault);
	}

	if (current_mode == MODE_MULTIMEDIA) {

		oled_screen.OLEDBitmap(0, 36, 16, 12, icon_mute, false, sizeof(icon_mute)/sizeof(uint8_t));
		oled_screen.OLEDBitmap(60, 36, 16, 12, icon_volume, false, sizeof(icon_volume)/sizeof(uint8_t));

		oled_screen.OLEDBitmap(0, 55, 16, 8, icon_prev, false, sizeof(icon_prev)/sizeof(uint8_t));
		oled_screen.OLEDBitmap(55, 55, 16, 8, icon_play, false, sizeof(icon_play)/sizeof(uint8_t));
		oled_screen.OLEDBitmap(65, 55, 16, 8, icon_pause, false, sizeof(icon_pause)/sizeof(uint8_t));
		oled_screen.OLEDBitmap(118, 55, 16, 8, icon_next, false, sizeof(icon_next)/sizeof(uint8_t));
	}

	if (current_mode == MODE_OSU)
	{
		oled_screen.writeCharString(0, 17, (char *)"ESC");
		oled_screen.OLEDBitmap(59, 17, 16, 8, icon_chat, false, sizeof(icon_chat)/sizeof(uint8_t));
		oled_screen.OLEDBitmap(116, 17, 16, 8, icon_restart, false, sizeof(icon_restart)/sizeof(uint8_t));
		// Adicionar um ESC, e a tecla de reiniciar, CHAT
		oled_screen.setFont(pFontGroTesk);
		oled_screen.writeCharString(0, 32, (char *)"Z");
		oled_screen.writeCharString(56, 32, (char *)"X");
		oled_screen.writeCharString(112, 32, (char *)"Z");
		oled_screen.setFont(pFontDefault);
	}

    if (current_mode == MODE_GIT) {
        draw_key_lines();
        const char *keys[3][3] = {
            {nullptr,	"Stash",	"St pop"},
            {"Diff",	"Pull", 	"Push"},
            {"Status",	"Add .",	"Commit"}
        };
        draw_keypad(keys);
    }

    if (current_mode == MODE_DOCEKR) {
        draw_key_lines();
        const char *keys[3][3] = {
            {"Torchic",	"DCU",	"Treecko"},
            {"PS",		"NVIM",	"DCL"},
            {"DCD",		"DCP",	"DCUD"}
        };
        draw_keypad(keys);
    }

    if (current_mode == MODE_ARROWPAD) {
        draw_key_lines();
        const char *keys[3][3] = {
            {"Esc",		"Up",		nullptr},
            {"Left",	"Down", 	"Right"},
            {"L CTRL",	nullptr,	"Space"}
        };
        draw_keypad(keys);
    }

	if (current_mode == MODE_WASD) {
        draw_key_lines();
        const char *keys[3][3] = {
            {"Esc",		"W",		nullptr},
            {"A",		"S", 		"D"},
            {"L CTRL",	nullptr,	"Space"}
        };
        draw_keypad(keys);
    }

	// TODO: WASD mode

	if (current_mode == MODE_IDE) {
        draw_key_lines();
        const char *keys[3][3] = {
            {"Sidebar",	"Comment",	"Impl"},
            {"Termnl",	"Run",		"MovUp"},
            {"DeLine",	"Compile",	"MovDown"}
        };
        draw_keypad(keys);
    }

		if (current_mode == MODE_IDE_2) {
        draw_key_lines();
        const char *keys[3][3] = {
            {nullptr,	nullptr,	"Refs"},
            {nullptr,	nullptr,	"SpMov R"},
            {nullptr,	"Rename",	"Splt R"}
        };
        draw_keypad(keys);
    }

    oled_screen.OLEDupdate();
}

void draw_ui(void)
{
	printf("Drawing UI\r\n");
	oled_screen.fillScreen(BLACK);
	oled_screen.setFont(pFontDefault);
	oled_screen.drawLine(0,15,128,15,WHITE);

	if (usb_mounted)
	{
		oled_screen.OLEDBitmap(109, 3, 16, 9, icon_usb, false, sizeof(icon_usb)/sizeof(uint8_t));
	}

	draw_current_mode();
}

void update_last_interaction(void)
{
	last_interaction_ms = board_millis();

	if (is_in_screensaver_mode) {
		printf("Exiting from screensave mode!\r\n");
		oled_screen.OLEDEnable(1);
		draw_ui();
		is_in_screensaver_mode = false;
	}
}

void setup_oled(void) {
	while(oled_screen.OLEDbegin(i2C_ADDRESS, i2c1,  I2C_SPEED, GPIO_OLED_SDA, GPIO_OLED_SCLK) != true)
	{
		printf("SetupTest ERROR : Failed to initialize OLED!\r\n");
		while(1){busy_wait_ms(1000);}
	} // initialize the OLED

	if (oled_screen.OLEDSetBufferPtr(OLED_WIDTH, OLED_HEIGHT, screen_buffer, sizeof(screen_buffer)/sizeof(uint8_t)) != 0)
	{
		printf("SetupTest : ERROR : OLEDSetBufferPtr Failed!\r\n");
		while(1){busy_wait_ms(1000);}
	} // Initialize the buffer

	draw_ui();
	busy_wait_ms(100);
}

void setup_encoder(void) {
	gpio_init(GPIO_ENCODER_A);
    gpio_set_dir(GPIO_ENCODER_A, GPIO_IN);
    gpio_pull_up(GPIO_ENCODER_A);

    gpio_init(GPIO_ENCODER_B);
    gpio_set_dir(GPIO_ENCODER_B, GPIO_IN);
    gpio_pull_up(GPIO_ENCODER_B);

	encoder.gpio_a = GPIO_ENCODER_A;
    encoder.gpio_b = GPIO_ENCODER_B;
	encoder.min_value = 1;
    encoder.max_value = 5;
    encoder.factor = 1;
    encoder.current_value = 1;
}

uint8_t ascii_to_keycode(char c) {
    // This is a basic example; you'll need a full table for all characters
    if (c >= 'a' && c <= 'z') {
        return HID_KEY_A + (c - 'a');  // 'a' starts from HID_KEY_A
    }
    if (c >= 'A' && c <= 'Z') {
        return HID_KEY_A + (c - 'A');  // Capital letters are the same keycodes
    }
    if (c == ' ') {
        return HID_KEY_SPACE;          // Space key
    }
	if (c == '.') {
		return HID_KEY_PERIOD;
	}
	if (c == '-') {
		return HID_KEY_MINUS;
	}
	if (c == '\"') {
		return HID_KEY_APOSTROPHE;
	}
	if (c == '<') {
		return HID_KEY_ARROW_LEFT;
	}
	if (c == '`') {
		return HID_KEY_GRAVE;
	}
	if (c == '/') {
		return HID_KEY_SLASH;
	}
    // Add more characters as needed
    return 0;
}

void send_string(const char* str, uint8_t modifier) {
    for (const char* p = str; *p != '\0'; p++) {
		// Wait for the host to be ready
		while (!tud_hid_ready()) {
			tud_task();  // TinyUSB device task
		}

        // Get the keycode for the current character
        uint8_t keycode = ascii_to_keycode(*p);

		uint8_t keys_pressed[6] = {0}; 
	
		keys_pressed[0] = keycode;

        if (keycode) {
            // Send key press report (0 is the modifier byte)
            tud_hid_keyboard_report(REPORT_ID_KEYBOARD, modifier, keys_pressed);

            // Delay to simulate keypress duration
            sleep_ms(10);
			tud_task();

			uint8_t empty_keys[6] = {0, 0, 0, 0, 0, 0};
            tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, empty_keys);

			// Slightly longer delay to ensure the key release is registered
            sleep_ms(10);
        }

        str++;
    }
}

void send_string(const char* str)
{
	send_string(str, 0);
}

void send_git_command(bool keys_pressed)
{
	// skip if hid is not ready yet
    if (!keys_pressed)
    {
        return;
    }

	switch (keyboard.keys_pressed[0])
	{
	case GPIO_KEY_7:

		break;
	case GPIO_KEY_8:
		send_string("git stash");
		break;
	case GPIO_KEY_9:
		send_string("git stash pop");
		break;
	case GPIO_KEY_4:
		send_string("git diff");
		break;
	case GPIO_KEY_5:
		send_string("git pull");
		break;
	case GPIO_KEY_6:
		send_string("git push");
		break;
	case GPIO_KEY_1:
		send_string("git status");
		break;
	case GPIO_KEY_2:
		send_string("git add .");
		break;
	case GPIO_KEY_3:
		send_string("git commit -m ");
		send_string("\"\"", KEYBOARD_MODIFIER_LEFTSHIFT);
		send_string("<");
		break;
	default:
		break;
	}
}

void send_docker_command(bool keys_pressed)
{
	// skip if hid is not ready yet
    if (!keys_pressed)
    {
        return;
    }

	switch (keyboard.keys_pressed[0])
	{
	case GPIO_KEY_7:
		send_string("cd ");
		send_string("`", KEYBOARD_MODIFIER_LEFTSHIFT);
		send_string("/composes/torchic");
		break;
	case GPIO_KEY_8:
		send_string("docker compose up");
		break;
	case GPIO_KEY_9:
		send_string("cd ");
		send_string("`", KEYBOARD_MODIFIER_LEFTSHIFT);
		send_string("/composes/treecko");
		break;
	case GPIO_KEY_4:
		send_string("docker ps");
		break;
	case GPIO_KEY_5:
		send_string("nvim compose.yml");
		break;
	case GPIO_KEY_6:
		send_string("docker compose logs -f");
		break;
	case GPIO_KEY_1:
		send_string("docker compose down");
		break;
	case GPIO_KEY_2:
		send_string("docker compose pull");
		break;
	case GPIO_KEY_3:
		send_string("docker compose up -d");
		break;
	default:
		break;
	}
}

static void send_hid_report(bool keys_pressed, uint8_t modifier)
{
    // skip if hid is not ready yet
    if (!tud_hid_ready())
    {
        return;
    }

    // avoid sending multiple zero reports
    static bool send_empty = false;

    if (keys_pressed)
    {
        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, modifier, keyboard.keys_pressed);
        send_empty = true;
		return;
    }

	// send empty key report if previously has key pressed
	if (send_empty)
	{
		tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
	}
	send_empty = false;
}

static void send_hid_report(bool keys_pressed)
{
	return send_hid_report(keys_pressed, 0);
}

static void send_consumer_hid_report(bool keys_pressed)
{
    // skip if hid is not ready yet
    if (!tud_hid_ready())
    {
        return;
    }

	if (keyboard.keys_pressed[0] == GPIO_KEY_5)
	{
		while (keyboard.keys_pressed[0] == GPIO_KEY_5)
		{
			while (!tud_hid_ready()) {
				tud_task();  // TinyUSB device task
			}

			rotary_task(&encoder);
			if (encoder.triggered && encoder.dir != 0)
			{
				uint16_t volume_key = 
					encoder.dir == 1 ? HID_USAGE_CONSUMER_VOLUME_INCREMENT : HID_USAGE_CONSUMER_VOLUME_DECREMENT;

				printf("%i", encoder.dir);

 				tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &volume_key, 2);

				// Delay to simulate keypress duration
				sleep_ms(10);
				tud_task();

				uint16_t empty_key = 0;
				tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &empty_key, 2);

				sleep_ms(50);
			}
			keyboard.update(current_mode);
			
		}
		printf("\r\n");
		return;
	}

    // avoid sending multiple zero reports
    static bool send_empty = false;

    if (keys_pressed)
    {
        tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &keyboard.keys_pressed[0], 2);
		// uint16_t volume_down = HID_USAGE_CONSUMER_VOLUME_DECREMENT;
      	// tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &volume_down, 2);
        send_empty = true;
		return;
    }

	// send empty key report if previously has key pressed
	if (send_empty)
	{
		uint16_t empty_key = 0;
		tud_hid_report(REPORT_ID_CONSUMER_CONTROL, &empty_key, 2);
	}
	send_empty = false;
}

void handle_hid_task(bool const keys_pressed) {
    if (tud_suspended() && keys_pressed)
    {
        // Wake up host if we are in suspend mode
        // and REMOTE_WAKEUP feature is enabled by host
        tud_remote_wakeup();
		return;
    }

	switch (current_mode)
	{
	case MODE_NUMPAD:
	case MODE_OSU:
	case MODE_ARROWPAD:
	case MODE_WASD:
	case MODE_IDE:
		send_hid_report(keys_pressed);
		break;
	case MODE_IDE_2:
		send_hid_report(keys_pressed, KEYBOARD_MODIFIER_LEFTSHIFT);
		break;
	case MODE_MULTIMEDIA:
		send_consumer_hid_report(keys_pressed);
		break;
	case MODE_GIT:
		send_git_command(keys_pressed);
		break;
	case MODE_DOCEKR:
		send_docker_command(keys_pressed);
	default:
		break;
	}
}

void change_current_mode(int8_t direction)
{
	if (direction == 0) {
		return;
	}

	update_last_interaction();

	current_mode += direction;
	if (current_mode == MODE_COUNT) {
		current_mode = 0;
	}

	if (current_mode < 0) {
		current_mode = MODE_COUNT - 1;
	}

	draw_current_mode();
	printf("Changed mode to [%s], count[%i], direction: [%i]\r\n", modes[current_mode], current_mode, direction);
	sleep_ms(150);
}

void keys_task(void)
{
    static uint32_t start_ms = 0;

    if (board_millis() - start_ms < POLL_INTERVAL)
    {
        return; // not enough time
    }

    start_ms += POLL_INTERVAL;

    // Check for keys pressed
    bool const keys_pressed = keyboard.update(current_mode);

	if (keys_pressed)
	{
		update_last_interaction();
	}

	handle_hid_task(keys_pressed);
}

void screensave_task(void)
{
	static uint32_t last_blip_on_ms = 0;
	static uint32_t last_blip_off_ms = 0;
	static bool is_blip_on = false;

	if (is_in_screensaver_mode)
	{
		bool should_turn_on_blip = (board_millis() - last_blip_off_ms > BLIP_FREQUENCY_S * 1000) && !is_blip_on;
		bool should_turn_off_blip = (board_millis() - last_blip_on_ms > BLIP_DURATION_MS) && is_blip_on;

		if (should_turn_on_blip)
		{
			oled_screen.fillScreen(BLACK);
			oled_screen.drawPixel(0, 63, WHITE);
			oled_screen.OLEDEnable(1);
			oled_screen.OLEDupdate();
			last_blip_on_ms = board_millis();
			is_blip_on = true;
			return;
		}

		if (should_turn_off_blip)
		{
			oled_screen.OLEDEnable(0);
			last_blip_off_ms = board_millis();
			is_blip_on = false;
			return;
		}
		return;
	}

	bool should_be_in_screensave = board_millis() - last_interaction_ms > SCREENSAVER_TIME_S * 1000;

	// TODO: But brightness to minimum at half SCREENSAVER_TIME_S

	if (should_be_in_screensave)
	{
		printf("Entering screensave mode\r\n");
		is_in_screensaver_mode = true;
		oled_screen.OLEDEnable(0);
		last_blip_off_ms = board_millis();
	}
}

int main()
{
	stdio_uart_init_full(uart0, 115200, 12, 13);
	
	printf("\r\n\r\n=-=-=- Welcome to TaviscoMacropad! -=-=-=\r\n");
	printf("Setting up OLED\r\n");
	setup_oled();
	
	printf("Setting up Rotary Encoder\r\n");
	setup_encoder();

	printf("Setting up USB stack\r\n");
	tusb_init();

	printf("Ready! Entering main loop\r\n");
	while (true) {
		tud_task();				// tinyusb device task
		rotary_task(&encoder);	// handle encoder rotation

		if (encoder.triggered && encoder.dir != 0) {
			change_current_mode(encoder.dir);
			continue;
		}

		keys_task();			// handle key presses
		screensave_task();
	}
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report, uint8_t len)
{
    // not implemented, we only send REPORT_ID_KEYBOARD
    (void)instance;
    (void)len;
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen)
{
    // TODO not Implemented
    (void)instance;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)reqlen;

    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize)
{
    (void)instance;

}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
	printf("USB is mounted!\r\n");
	usb_mounted = true;
	draw_ui();
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
	oled_screen.fillRect(100, 0, 27, 14, BLACK);
	oled_screen.writeCharString(122, 3, (char *)"N");
	oled_screen.OLEDupdate();
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
    (void)remote_wakeup_en;
	oled_screen.fillRect(100, 0, 27, 14, BLACK);
	oled_screen.writeCharString(122, 3, (char *)"S");
	oled_screen.OLEDupdate();
	oled_screen.OLEDEnable(0);
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
	oled_screen.OLEDEnable(1);
	oled_screen.fillRect(100, 0, 27, 14, BLACK);
	oled_screen.OLEDBitmap(109, 3, 16, 9, icon_usb, false, sizeof(icon_usb)/sizeof(uint8_t));
	oled_screen.OLEDupdate();
}