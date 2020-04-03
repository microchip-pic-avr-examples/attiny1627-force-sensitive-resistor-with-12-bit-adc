/*
    (c) 2020 Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip software and any
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party
    license terms applicable to your use of third party software (including open source software) that
    may accompany Microchip software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS
    FOR A PARTICULAR PURPOSE.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS
    SOFTWARE.
*/

#include "clock_config.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <util/delay.h>
#include <math.h>
#include <stdlib.h>
#include <avr/cpufunc.h>
#include "io.h"
#include "usart.h"
#include "RGBClick_4x4.h"
#include "rn4871_click.h"


#define TIMEBASE_VALUE ((uint8_t) ceil(F_CPU*0.000001))

#define ADC_MAX_VALUE_for_FORCE_CLICK   0xFCF /* In the test setup, VDD = 3.3V, ADC ref is VDD. 12 bit ADC count is 4095 at 3.3V
                                                 When maximum force is applied on the Force sensor the voltage on ADC pin is 3.261V 
                                                 which gives ADC count ~ 0xFCF. It may vary from setup to setup */

#define SAMPLES  ADC_SAMPNUM_ACC1024_gc /* 1024 samples accumulated */

#define MAX_FORCE_PERCENT   100.0

#define COUNTER_to_CHANGE_COLOR 0x0F

#define MAX_COLORS 7

rgb_led_t rgb_array_configure[RGB_CLICK_NUM_LEDS] = {0};
rgb_led_t rgb_array_output[RGB_CLICK_NUM_LEDS] = {0};
uint8_t output_channel = 0;
uint32_t counter = 0;
float strength_percentage;


uint8_t brightness = 0;
uint8_t leds_to_glow = 0;
uint8_t led_seq_to_glow[RGB_CLICK_NUM_LEDS] = {3, 2, 7, 1, 6, 11, 0, 5, 10, 15, 4, 9, 14, 8, 13, 12}; /* LED numbers on 4x4 RGB Click, array with needed sequence */

/* Colors are configured with 50% intensity. Example 100% intensity for blue color is 0x00, 0x00, 0xFF */
uint8_t led_color_array[MAX_COLORS][NEOPIXEL_CHANNELS] =
{
	{0x00, 0x00, 0x0F}, /* Blue */
	{0x00, 0x0F, 0x00}, /* Red */
	{0x0F, 0x00, 0x00}, /* Green */
	{0x0F, 0x0F, 0x00}, /* Yellow */
	{0x00, 0x0F, 0x0F}, /* Magenta */
	{0x0F, 0x00, 0x0F}, /* Cyan */
	{0x0F, 0x0F, 0x0F}  /* White */
};
uint8_t change_color_index = 0;


struct
{
	uint32_t adc_result;
	uint16_t adc_sample;
	uint16_t adc_average_result;
} adc_t;


/**************************************send_string_to_USART_CDC************************************
    Send character through USART until null char.
    Send string over USART
**************************************************************************************************/
void send_string_to_USART_CDC(char *d)
{
	uint8_t i = 0;
	do
	{
		usart_CDC_put_c(*(d + i));
		i++;
	}
	while(*(d + i) != 0);
}

/**************************************adc_init****************************************************
    ADC initialization
**************************************************************************************************/
void adc_init()
{
	ADC0.CTRLA = ADC_ENABLE_bm;

#if F_CPU == 20000000ul
	ADC0.CTRLB = ADC_PRESC_DIV8_gc; /* CLK_ADC: 2.5 MHz*/
#elif F_CPU == 10000000ul
	ADC0.CTRLB = ADC_PRESC_DIV4_gc; /* CLK_ADC: 2.5 MHz*/
#endif

	ADC0.CTRLC = (ADC_REFSEL_VDD_gc | TIMEBASE_VALUE << ADC_TIMEBASE_gp); /* ADC reference VDD */

	ADC0.CTRLE = 20; /* SAMPDUR */
	ADC0.CTRLF = ADC_SAMPNUM_ACC1024_gc | ADC_FREERUN_bm; /* 1024 sample accumulation, ADC free running */
	ADC0.MUXPOS = ADC_MUXPOS_AIN6_gc; /* ADC channel AIN6 -> PA6 */
	ADC0.COMMAND = ADC_MODE_BURST_gc; /* ADC Burst Accumulation mode */

}

uint16_t adc_get_sample(void)
{
	return ADC0.SAMPLE;
}
uint32_t adc_get_result(void)
{
	return ADC0.RESULT;
}

bool adc_result_is_ready(void)
{
	return (ADC0.INTFLAGS & ADC_RESRDY_bm);
}
bool adc_sample_is_ready(void)
{
	return (ADC0.INTFLAGS & ADC_SAMPRDY_bm);
}


ISR(USART1_RXC_vect)
{
	RN4871_CaptureReceivedMessage();
}

/**************************************transmit_to_BLE*********************************************
    Convert float number(calculated strength) to string and send to USART, (BLE communication)
**************************************************************************************************/
void transmit_to_BLE(float number)
{
	char string[50];

	dtostrf(number, 4, 4, string);

	RN4871_sendAndWait("Stress ball Strength:", "", 0);
	RN4871_sendAndWait(string, "", 0);
	RN4871_sendAndWait("%\n", "", 0);
}

/**************************************trasmit_to_terminal*****************************************
    Convert float number(calculated strength) to string and send to USART, (PC serial terminal)
**************************************************************************************************/
void trasmit_to_terminal(float number)
{
	char string[50];

	dtostrf(number, 4, 4, string);
	send_string_to_USART_CDC("Stress ball Strength:");
	send_string_to_USART_CDC(string);
	send_string_to_USART_CDC("%\n");
}

/**************************************rgb_pattern_MixColor****************************************
    Different LEDs with different colors, set color for 16 LEDs by
    filling array rgb_array_output[0-15] with different colors
**************************************************************************************************/
void rgb_pattern_MixColor()
{
	uint8_t i, j = 0;
	for(i = 0; i <= RGB_CLICK_NUM_LEDS - 1; i++)
	{
		rgb_array_output[i].green = led_color_array[j][0];
		rgb_array_output[i].red = led_color_array[j][1];
		rgb_array_output[i].blue = led_color_array[j][2];
		j++;
		if(j >= MAX_COLORS)
		{
			j = 0;
		}
	}
	rgb_update(rgb_array_output, RGB_CLICK_NUM_LEDS);
	_delay_ms(1000);
}

/**************************************rgb_pattern_Red_Green_White*********************************
   Same color for all LEDS, color is configured in rgb_array_output[0].
   Colors configured are RED,Green, White
**************************************************************************************************/
void rgb_pattern_Red_Green_White()
{
	//RED
	rgb_array_output[0].green = 0x00;
	rgb_array_output[0].red = 0x0F;
	rgb_array_output[0].blue = 0x00;
	rgb_update_single_color(rgb_array_output[0], RGB_CLICK_NUM_LEDS);
	_delay_ms(500);
	//GREEN
	rgb_array_output[0].green = 0x0F;
	rgb_array_output[0].red = 0x00;
	rgb_array_output[0].blue = 0x00;
	rgb_update_single_color(rgb_array_output[0], RGB_CLICK_NUM_LEDS);
	_delay_ms(500);
	//WHITE
	rgb_array_output[0].green = 0x0F;
	rgb_array_output[0].red = 0x0F;
	rgb_array_output[0].blue = 0x0F;
	rgb_update_single_color(rgb_array_output[0], RGB_CLICK_NUM_LEDS);

}

/**************************************rgb_clear_all_leds******************************************
   Clear output array.
**************************************************************************************************/
void rgb_clear_all_leds()
{
	uint8_t i;

	for(i = 0; i < RGB_CLICK_NUM_LEDS; i++)
	{
		rgb_array_output[i].green = 0;
		rgb_array_output[i].blue = 0;
		rgb_array_output[i].red = 0;
	}
}

/**************************************rgb_display_pattern_per_force*******************************
    Calculate brightness as per applied force by reading adc_t.adc_average_result and right shift(12bit ADC result) by 4 bits as brightness is 8 bit variable.
    Calculate leds_to_glow as per applied force. Maximum 16 LEDS to lit.
    When leds_to_glow < 16, fill array output only for required LEDs with blue color and brightness.
    when leds_to_glow =16, 100% strength, fill array output  for all LEDs with different colors brightness.
    index to change the color is changed in main after certain delay which is defined by COUNTER_to_CHNAGE_COLOR.
**************************************************************************************************/
void rgb_display_pattern_per_force()
{
	uint8_t i;
	brightness = (uint8_t)(adc_t.adc_average_result >> 4);  /* Brightness as per ADC result. For RGB LEDs, brightness is 8 bit so shift right the 12bit ADC result by 4 */
	leds_to_glow = (adc_t.adc_average_result * (RGB_CLICK_NUM_LEDS - 1)) / ADC_MAX_VALUE_for_FORCE_CLICK; /* As per ADC result, maximum LEDS to glow = 16*/
	/* When leds_to_glow = 15, All 16 LEDs will be lit */
	rgb_clear_all_leds();
	if(leds_to_glow != RGB_CLICK_NUM_LEDS - 1) /* Force is not 100% so number of LEDs to glow are less than 16 */
	{
		for(i = 0; i <= leds_to_glow ; i++) /* Fill needed LEDs with color and brightness as per force */
		{
			rgb_array_output[led_seq_to_glow[i]].green = 0x00;
			rgb_array_output[led_seq_to_glow[i]].red =  0;
			rgb_array_output[led_seq_to_glow[i]].blue = (0xFF & brightness);
		}

	}
	else /* When 100% strength, fill LEDs with different colors and turn ON all 16 LEDS */
	{
		counter++;
		if(counter >= COUNTER_to_CHANGE_COLOR)
		{
			counter = 0;
			change_color_index++;
			if(change_color_index >= MAX_COLORS)
			{
				change_color_index = 0;
			}
		}
		for(i = 0; i <= leds_to_glow ; i++)
		{
			rgb_array_output[led_seq_to_glow[i]].green = led_color_array[change_color_index][0] ;
			rgb_array_output[led_seq_to_glow[i]].red  =  led_color_array[change_color_index][1] ;
			rgb_array_output[led_seq_to_glow[i]].blue =  led_color_array[change_color_index][2] ;
		}

	}
	rgb_update(rgb_array_output, RGB_CLICK_NUM_LEDS);
}

/**************************************main********************************************************
    Initialize peripherals.
    Display RGB LEDs colors.
    Set up RN4871/70 in UART Transparent service.
    Configure ADC for free running and start conversion.
    When ADC result is ready(after accumulating 1024 samples), read accumulated result,
    calculate average result,calculate % strength and send % strength to USART0 and USART1
    to transmit to Bluetooth and PC terminal.
    Update RGB LEDs color and brightness as per applied force on Force sensitive resistor on Force click.
**************************************************************************************************/
int main(void)
{
#if F_CPU == 20000000ul
	ccp_write_io((void *) & (CLKCTRL.MCLKCTRLB), 0);
#elif F_CPU == 10000000ul
	ccp_write_io((void *) & (CLKCTRL.MCLKCTRLB), CLKCTRL_PDIV_2X_gc | 1 << CLKCTRL_PEN_bp);
#endif

	io_init();
	adc_init();
	usart_ble_init();
	usart_CDC_init();
	sei();
	rgb_pattern_MixColor();
	rgb_pattern_Red_Green_White();

	RN4871_Setup_Transparent_UART_service();

	ADC0.COMMAND |= ADC_START_IMMEDIATE_gc; /* Start free running ADC */

	while(1)
	{
		if(adc_result_is_ready())
		{
			adc_t.adc_result = adc_get_result();
			adc_t.adc_average_result = adc_t.adc_result >> SAMPLES;
			strength_percentage  = (float)(adc_t.adc_average_result * MAX_FORCE_PERCENT) / ADC_MAX_VALUE_for_FORCE_CLICK;

			transmit_to_BLE(strength_percentage);
			trasmit_to_terminal(strength_percentage);
			rgb_display_pattern_per_force();
		}

	}
}



