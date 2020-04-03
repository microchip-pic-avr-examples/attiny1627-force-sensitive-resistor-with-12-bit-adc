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
#include <util/delay.h>

#include "RGBClick_4x4.h"


#define T1H 700
#define T1L 600
#define T0H 350
#define T0L 800


#define NS_PER_CYCLE (1000000000/F_CPU)
#define MINIMUM_LOW_TIMING (13 * NS_PER_CYCLE) //CBI + loop subtracted - Found by singlestepping..
#define _delay_one_high()       _delay_us((T1H - NS_PER_CYCLE)/1000.0)  //SBI cycle subtracted

#if (T1L > MINIMUM_LOW_TIMING)
#define _delay_one_low()     _delay_us((T1L - MINIMUM_LOW_TIMING)/1000.0) //CBI + loop subtracted
#else
#define _delay_one_low()
#endif

#define _delay_zero_high()      _delay_us((T0H - NS_PER_CYCLE)/1000.0)  //SBI cycle subtracted
#if (T0L > MINIMUM_LOW_TIMING)
#   define _delay_zero_low()    _delay_us((T0L - MINIMUM_LOW_TIMING)/1000.0)    //CBI + loop subtracted
#else
#   define _delay_zero_low()
#endif


#define rgb_led_set_high()  RGB_LED_VPORT.OUT |= RGB_LED_bm;
#define rgb_led_set_low()   RGB_LED_VPORT.OUT &= ~RGB_LED_bm;

static void reset(void)
{
	rgb_led_set_low()
	_delay_us(50);
}

/**************************************rgb_update_single*******************************************
    Update single LED
**************************************************************************************************/
static void rgb_update_single(rgb_led_t led)
{
	for(uint8_t j = 0; j < NEOPIXEL_CHANNELS; j++)
	{
		for(uint8_t k = 0; k < 8; k++)
		{
			if(led.channel[j] & 0x80)
			{
				rgb_led_set_high()
				_delay_one_high();
				rgb_led_set_low()
				_delay_one_low();
			}
			else
			{
				rgb_led_set_high()
				_delay_zero_high();
				rgb_led_set_low()
				_delay_zero_low();
			}
			led.channel[j] <<= 1;
		}
	}
}

/**************************************rgb_update**************************************************
    Update different leds with different color
**************************************************************************************************/
void rgb_update(const rgb_led_t *led_string, uint16_t len)
{
	for(uint16_t i = 0; i < len; i++)
	{
		rgb_update_single(led_string[i]);
	}
	reset();
}

/**************************************rgb_update_single_color*************************************
    Same color is set for all LEDS upto len.
    e.g. when len= 4, first 4 leds are ON
**************************************************************************************************/
void rgb_update_single_color(rgb_led_t led_string, uint16_t len)
{
	for(uint16_t i = 0; i < len; i++)
	{
		rgb_update_single(led_string);
	}
	reset();
}