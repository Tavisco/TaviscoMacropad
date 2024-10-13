#pragma once

// Encoder
#define GPIO_ENCODER_A		27
#define GPIO_ENCODER_B		28

// OLED	
#define GPIO_OLED_SDA		2
#define GPIO_OLED_SCLK		3

// I2C	
#define I2C_SPEED			800
#define i2C_ADDRESS			0x3C

// Screen settings	
#define OLED_WIDTH			128
#define OLED_HEIGHT			64
#define OLED_SIZE			(OLED_WIDTH * (OLED_HEIGHT/8))

// HID	
#define POLL_INTERVAL		10

// Modes	
#define MODE_COUNT			5

#define MODE_KEYPAD			0
#define MODE_GIT			1
#define MODE_MULTIMEDIA		2
#define MODE_IOT			3
#define MODE_OSU			4

// Preferences - Screensaver
#define SCREENSAVER_TIME_S	120
#define BLIP_FREQUENCY_S	3

// Bitmaps
const uint8_t usb_icon[] = {
	0x01, 0x80, 0x03, 0xc0, 0x65, 0x84, 0xf4, 0x06, 0xff, 0xff, 0xf1, 0x06, 0x61, 0x74, 0x00, 0xf0, 
	0x00, 0x70
};