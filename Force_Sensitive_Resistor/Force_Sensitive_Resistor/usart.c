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
#include "clock_config.h"

#define BAUD_RATE  115200

#define USART_BLE USART1            /* BLE */
#define USART_CDC USART0            /* Virtual COM port (CDC) */


/**************************************usart_ble_init**********************************************
    Initializes USART
    BLE communication. PA1:TX, PA2:RX
    Baud Rate : 115200
**************************************************************************************************/
void usart_ble_init()
{
	USART_BLE.CTRLA =  1 << USART_RXCIE_bp;
	USART_BLE.CTRLB = USART_TXEN_bm |  USART_RXEN_bm;
	USART_BLE.BAUD = (F_CPU * 64.0) / (BAUD_RATE * 16.0);
}

/**************************************usart_CDC_init**********************************************
    Initializes USART
    Virtual COM port (CDC) is connected to USART0 on ATtiny1627 Curiosity Nano board. PB2:TX, PB3:RX
    Baud Rate : 115200
**************************************************************************************************/
void usart_CDC_init()
{
	USART_CDC.CTRLB = USART_TXEN_bm |  USART_RXEN_bm;
	USART_CDC.BAUD = (F_CPU * 64.0) / (BAUD_RATE * 16.0);
}

/**************************************usart_ble_put_c*********************************************
    Transmit data through USART
**************************************************************************************************/
void usart_ble_put_c(uint8_t c)
{
	while(!(USART_BLE.STATUS & USART_DREIF_bm));
	USART_BLE.TXDATAL = c;
}

/**************************************usart_CDC_put_c*********************************************
    Transmit data through USART
**************************************************************************************************/
void usart_CDC_put_c(uint8_t c)
{
	while(!(USART_CDC.STATUS & USART_DREIF_bm));
	USART_CDC.TXDATAL = c;
}

/**************************************usart_ble_get_c*********************************************
    Read data through USART
**************************************************************************************************/
uint8_t usart_ble_get_c()
{
	while(!(USART_BLE.STATUS & USART_RXCIF_bm));
	return USART_BLE.RXDATAL;
}

/**************************************usart_CDC_get_c*********************************************
    Read data through USART
**************************************************************************************************/
uint8_t usart_CDC_get_c()
{
	while(!(USART_CDC.STATUS & USART_RXCIF_bm));
	return USART_CDC.RXDATAL;
}


