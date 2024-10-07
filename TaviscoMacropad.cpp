#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "TaviscoMacropad.hpp"
#include "libs/SSD1306_OLED_PICO/include/ssd1306/SSD1306_OLED.hpp"
#include "libs/RP2040-Button/button.h"

const uint8_t mode_count = 2;

char *modes[]= {"Multimedia", "Osu!"};

uint8_t screen_buffer[OLED_SIZE]; // Define a buffer to cover whole screen  128 * 64/8
SSD1306 oled_screen(OLED_WIDTH, OLED_HEIGHT);
uint8_t current_mode = 0;

void draw_current_mode(void) {
	oled_screen.fillRect(0, 0, 64, 14, BLACK);
	oled_screen.writeCharString(0, 3, modes[current_mode]);
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
	busy_wait_ms(200);
}

void handleButton(button_t *button_p) {
  button_t *button = (button_t*)button_p;
  //printf("Button on pin %d changed its state to %d\n", button->pin, button->state);

  if(button->state) return; // Ignore button release. Invert the logic if using
                            // a pullup (internal or external).

  switch(button->pin){
    case GPIO_ENCODER_SW:
        printf("Changing mode\n");
        if (current_mode + 1 == mode_count) {
			current_mode = 0;
		} else {
			current_mode++;
		}
    break;
	// case GPIO_SW:
		//send_key(HID_KEY_A);  // Send key 'A'
		// sleep_ms(500);
		// break;
  }

  draw_current_mode();
}

void setup_button(void)
{
	create_button(GPIO_ENCODER_SW, handleButton);
	//create_button(GPIO_SW, handleButton);
}



int main()
{
    stdio_init_all();
    busy_wait_ms(500);

	setup_oled();
	setup_button();
    
    // For more examples of UART use see https://github.com/raspberrypi/pico-examples/tree/master/uart

    while (true) {
        // tud_task();
		busy_wait_ms(500);
    }
}
