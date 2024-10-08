#pragma once

// Encoder
#define GPIO_ENCODER_A	28
#define GPIO_ENCODER_B	27

// OLED
#define GPIO_OLED_SDA	2
#define GPIO_OLED_SCLK	3

// I2C
#define I2C_SPEED		800
#define i2C_ADDRESS		0x3C

// Screen settings
#define OLED_WIDTH		128
#define OLED_HEIGHT		64
#define OLED_SIZE		(OLED_WIDTH * (OLED_HEIGHT/8))

// HID
#define POLL_INTERVAL	10

// Modes
#define MODE_COUNT		5

#define MODE_KEYPAD		0
#define MODE_MACROS		1
#define MODE_MULTIMEDIA	2
#define MODE_IOT		3
#define MODE_OSU		4
