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

const uint8_t mode_count = 5;

const char *modes[]= {"Numpad", "Macros", "Multimedia", "IoT", "Osu!"};

uint8_t screen_buffer[OLED_SIZE]; // Define a buffer to cover whole screen  128 * 64/8
SSD1306 oled_screen(OLED_WIDTH, OLED_HEIGHT);
uint8_t current_mode = 0;
KeyBoard keyboard;
rotary_encoder_t encoder;

void keys_task(void);

void draw_current_mode(void) {
	oled_screen.fillRect(0, 0, 64, 14, BLACK);
	oled_screen.writeCharString(0, 3, (char *)modes[current_mode]);
	
	oled_screen.fillRect(0, 16, 128, 48, BLACK);

	if (current_mode == MODE_KEYPAD){
		//TODO: Move this a proper file?
		oled_screen.setFont(pFontMega);
		oled_screen.writeCharString(00, 16, (char *)"7");
		oled_screen.writeCharString(18, 16, (char *)"8");
		oled_screen.writeCharString(36, 16, (char *)"9");
		oled_screen.writeCharString(112, 16, (char *)"0");
		oled_screen.writeCharString(00, 31, (char *)"4");
		oled_screen.writeCharString(18, 31, (char *)"5");
		oled_screen.writeCharString(36, 31, (char *)"6");
		oled_screen.writeCharString(00, 48, (char *)"1");
		oled_screen.writeCharString(18, 48, (char *)"2");
		oled_screen.writeCharString(36, 48, (char *)"3");
		oled_screen.setFont(pFontDefault);
	}

	oled_screen.OLEDupdate();
}

void setup_oled(void) {

	printf("OLED SSD1306 :: Start!\r\n");
	
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
	oled_screen.OLEDFillScreen(0x00, 0);
	oled_screen.setFont(pFontDefault);
	oled_screen.drawLine(0,15,128,15,WHITE);

	draw_current_mode();
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

static void send_hid_report(bool keys_pressed)
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
        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keyboard.keys_pressed);
		tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
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
	case MODE_KEYPAD:
		send_hid_report(keys_pressed);
		break;
	
	default:
		break;
	}
}

void change_current_mode(int8_t direction)
{
	if (direction == 0) {
		return;
	}

	current_mode += direction;
	if (current_mode == mode_count) {
		current_mode = 0;
	}
	if (current_mode < 0) {
		current_mode = mode_count - 1;
	}

	draw_current_mode();
}

// Every 10ms, we poll the pins
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

	if (current_mode == MODE_KEYPAD){
		handle_hid_task(keys_pressed);
	}
}

uint8_t lastMode = 0;

int main()
{
	stdio_init_all();
	busy_wait_ms(250);

	setup_oled();
	setup_encoder();

	tusb_init();

	while (true) {
		tud_task();				// tinyusb device task
		rotary_task(&encoder);	// handle encoder rotation, NOT WORKING
		
		if (encoder.triggered && encoder.dir != 0) {
			change_current_mode(encoder.dir);
			continue;
		}
		keys_task();			// handle key presses
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
	oled_screen.fillRect(100, 0, 27, 14, BLACK);
	oled_screen.writeCharString(122, 3, (char *)"M");
	oled_screen.OLEDupdate();
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
	oled_screen.writeCharString(122, 3, (char *)"R");
	oled_screen.OLEDupdate();
}