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

#include <avr/io.h>
#include <stdbool.h>

/**************************************IO_init*****************************************************
    Initializes IO pin directions.

    PA1:    USART1 TXD:             output
    PA2:    USART1 RXD:             input
    PA3:    RN4871 INIT LED:        output
    PA6:    Analog channel 2:       digital input buffer disabled

    PB2:    USART0 TXD (CDC):       output
    PB6:    rn4871 reset signal:    output

    PC5:    RGBLED:                 output
**************************************************************************************************/
void io_init(void)
{
	PORTA.DIRSET = PIN1_bm | PIN3_bm; /* PA1, PA3 output */
	PORTA.DIRCLR = PIN2_bm; /* PA2 input */
	PORTA.PIN6CTRL |= PORT_ISC_INPUT_DISABLE_gc; /* PA6 analog */

	PORTB.DIRSET = PIN2_bm | PIN6_bm; /* PB6, rn4871 HWreset */

	PORTC.DIRSET = PIN5_bm;  /* CS3, RGBLED */
}

void HWRESET_PIN_set_level(bool state)
{
	if(state)
	{
		PORTB.OUTSET |= PIN6_bm;
	}
	else
	{
		PORTB.OUTCLR |= PIN6_bm;
	}
}
