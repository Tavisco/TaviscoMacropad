#pragma once

// Encoder
#define GPIO_ENCODER_A		28
#define GPIO_ENCODER_B		27

// OLED	
#define GPIO_OLED_SDA		2
#define GPIO_OLED_SCLK		3

// I2C	
#define I2C_SPEED			1000
#define i2C_ADDRESS			0x3C

// Screen settings	
#define OLED_WIDTH			128
#define OLED_HEIGHT			64
#define OLED_SIZE			(OLED_WIDTH * (OLED_HEIGHT/8))

// HID	
#define POLL_INTERVAL		10

// Modes	
#define MODE_COUNT			10

#define MODE_IDE			0
#define MODE_GIT			1
#define MODE_DOCEKR			2
#define MODE_NUMPAD			3
#define MODE_IOT			4
#define MODE_OSU			5
#define MODE_ARROWPAD		6
#define MODE_WASD			7
#define MODE_MULTIMEDIA		8
#define MODE_IDE_2			9


// Preferences - Screensaver
#define SCREENSAVER_TIME_S	60*10
#define BLIP_FREQUENCY_S	3	// blip every X seconds
#define BLIP_DURATION_MS 	750	// blip stays on for X ms

// Bitmaps
const uint8_t icon_usb[] = {
	0x01, 0x80, 0x03, 0xc0, 0x65, 0x84, 0xf4, 0x06, 0xff, 0xff, 0xf1, 0x06, 0x61, 0x74, 0x00, 0xf0, 
	0x00, 0x70
};

const uint8_t icon_play[] = {
	0xe0, 0x00, 0xb8, 0x00, 0x86, 0x00, 0xff, 0x80, 0xff, 0x80, 0xfe, 0x00, 0xf8, 0x00, 0xe0, 0x00
};

const uint8_t icon_pause[] = {
	0xf7, 0x80, 0x94, 0x80, 0xb5, 0x80, 0xf7, 0x80, 0xf7, 0x80, 0xf7, 0x80, 0xf7, 0x80, 0xf7, 0x80
};

const uint8_t icon_next[] = {
	0x00, 0x00, 0xc6, 0x00, 0xe7, 0x00, 0xb5, 0x80, 0xde, 0xc0, 0xf7, 0x80, 0xe7, 0x00, 0xc6, 0x00
};

const uint8_t icon_prev[] = {
	0x00, 0x00, 0x18, 0xc0, 0x39, 0xc0, 0x6b, 0x40, 0xde, 0xc0, 0x7b, 0xc0, 0x39, 0xc0, 0x18, 0xc0
};

const uint8_t icon_volume[] = {
	0x03, 0x80, 0x04, 0x80, 0x0a, 0x80, 0xf6, 0x80, 0x8e, 0x80, 0xbe, 0x80, 0xbe, 0x80, 0x8e, 0x80, 
	0xf6, 0x80, 0x0a, 0x80, 0x04, 0x80, 0x03, 0x80
};

const uint8_t icon_mute [] = {
	0x00, 0x90, 0x01, 0x20, 0x02, 0x40, 0x3c, 0x80, 0x21, 0x20, 0x22, 0x20, 0x24, 0xa0, 0x09, 0xa0, 
	0x11, 0xa0, 0x22, 0xa0, 0x41, 0x20, 0x80, 0xe0
};

const uint8_t icon_chat [] = {
	0x3f, 0x00, 0x40, 0x80, 0x80, 0x40, 0x80, 0x40, 0x40, 0x80, 0x3f, 0x00, 0x30, 0x00, 0x60, 0x00
};

const uint8_t icon_restart [] = {
	0xb8, 0x00, 0xc4, 0x00, 0xe2, 0x00, 0x02, 0x00, 0x02, 0x00, 0x44, 0x00, 0x38, 0x00, 0x00, 0x00
};